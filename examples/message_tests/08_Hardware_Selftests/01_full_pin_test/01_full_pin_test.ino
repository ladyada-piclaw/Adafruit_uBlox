/*!
 * @file 01_full_pin_test.ino
 *
 * Comprehensive hardware pin test for SAM-M8Q GPS module.
 * Tests I2C, UART, Reset, PPS, and EXTINT pins.
 *
 * Pin connections:
 *   A0 -> RESET (active low)
 *   A1 -> PPS   (1Hz pulse with fix)
 *   A2 -> EXTINT (interrupt/wakeup)
 *   Serial1 TX/RX -> SAM-M8Q UART RX/TX
 *   Wire (I2C) -> DDC (0x42)
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 */

#include <Adafruit_UBX.h>
#include <Adafruit_UBloxDDC.h>

#define PIN_RESET A0
#define PIN_PPS A1
#define PIN_EXTINT A2

Adafruit_UBloxDDC ddc;
Adafruit_UBX ubx(ddc);

int passCount = 0;
int failCount = 0;
int skipCount = 0;

void setup() {
  // Drive RESET high IMMEDIATELY
  pinMode(PIN_RESET, OUTPUT);
  digitalWrite(PIN_RESET, HIGH);

  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F(""));
  Serial.println(F("========================================"));
  Serial.println(F("  SAM-M8Q Full Pin Test"));
  Serial.println(F("========================================"));
  Serial.println(F(""));

  pinMode(PIN_PPS, INPUT);
  pinMode(PIN_EXTINT, INPUT);

  Serial.println(F("Waiting 2s for module startup..."));
  delay(2000);

  // Test 1: I2C Communication
  Serial.println(F("--- Test 1: I2C Communication ---"));
  bool i2cOk = testI2C();
  Serial.println();

  // Test 2: Hardware Reset
  Serial.println(F("--- Test 2: Hardware Reset ---"));
  testReset(i2cOk);
  Serial.println();

  // Test 3: UART Auto-Baud Detection
  Serial.println(F("--- Test 3: UART Auto-Baud Detection ---"));
  uint32_t detectedBaud = testUartAutoBaud();
  Serial.println();

  // Test 4: UART Communication
  Serial.println(F("--- Test 4: UART Communication ---"));
  testUartComm(detectedBaud);
  Serial.println();

  // Test 5: PPS Signal Detection
  Serial.println(F("--- Test 5: PPS Signal ---"));
  testPPS(i2cOk);
  Serial.println();

  // Test 6: EXTINT Pin
  Serial.println(F("--- Test 6: EXTINT Pin ---"));
  testExtint();
  Serial.println();

  // Summary
  Serial.println(F("========================================"));
  Serial.println(F("  SUMMARY"));
  Serial.println(F("========================================"));
  Serial.print(F("  PASS: "));
  Serial.println(passCount);
  Serial.print(F("  FAIL: "));
  Serial.println(failCount);
  Serial.print(F("  SKIP: "));
  Serial.println(skipCount);
  Serial.println(F("========================================"));
}

void loop() {
  delay(10000);
}

/**************************************************************************/
/* Helper functions                                                       */
/**************************************************************************/

void halt(const __FlashStringHelper *msg) {
  Serial.print(F("HALT: "));
  Serial.println(msg);
  while (1)
    delay(10);
}

void printResult(const __FlashStringHelper *name, const char *result) {
  Serial.print(F("["));
  Serial.print(result);
  Serial.print(F("] "));
  Serial.println(name);
  if (strcmp(result, "PASS") == 0)
    passCount++;
  else if (strcmp(result, "FAIL") == 0)
    failCount++;
  else
    skipCount++;
}

bool testI2C() {
  if (!ddc.begin()) {
    Serial.println(F("  Could not connect to GPS on I2C"));
    printResult(F("I2C connection"), "FAIL");
    return false;
  }
  Serial.println(F("  GPS module found on I2C 0x42"));

  if (!ubx.begin()) {
    Serial.println(F("  UBX parser init failed"));
    printResult(F("I2C communication"), "FAIL");
    return false;
  }

  ubx.setUBXOnly(UBX_PORT_DDC, true, 1000);
  delay(100);

  UBX_NAV_PVT_t pvt;
  if (ubx.poll(UBX_CLASS_NAV, UBX_NAV_PVT, (uint8_t *)&pvt, sizeof(pvt), 3000)) {
    Serial.print(F("  iTOW: "));
    Serial.print(pvt.iTOW);
    Serial.print(F(" ms, fixType: "));
    Serial.println(pvt.fixType);
    printResult(F("I2C communication"), "PASS");
    return true;
  }

  Serial.println(F("  NAV-PVT poll failed"));
  printResult(F("I2C communication"), "FAIL");
  return false;
}

