/*!
 * @file 14_nav_hpposllh_test.ino
 *
 * Message test: Poll UBX-NAV-HPPOSLLH high precision position.
 * Note: SAM-M8Q may not support this message (HP GNSS only).
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

  Serial.println(F("=== UBX-NAV-HPPOSLLH Message Test ==="));
  Serial.println(F("Note: This message requires HP GNSS products (ZED-F9P etc)"));
  Serial.println(F("SAM-M8Q may NAK this message - that's expected!"));

  if (!ddc.begin()) {
    halt(F("Could not connect to GPS module on I2C"));
  }
  Serial.println(F("GPS module connected on I2C"));

  if (!ubx.begin()) {
    halt(F("UBX parser init failed"));
  }

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
  UBX_NAV_HPPOSLLH_t hppos;

  if (!ubx.pollNavHpposllh(&hppos)) {
    Serial.println(F("NAV-HPPOSLLH poll failed (not supported on non-HP GNSS)"));
    delay(5000);
    return;
  }

  Serial.println(F("--- NAV-HPPOSLLH ---"));

  Serial.print(F("iTOW: "));
  Serial.print(hppos.iTOW);
  Serial.println(F(" ms"));

  // Calculate full precision position
  double lat = hppos.lat * 1e-7 + hppos.latHp * 1e-9;
  double lon = hppos.lon * 1e-7 + hppos.lonHp * 1e-9;
  double height = hppos.height + hppos.heightHp * 0.1;
  double hMSL = hppos.hMSL + hppos.hMSLHp * 0.1;

  Serial.print(F("lat: "));
  Serial.print(lat, 9);
  Serial.println(F(" deg"));

  Serial.print(F("lon: "));
  Serial.print(lon, 9);
  Serial.println(F(" deg"));

  Serial.print(F("height: "));
  Serial.print(height, 1);
  Serial.println(F(" mm"));

  Serial.print(F("hMSL: "));
  Serial.print(hMSL, 1);
  Serial.println(F(" mm"));

  Serial.print(F("hAcc: "));
  Serial.print(hppos.hAcc * 0.1, 1);
  Serial.println(F(" mm"));

  Serial.print(F("vAcc: "));
  Serial.print(hppos.vAcc * 0.1, 1);
  Serial.println(F(" mm"));

  Serial.print(F("flags: 0x"));
  Serial.print(hppos.flags, HEX);
  Serial.print(F(" (invalid="));
  Serial.print((hppos.flags & UBX_NAV_HPPOSLLH_FLAG_INVALID) ? F("Y") : F("N"));
  Serial.println(F(")"));

  Serial.println();
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
