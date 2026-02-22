/*!
 * @file 05_mon_io_test.ino
 *
 * Message test: Poll UBX-MON-IO (I/O System Status)
 * Displays RX/TX bytes and error counts per port.
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

  Serial.println(F("=== UBX-MON-IO Message Test ==="));

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
  UBX_MON_IO_port_t ports[6];
  uint8_t numPorts = ubx.pollMonIo(ports, 6);

  if (numPorts == 0) {
    Serial.println(F("MON-IO poll failed (timeout)"));
    delay(3000);
    return;
  }

  Serial.println(F("--- MON-IO ---"));
  Serial.print(F("Number of ports: "));
  Serial.println(numPorts);

  for (uint8_t i = 0; i < numPorts; i++) {
    if (ports[i].rxBytes > 0 || ports[i].txBytes > 0) {
      Serial.print(F("Port "));
      Serial.print(i);
      Serial.print(F(" ("));
      Serial.print(getPortName(i));
      Serial.println(F("):"));
      Serial.print(F("  RX: "));
      Serial.print(ports[i].rxBytes);
      Serial.print(F(" bytes, TX: "));
      Serial.print(ports[i].txBytes);
      Serial.println(F(" bytes"));
      if (ports[i].parityErrs > 0 || ports[i].framingErrs > 0) {
        Serial.print(F("  Errors: parity="));
        Serial.print(ports[i].parityErrs);
        Serial.print(F(", framing="));
        Serial.println(ports[i].framingErrs);
      }
    }
  }

  Serial.println();
  delay(3000);
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
      return "DDC/I2C";
    case 1:
      return "UART1";
    case 2:
      return "UART2";
    case 3:
      return "USB";
    case 4:
      return "SPI";
    default:
      return "???";
  }
}
