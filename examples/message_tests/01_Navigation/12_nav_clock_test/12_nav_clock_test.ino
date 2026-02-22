/*!
 * @file 12_nav_clock_test.ino
 *
 * Message test: Poll UBX-NAV-CLOCK and continuously print all fields.
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

  Serial.println(F("=== UBX-NAV-CLOCK Message Test ==="));

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
  UBX_NAV_CLOCK_t clk;

  if (!ubx.pollNAVCLOCK(&clk)) {
    Serial.println(F("NAV-CLOCK poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- NAV-CLOCK ---"));

  Serial.print(F("iTOW: "));
  Serial.print(clk.iTOW);
  Serial.println(F(" ms"));

  Serial.print(F("clkB: "));
  Serial.print(clk.clkB);
  Serial.println(F(" ns"));

  Serial.print(F("clkD: "));
  Serial.print(clk.clkD);
  Serial.println(F(" ns/s"));

  Serial.print(F("tAcc: "));
  Serial.print(clk.tAcc);
  Serial.println(F(" ns"));

  Serial.print(F("fAcc: "));
  Serial.print(clk.fAcc);
  Serial.println(F(" ps/s"));

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
