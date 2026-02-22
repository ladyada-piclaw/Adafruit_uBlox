/*!
 * @file 01_cfg_pm2_test.ino
 *
 * Message test: Poll UBX-CFG-PM2 (Extended Power Management Configuration)
 * and display current settings.
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

  Serial.println(F("=== UBX-CFG-PM2 Message Test ==="));

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
  UBX_CFG_PM2_t pm2;

  if (!ubx.pollCfgPm2(&pm2)) {
    Serial.println(F("CFG-PM2 poll failed (timeout)"));
    delay(5000);
    return;
  }

  Serial.println(F("--- CFG-PM2 ---"));
  Serial.print(F("Version: "));
  Serial.println(pm2.version);
  Serial.print(F("Update period: "));
  Serial.print(pm2.updatePeriod);
  Serial.println(F(" ms"));
  Serial.print(F("Search period: "));
  Serial.print(pm2.searchPeriod);
  Serial.println(F(" ms"));
  Serial.print(F("On time: "));
  Serial.print(pm2.onTime);
  Serial.println(F(" s"));
  Serial.print(F("Min acquisition time: "));
  Serial.print(pm2.minAcqTime);
  Serial.println(F(" s"));
  Serial.print(F("Flags: 0x"));
  Serial.println(pm2.flags, HEX);
  Serial.print(F("  Mode: "));
  Serial.println((pm2.flags & UBX_PM2_FLAG_MODE_CYCLIC) ? F("Cyclic (PSMCT)") : F("ON/OFF (PSMOO)"));

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
