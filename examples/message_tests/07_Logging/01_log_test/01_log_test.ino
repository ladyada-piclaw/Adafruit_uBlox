/*!
 * @file 01_log_test.ino
 *
 * Message test: Create, info, and erase log using LOG messages.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 */

#include <Adafruit_UBX.h>
#include <Adafruit_UBloxDDC.h>

Adafruit_UBloxDDC ddc;
Adafruit_UBX ubx(ddc);

bool tests_run = false;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== UBX-LOG Message Test ==="));

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
  Serial.println(F("Waiting a few seconds for module to stabilize..."));
  delay(3000);

  runTests();
  tests_run = true;
}

void loop() {
  UBX_LOG_INFO_t info;

  if (!ubx.pollLogInfo(&info)) {
    Serial.println(F("LOG-INFO poll failed (timeout)"));
    delay(10000);
    return;
  }

  Serial.println(F("--- LOG-INFO ---"));
  Serial.print(F("Capacity: "));
  Serial.print(info.filestoreCapacity);
  Serial.println(F(" bytes"));
  Serial.print(F("Current size: "));
  Serial.print(info.currentLogSize);
  Serial.println(F(" bytes"));
  Serial.print(F("Entries: "));
  Serial.println(info.entryCount);
  Serial.print(F("Status: 0x"));
  Serial.print(info.status, HEX);
  Serial.print(F(" (recording="));
  Serial.print((info.status & UBX_LOG_INFO_STATUS_RECORDING) ? F("Y") : F("N"));
  Serial.print(F(", inactive="));
  Serial.print((info.status & UBX_LOG_INFO_STATUS_INACTIVE) ? F("Y") : F("N"));
  Serial.println(F(")"));

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

void printTestResult(const __FlashStringHelper *name, bool pass) {
  Serial.print(F("  ["));
  Serial.print(pass ? F("PASS") : F("FAIL"));
  Serial.print(F("] "));
  Serial.print(name);
  Serial.print(F(": "));
}

void runTests() {
  uint8_t passed = 0;
  const uint8_t total = 6;
  UBX_LOG_INFO_t info;

  Serial.println();
  Serial.println(F("Running LOG tests..."));

  Serial.println(F("Erasing any existing log..."));
  ubx.eraseLog();
  delay(500);

  bool create_ok = ubx.createLog(UBX_LOG_SIZE_MINIMUM, true, 0);
  printTestResult(F("create_log"), create_ok);
  Serial.println(create_ok ? F("OK") : F("FAIL"));
  if (create_ok)
    passed++;

  delay(500);

  bool poll_ok = ubx.pollLogInfo(&info);
  printTestResult(F("poll_info"), poll_ok);
  Serial.println(poll_ok ? F("OK") : F("FAIL"));
  if (poll_ok)
    passed++;

  bool active_ok = poll_ok && !(info.status & UBX_LOG_INFO_STATUS_INACTIVE);
  printTestResult(F("log_active"), active_ok);
  Serial.println(active_ok ? F("OK") : F("FAIL"));
  if (active_ok)
    passed++;

  bool circular_ok = poll_ok && (info.status & UBX_LOG_INFO_STATUS_CIRCULAR);
  printTestResult(F("log_circular"), circular_ok);
  Serial.println(circular_ok ? F("OK") : F("FAIL"));
  if (circular_ok)
    passed++;

  bool capacity_ok = poll_ok && info.filestoreCapacity > 0;
  printTestResult(F("capacity_nonzero"), capacity_ok);
  Serial.print(info.filestoreCapacity);
  Serial.println(F(" bytes"));
  if (capacity_ok)
    passed++;

  bool erase_ok = ubx.eraseLog();
  printTestResult(F("erase_log"), erase_ok);
  Serial.println(erase_ok ? F("OK") : F("FAIL"));
  if (erase_ok)
    passed++;

  Serial.println();
  Serial.print(F("Results: "));
  Serial.print(passed);
  Serial.print(F("/"));
  Serial.print(total);
  Serial.println(F(" tests passed"));
}
