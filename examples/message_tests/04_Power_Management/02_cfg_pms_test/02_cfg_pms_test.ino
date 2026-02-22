/*!
 * @file 02_cfg_pms_test.ino
 *
 * Message test: Poll and set UBX-CFG-PMS (Power Mode Setup)
 * Tests various power modes without actually entering power save.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 */

#include <Adafruit_UBX.h>
#include <Adafruit_UBloxDDC.h>

Adafruit_UBloxDDC ddc;
Adafruit_UBX ubx(ddc);

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== UBX-CFG-PMS Message Test ==="));

  if (!ddc.begin()) {
    halt(F("Could not connect to GPS module on I2C"));
  }
  Serial.println(F("GPS module connected on I2C"));

  if (!ubx.begin()) {
    halt(F("UBX parser init failed"));
  }

  delay(500);

  UBXSendStatus status = ubx.setUBXOnly(UBX_PORT_DDC, true, 1000);
  if (status != UBX_SEND_SUCCESS) {
    Serial.print(F("WARNING: setUBXOnly status: "));
    Serial.println(status);
  } else {
    Serial.println(F("UBX-only mode set on DDC port"));
  }

  Serial.println();
}

void loop() {
  UBX_CFG_PMS_t pms;

  if (!ubx.pollCfgPms(&pms)) {
    Serial.println(F("CFG-PMS poll failed (timeout)"));
    delay(5000);
    return;
  }

  Serial.println(F("--- CFG-PMS ---"));
  Serial.print(F("Version: "));
  Serial.println(pms.version);
  Serial.print(F("Power mode: "));
  Serial.print(pms.powerSetupValue);
  Serial.print(F(" ("));
  Serial.print(getPowerModeName(pms.powerSetupValue));
  Serial.println(F(")"));
  Serial.print(F("Period: "));
  Serial.print(pms.period);
  Serial.println(F(" s"));
  Serial.print(F("On time: "));
  Serial.print(pms.onTime);
  Serial.println(F(" s"));

  Serial.println();
  delay(5000);
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

const char *getPowerModeName(uint8_t mode) {
  switch (mode) {
    case UBX_PMS_FULLPOWER:
      return "Full Power";
    case UBX_PMS_BALANCED:
      return "Balanced";
    case UBX_PMS_INTERVAL:
      return "Interval";
    case UBX_PMS_AGGRESSIVE_1HZ:
      return "Aggressive 1Hz";
    case UBX_PMS_AGGRESSIVE_2HZ:
      return "Aggressive 2Hz";
    case UBX_PMS_AGGRESSIVE_4HZ:
      return "Aggressive 4Hz";
    case UBX_PMS_INVALID:
      return "Invalid";
    default:
      return "Unknown";
  }
}
