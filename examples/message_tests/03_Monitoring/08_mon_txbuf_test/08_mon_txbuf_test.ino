/*!
 * @file 08_mon_txbuf_test.ino
 *
 * Message test: Poll UBX-MON-TXBUF (Transmitter Buffer Status)
 * Displays TX buffer pending, usage, peak, and error flags.
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

  Serial.println(F("=== UBX-MON-TXBUF Message Test ==="));

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
  UBX_MON_TXBUF_t txbuf;

  if (!ubx.pollMonTxbuf(&txbuf)) {
    Serial.println(F("MON-TXBUF poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- MON-TXBUF ---"));
  Serial.println(F("Port      Pending  Usage%  Peak%"));

  for (uint8_t i = 0; i < 6; i++) {
    Serial.print(getPortName(i));
    Serial.print(F("\t"));
    Serial.print(txbuf.pending[i]);
    Serial.print(F("\t "));
    Serial.print(txbuf.usage[i]);
    Serial.print(F("\t "));
    Serial.println(txbuf.peakUsage[i]);
  }

  Serial.print(F("Total: usage="));
  Serial.print(txbuf.tUsage);
  Serial.print(F("%, peak="));
  Serial.print(txbuf.tPeakUsage);
  Serial.println(F("%"));

  if (txbuf.errors != 0) {
    Serial.print(F("ERRORS: 0x"));
    Serial.println(txbuf.errors, HEX);
  }

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
