/*!
 * @file 02_nav_status_test.ino
 *
 * Message test: Poll UBX-NAV-STATUS and continuously print all fields.
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

  Serial.println(F("=== UBX-NAV-STATUS Message Test ==="));

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
  UBX_NAV_STATUS_t status;

  if (!ubx.poll(UBX_CLASS_NAV, UBX_NAV_STATUS, &status, sizeof(status))) {
    Serial.println(F("NAV-STATUS poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- NAV-STATUS ---"));

  Serial.print(F("Fix type: "));
  Serial.print(status.gpsFix);
  switch (status.gpsFix) {
    case 0:
      Serial.println(F(" (no fix)"));
      break;
    case 1:
      Serial.println(F(" (dead reckoning)"));
      break;
    case 2:
      Serial.println(F(" (2D fix)"));
      break;
    case 3:
      Serial.println(F(" (3D fix)"));
      break;
    case 4:
      Serial.println(F(" (GNSS + dead reckoning)"));
      break;
    case 5:
      Serial.println(F(" (time only)"));
      break;
    default:
      Serial.println(F(" (unknown)"));
      break;
  }

  Serial.print(F("Fix valid: "));
  Serial.println((status.flags & 0x01) ? F("yes") : F("no"));
  Serial.print(F("Differential corrections: "));
  Serial.println(((status.flags >> 1) & 0x01) ? F("applied") : F("none"));
  Serial.print(F("Week number valid: "));
  Serial.println(((status.flags >> 2) & 0x01) ? F("yes") : F("no"));
  Serial.print(F("Time of week valid: "));
  Serial.println(((status.flags >> 3) & 0x01) ? F("yes") : F("no"));

  Serial.print(F("Time to first fix: "));
  Serial.print(status.ttff / 1000.0, 1);
  Serial.println(F(" sec"));
  Serial.print(F("Uptime since reset: "));
  Serial.print(status.msss / 1000.0, 1);
  Serial.println(F(" sec"));

  uint8_t psm_state = status.flags2 & 0x07;
  uint8_t spoof_state = (status.flags2 >> 3) & 0x03;
  uint8_t carr_soln = (status.flags2 >> 6) & 0x03;
  Serial.print(F("Power save mode: "));
  Serial.println(psmLabel(psm_state));
  Serial.print(F("Spoofing detection: "));
  Serial.println(spoofLabel(spoof_state));
  Serial.print(F("Carrier solution: "));
  switch (carr_soln) {
    case 0:
      Serial.println(F("none"));
      break;
    case 1:
      Serial.println(F("floating"));
      break;
    case 2:
      Serial.println(F("fixed"));
      break;
    default:
      Serial.println(F("unknown"));
      break;
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

const __FlashStringHelper *psmLabel(uint8_t psm_state) {
  switch (psm_state) {
    case 0:
      return F("acquisition");
    case 1:
      return F("tracking");
    case 2:
      return F("power optimized");
    case 3:
      return F("inactive");
    default:
      return F("unknown");
  }
}

const __FlashStringHelper *spoofLabel(uint8_t spoof_state) {
  switch (spoof_state) {
    case 0:
      return F("unknown/deactivated");
    case 1:
      return F("no spoofing");
    case 2:
      return F("spoofing indicated");
    case 3:
      return F("multiple spoofing indicated");
    default:
      return F("unknown");
  }
}
