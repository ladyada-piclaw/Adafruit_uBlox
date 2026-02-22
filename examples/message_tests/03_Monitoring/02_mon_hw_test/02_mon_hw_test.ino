/*!
 * @file 02_mon_hw_test.ino
 *
 * Message test: Poll UBX-MON-HW (Hardware Status)
 * Displays antenna status, noise level, AGC, and jamming indicator.
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

  Serial.println(F("=== UBX-MON-HW Message Test ==="));

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
  UBX_MON_HW_t hw;

  if (!ubx.pollMonHw(&hw)) {
    Serial.println(F("MON-HW poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- MON-HW ---"));

  Serial.println(F("Antenna:"));
  Serial.print(F("  Status: "));
  Serial.print(hw.aStatus);
  Serial.print(F(" ("));
  Serial.print(getAntennaStatusName(hw.aStatus));
  Serial.println(F(")"));
  Serial.print(F("  Power: "));
  Serial.print(hw.aPower);
  Serial.print(F(" ("));
  Serial.print(getAntennaPowerName(hw.aPower));
  Serial.println(F(")"));

  Serial.println(F("Signal quality:"));
  Serial.print(F("  Noise per MS: "));
  Serial.println(hw.noisePerMS);
  Serial.print(F("  AGC count: "));
  Serial.print(hw.agcCnt);
  Serial.print(F(" ("));
  Serial.print((hw.agcCnt * 100) / 8191);
  Serial.println(F("%)"));
  Serial.print(F("  Jamming indicator: "));
  Serial.print(hw.jamInd);
  Serial.print(F(" ("));
  if (hw.jamInd == 0) {
    Serial.print(F("none"));
  } else if (hw.jamInd < 80) {
    Serial.print(F("low"));
  } else if (hw.jamInd < 160) {
    Serial.print(F("medium"));
  } else {
    Serial.print(F("HIGH"));
  }
  Serial.println(F(")"));

  Serial.println(F("Flags:"));
  Serial.print(F("  RTC calibrated: "));
  Serial.println((hw.flags & UBX_MON_HW_FLAG_RTCCALIB) ? F("yes") : F("no"));
  Serial.print(F("  Safe boot: "));
  Serial.println((hw.flags & UBX_MON_HW_FLAG_SAFEBOOT) ? F("active") : F("inactive"));

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

const char *getAntennaStatusName(uint8_t status) {
  switch (status) {
    case UBX_MON_HW_ASTATUS_INIT:
      return "INIT";
    case UBX_MON_HW_ASTATUS_DONTKNOW:
      return "UNKNOWN";
    case UBX_MON_HW_ASTATUS_OK:
      return "OK";
    case UBX_MON_HW_ASTATUS_SHORT:
      return "SHORT";
    case UBX_MON_HW_ASTATUS_OPEN:
      return "OPEN";
    default:
      return "???";
  }
}

const char *getAntennaPowerName(uint8_t power) {
  switch (power) {
    case UBX_MON_HW_APOWER_OFF:
      return "OFF";
    case UBX_MON_HW_APOWER_ON:
      return "ON";
    case UBX_MON_HW_APOWER_DONTKNOW:
      return "UNKNOWN";
    default:
      return "???";
  }
}
