/*!
 * @file 05_mon_io_hw.ino
 * @brief Hardware test for MON-IO (I/O System Status)
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

  Serial.println(F("=== MON-IO Hardware Test ==="));

  if (!ddc.begin()) {
    halt(F("GPS not found on I2C"));
  }
  Serial.println(F("GPS module connected"));

  if (!ubx.begin()) {
    halt(F("UBX parser init failed"));
  }

  delay(500);
  ubx.setUBXOnly(UBX_PORT_DDC, true, 1000);

  UBX_MON_IO_port_t ports[6];
  uint8_t n = ubx.pollMonIo(ports, 6);
  if (n > 0) {
    Serial.print(F("PASS: MON-IO poll OK, "));
    Serial.print(n);
    Serial.println(F(" ports"));
    for (uint8_t i = 0; i < n; i++) {
      if (ports[i].rxBytes > 0 || ports[i].txBytes > 0) {
        Serial.print(F("  "));
        Serial.print(portName(i));
        Serial.print(F(": rx="));
        Serial.print(ports[i].rxBytes);
        Serial.print(F(" tx="));
        Serial.println(ports[i].txBytes);
      }
    }
  } else {
    Serial.println(F("FAIL: MON-IO poll failed"));
  }

  Serial.println();
}

void loop() {
  UBX_MON_IO_port_t ports[6];

  uint8_t n = ubx.pollMonIo(ports, 6);
  if (n == 0) {
    Serial.println(F("MON-IO poll failed (timeout)"));
    delay(3000);
    return;
  }

  Serial.print(F("DDC rx="));
  Serial.print(ports[0].rxBytes);
  Serial.print(F(" tx="));
  Serial.println(ports[0].txBytes);

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

const char *portName(uint8_t p) {
  switch (p) {
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
    default:
      return "???";
  }
}
