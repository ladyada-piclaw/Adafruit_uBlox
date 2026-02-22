/*!
 * @file 04_mon_gnss_test.ino
 *
 * Message test: Poll UBX-MON-GNSS (GNSS System Information)
 * Displays supported, default, and enabled GNSS systems.
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

  Serial.println(F("=== UBX-MON-GNSS Message Test ==="));

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
  UBX_MON_GNSS_t gnss;

  if (!ubx.pollMonGnss(&gnss)) {
    Serial.println(F("MON-GNSS poll failed (timeout)"));
    delay(5000);
    return;
  }

  Serial.println(F("--- MON-GNSS ---"));

  Serial.print(F("Version: "));
  Serial.println(gnss.version);

  Serial.print(F("Supported: "));
  printGnssBits(gnss.supported);
  Serial.print(F(" (0x"));
  Serial.print(gnss.supported, HEX);
  Serial.println(F(")"));

  Serial.print(F("Default: "));
  printGnssBits(gnss.defaultGnss);
  Serial.print(F(" (0x"));
  Serial.print(gnss.defaultGnss, HEX);
  Serial.println(F(")"));

  Serial.print(F("Enabled: "));
  printGnssBits(gnss.enabled);
  Serial.print(F(" (0x"));
  Serial.print(gnss.enabled, HEX);
  Serial.println(F(")"));

  Serial.print(F("Max simultaneous: "));
  Serial.println(gnss.simultaneous);

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

void printGnssBits(uint8_t bits) {
  if (bits & UBX_MON_GNSS_GPS)
    Serial.print(F("GPS "));
  if (bits & UBX_MON_GNSS_GLONASS)
    Serial.print(F("GLONASS "));
  if (bits & UBX_MON_GNSS_BEIDOU)
    Serial.print(F("BeiDou "));
  if (bits & UBX_MON_GNSS_GALILEO)
    Serial.print(F("Galileo "));
  if (bits == 0)
    Serial.print(F("(none)"));
}
