/*!
 * @file 06_mon_msgpp_test.ino
 *
 * Message test: Poll UBX-MON-MSGPP (Message Parse/Process Status)
 * Displays message counts per protocol per port.
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

  Serial.println(F("=== UBX-MON-MSGPP Message Test ==="));

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
  UBX_MON_MSGPP_t msgpp;

  if (!ubx.pollMonMsgpp(&msgpp)) {
    Serial.println(F("MON-MSGPP poll failed (timeout)"));
    delay(5000);
    return;
  }

  Serial.println(F("--- MON-MSGPP ---"));
  Serial.println(F("Message counts (UBX/NMEA/RTCM):"));

  for (uint8_t p = 0; p < 6; p++) {
    bool hasData = false;
    for (uint8_t pr = 0; pr < 3; pr++) {
      if (msgpp.msg[p][pr] > 0)
        hasData = true;
    }
    if (hasData) {
      Serial.print(F("  "));
      Serial.print(getPortName(p));
      Serial.print(F(": UBX="));
      Serial.print(msgpp.msg[p][0]);
      Serial.print(F(", NMEA="));
      Serial.print(msgpp.msg[p][1]);
      Serial.print(F(", RTCM="));
      Serial.println(msgpp.msg[p][2]);
    }
  }

  Serial.print(F("Skipped bytes on DDC: "));
  Serial.println(msgpp.skipped[0]);

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

const char *getPortName(uint8_t port) {
  switch (port) {
    case 0:
      return "DDC";
    case 1:
      return "UART1";
    case 2:
      return "UART2";
    case 3:
      return "USB";
    case 4:
      return "SPI";
    case 5:
      return "Port5";
    default:
      return "???";
  }
}
