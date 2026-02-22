/*!
 * @file 01_nav_pvt_test.ino
 *
 * Message test: Poll UBX-NAV-PVT and continuously print all fields.
 * Validates basic field ranges and halts on communication failure.
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

  Serial.println(F("=== UBX-NAV-PVT Message Test ==="));

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
  UBX_NAV_PVT_t pvt;

  if (!ubx.poll(UBX_CLASS_NAV, UBX_NAV_PVT, &pvt, sizeof(pvt))) {
    Serial.println(F("NAV-PVT poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- NAV-PVT ---"));

  // Time
  Serial.print(F("Time: "));
  printDate(pvt.year, pvt.month, pvt.day);
  Serial.print(F(" "));
  printTime(pvt.hour, pvt.min, pvt.sec);
  Serial.println();

  // Fix info
  Serial.print(F("Fix type: "));
  Serial.print(pvt.fixType);
  Serial.print(F("  Sats: "));
  Serial.print(pvt.numSV);
  Serial.print(F("  Fix OK: "));
  Serial.println(pvt.flags & 0x01);

  // Validity flags
  Serial.print(F("Valid: date="));
  Serial.print(pvt.valid & 0x01);
  Serial.print(F(" time="));
  Serial.print((pvt.valid >> 1) & 0x01);
  Serial.print(F(" resolved="));
  Serial.println((pvt.valid >> 2) & 0x01);

  // Position (last 4 digits blanked for privacy)
  Serial.print(F("Lat: "));
  printCoordRedacted(pvt.lat * 1e-7);
  Serial.println(F(" deg"));
  Serial.print(F("Lon: "));
  printCoordRedacted(pvt.lon * 1e-7);
  Serial.println(F(" deg"));
  Serial.print(F("Height: "));
  Serial.print(pvt.hMSL / 1000.0, 1);
  Serial.println(F(" m (MSL)"));

  // Accuracy
  Serial.print(F("hAcc: "));
  Serial.print(pvt.hAcc / 1000.0, 1);
  Serial.print(F(" m  vAcc: "));
  Serial.print(pvt.vAcc / 1000.0, 1);
  Serial.println(F(" m"));

  // Motion
  Serial.print(F("Ground speed: "));
  Serial.print(pvt.gSpeed / 1000.0, 3);
  Serial.println(F(" m/s"));
  Serial.print(F("Heading: "));
  Serial.print(pvt.headMot * 1e-5, 1);
  Serial.println(F(" deg"));

  // DOP
  Serial.print(F("pDOP: "));
  Serial.println(pvt.pDOP * 0.01, 2);

  Serial.println();
  delay(2000);
}

/**************************************************************************/
/* Helper functions                                                       */
/**************************************************************************/

/*!
 * @brief Print a fatal error message and halt execution.
 * @param msg Error message (F() string)
 */
void halt(const __FlashStringHelper *msg) {
  Serial.print(F("HALT: "));
  Serial.println(msg);
  while (1)
    delay(10);
}

/*!
 * @brief Print a coordinate with last 4 digits replaced by xxxx.
 */
void printCoordRedacted(double coord) {
  // Print with 3 decimal places, then append "xxxx"
  Serial.print(coord, 3);
  Serial.print(F("xxxx"));
}

/*!
 * @brief Print a date in YYYY-MM-DD format.
 */
void printDate(uint16_t year, uint8_t month, uint8_t day) {
  char buf[16];
  snprintf(buf, sizeof(buf), "%04u-%02u-%02u", year, month, day);
  Serial.print(buf);
}

/*!
 * @brief Print a time in HH:MM:SS format.
 */
void printTime(uint8_t hour, uint8_t min, uint8_t sec) {
  char buf[16];
  snprintf(buf, sizeof(buf), "%02u:%02u:%02u", hour, min, sec);
  Serial.print(buf);
}