void testReset(bool i2cOk) {
  if (!i2cOk) {
    Serial.println(F("  Skipping (I2C not working)"));
    printResult(F("hardware reset"), "SKIP");
    return;
  }

  Serial.println(F("  Pulling RESET low for 100ms..."));
  digitalWrite(PIN_RESET, LOW);
  delay(100);
  digitalWrite(PIN_RESET, HIGH);
  Serial.println(F("  Released RESET, waiting 1.5s for reboot..."));
  delay(1500);

  ddc.begin();
  ubx.begin();
  ubx.setUBXOnly(UBX_PORT_DDC, true, 1000);
  delay(100);

  UBX_NAV_PVT_t pvt;
  if (ubx.poll(UBX_CLASS_NAV, UBX_NAV_PVT, (uint8_t *)&pvt, sizeof(pvt), 3000)) {
    Serial.print(F("  Module responded after reset, iTOW: "));
    Serial.println(pvt.iTOW);
    printResult(F("hardware reset"), "PASS");
  } else {
    Serial.println(F("  Module did not respond after reset"));
    printResult(F("hardware reset"), "FAIL");
  }
}

bool uartProbe(uint32_t baud) {
  Serial1.end();
  Serial1.begin(baud);
  delay(100);
  while (Serial1.available())
    Serial1.read();

  uint8_t poll[] = {0xB5, 0x62, 0x01, 0x07, 0x00, 0x00, 0x08, 0x19};
  Serial1.write(poll, sizeof(poll));
  Serial1.flush();

  unsigned long start = millis();
  while (millis() - start < 2000) {
    if (Serial1.available() >= 2) {
      uint8_t b1 = Serial1.read();
      if (b1 == 0xB5 && Serial1.available()) {
        uint8_t b2 = Serial1.read();
        if (b2 == 0x62)
          return true;
      }
    }
  }
  return false;
}

uint32_t testUartAutoBaud() {
  uint32_t bauds[] = {9600, 57600, 115200};
  for (int i = 0; i < 3; i++) {
    Serial.print(F("  Trying "));
    Serial.print(bauds[i]);
    Serial.print(F(" baud... "));
    if (uartProbe(bauds[i])) {
      Serial.println(F("RESPONSE"));
      Serial.print(F("  Detected baud rate: "));
      Serial.println(bauds[i]);
      printResult(F("UART auto-baud"), "PASS");
      return bauds[i];
    }
    Serial.println(F("no response"));
  }
  Serial.println(F("  No response at any baud rate"));
  printResult(F("UART auto-baud"), "FAIL");
  return 0;
}

void testUartComm(uint32_t baud) {
  if (baud == 0) {
    Serial.println(F("  Skipping (no UART baud detected)"));
    printResult(F("UART communication"), "SKIP");
    return;
  }

  Serial1.begin(baud);
  delay(100);
  while (Serial1.available())
    Serial1.read();

  uint8_t poll[] = {0xB5, 0x62, 0x01, 0x07, 0x00, 0x00, 0x08, 0x19};
  Serial1.write(poll, sizeof(poll));
  Serial1.flush();

  // Simple check for response
  unsigned long start = millis();
  bool gotResponse = false;
  while (millis() - start < 3000) {
    if (Serial1.available() >= 6) {
      gotResponse = true;
      break;
    }
  }

  if (gotResponse) {
    Serial.println(F("  UART response received"));
    printResult(F("UART communication"), "PASS");
  } else {
    Serial.println(F("  Failed to read over UART"));
    printResult(F("UART communication"), "FAIL");
  }
}

void testPPS(bool i2cOk) {
  bool hasFix = false;
  if (i2cOk) {
    UBX_NAV_PVT_t pvt;
    if (ubx.poll(UBX_CLASS_NAV, UBX_NAV_PVT, (uint8_t *)&pvt, sizeof(pvt), 3000)) {
      hasFix = (pvt.fixType >= 2);
      Serial.print(F("  Fix type: "));
      Serial.print(pvt.fixType);
      Serial.print(F(", Satellites: "));
      Serial.println(pvt.numSV);
    }
  }

  if (!hasFix) {
    Serial.println(F("  No fix — PPS requires fix to pulse"));
    printResult(F("PPS signal"), "SKIP");
    return;
  }

  Serial.println(F("  Counting PPS edges over 3 seconds..."));
  int edgeCount = 0;
  bool lastState = digitalRead(PIN_PPS);
  unsigned long start = millis();

  while (millis() - start < 3500) {
    bool state = digitalRead(PIN_PPS);
    if (state && !lastState)
      edgeCount++;
    lastState = state;
  }

  Serial.print(F("  Rising edges detected: "));
  Serial.println(edgeCount);

  if (edgeCount >= 2 && edgeCount <= 5) {
    printResult(F("PPS signal"), "PASS");
  } else {
    printResult(F("PPS signal"), "FAIL");
  }
}

void testExtint() {
  bool extState = digitalRead(PIN_EXTINT);
  Serial.print(F("  EXTINT idle state: "));
  Serial.println(extState ? F("HIGH") : F("LOW"));
  Serial.println(F("  (INFO only — EXTINT used for wakeup from power save)"));
  printResult(F("EXTINT readable"), "PASS");
}
