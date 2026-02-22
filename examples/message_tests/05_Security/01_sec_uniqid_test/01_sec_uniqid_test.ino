/*!
 * @file 01_sec_uniqid_test.ino
 *
 * Message test: Poll UBX-SEC-UNIQID (Unique Chip ID)
 * Displays the 40-bit unique chip identifier.
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

  Serial.println(F("=== UBX-SEC-UNIQID Message Test ==="));

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
  UBX_SEC_UNIQID_t uniqid;

  if (!ubx.pollSecUniqid(&uniqid)) {
    Serial.println(F("SEC-UNIQID poll failed (timeout)"));
    delay(10000);
    return;
  }

  Serial.println(F("--- SEC-UNIQID ---"));
  Serial.print(F("Version: "));
  Serial.println(uniqid.version);

  Serial.print(F("Unique ID: "));
  for (uint8_t i = 0; i < 5; i++) {
    printHex(uniqid.uniqueId[i]);
    if (i < 4)
      Serial.print(F(":"));
  }
  Serial.println();

  Serial.println();
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

void printHex(uint8_t val) {
  if (val < 0x10)
    Serial.print(F("0"));
  Serial.print(val, HEX);
}
