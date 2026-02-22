/*!
 * @file 02_cfg_rst_test.ino
 *
 * Message test: Send UBX-CFG-RST hot start and validate reconnection.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 */

#include <Adafruit_UBX.h>
#include <Adafruit_UBloxDDC.h>

Adafruit_UBloxDDC ddc;
Adafruit_UBX ubx(ddc);

bool tests_run = false;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== UBX-CFG-RST Message Test ==="));

  if (!initModule()) {
    halt(F("Could not connect to GPS module on I2C"));
  }
  Serial.println(F("GPS module connected on I2C"));

  runTests();
  tests_run = true;
}

void loop() {
  UBX_NAV_STATUS_t status;

  if (!ubx.poll(UBX_CLASS_NAV, UBX_NAV_STATUS, &status, sizeof(status))) {
    Serial.println(F("NAV-STATUS poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.print(F("NAV-STATUS: gpsFix="));
  Serial.print(status.gpsFix);
  Serial.print(F(", ttff="));
  Serial.print(status.ttff);
  Serial.print(F(" ms, msss="));
  Serial.print(status.msss);
  Serial.println(F(" ms"));

  delay(2000);
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

void printTestResult(const __FlashStringHelper *name, bool pass) {
  Serial.print(F("  ["));
  Serial.print(pass ? F("PASS") : F("FAIL"));
  Serial.print(F("] "));
  Serial.print(name);
  Serial.print(F(": "));
}

bool initModule() {
  if (!ddc.begin()) {
    return false;
  }
  if (!ubx.begin()) {
    return false;
  }
  UBXSendStatus status = ubx.setUBXOnly(UBX_PORT_DDC, true, 1000);
  if (status != UBX_SEND_SUCCESS) {
    Serial.print(F("WARNING: setUBXOnly status: "));
    Serial.println(status);
  }
  return true;
}

bool reconnectModule(uint32_t timeout_ms) {
  uint32_t start_ms = millis();
  while (millis() - start_ms < timeout_ms) {
    if (initModule()) {
      return true;
    }
    delay(200);
  }
  return false;
}

void runTests() {
  uint8_t passed = 0;
  const uint8_t total = 2;

  Serial.println();
  Serial.println(F("Running tests..."));

  bool hot_start_sent = ubx.hotStart();
  printTestResult(F("hot_start_sent"), hot_start_sent);
  Serial.println(hot_start_sent ? F("OK") : F("BAD"));
  if (hot_start_sent)
    passed++;

  Serial.println(F("Waiting for module reset..."));
  delay(3000);

  bool module_reconnected = reconnectModule(10000);
  printTestResult(F("module_reconnected"), module_reconnected);
  Serial.println(module_reconnected ? F("OK") : F("BAD"));
  if (module_reconnected)
    passed++;

  Serial.println();
  Serial.print(F("Results: "));
  Serial.print(passed);
  Serial.print(F("/"));
  Serial.print(total);
  Serial.println(F(" tests passed"));
}
