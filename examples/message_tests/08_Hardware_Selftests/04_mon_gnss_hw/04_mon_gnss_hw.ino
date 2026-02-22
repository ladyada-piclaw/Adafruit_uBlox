/*!
 * @file 04_mon_gnss_hw.ino
 * @brief Hardware test for MON-GNSS (GNSS System Information)
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

  Serial.println(F("=== MON-GNSS Hardware Test ==="));

  if (!ddc.begin()) {
    halt(F("GPS not found on I2C"));
  }
  Serial.println(F("GPS module connected"));

  if (!ubx.begin()) {
    halt(F("UBX parser init failed"));
  }

  delay(500);
  ubx.setUBXOnly(UBX_PORT_DDC, true, 1000);

  UBX_MON_GNSS_t gnss;
  if (ubx.pollMonGnss(&gnss)) {
    Serial.println(F("PASS: MON-GNSS poll OK"));
    Serial.print(F("  Supported: "));
    printGnss(gnss.supported);
    Serial.println();
    Serial.print(F("  Enabled: "));
    printGnss(gnss.enabled);
    Serial.println();
    Serial.print(F("  Max simultaneous: "));
    Serial.println(gnss.simultaneous);
  } else {
    Serial.println(F("FAIL: MON-GNSS poll failed"));
  }

  Serial.println();
}

void loop() {
  UBX_MON_GNSS_t gnss;

  if (!ubx.pollMonGnss(&gnss)) {
    Serial.println(F("MON-GNSS poll failed (timeout)"));
    delay(5000);
    return;
  }

  Serial.print(F("enabled="));
  printGnss(gnss.enabled);
  Serial.print(F("max="));
  Serial.println(gnss.simultaneous);

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

void printGnss(uint8_t bits) {
  if (bits & UBX_MON_GNSS_GPS)
    Serial.print(F("GPS "));
  if (bits & UBX_MON_GNSS_GLONASS)
    Serial.print(F("GLO "));
  if (bits & UBX_MON_GNSS_BEIDOU)
    Serial.print(F("BDS "));
  if (bits & UBX_MON_GNSS_GALILEO)
    Serial.print(F("GAL "));
}
