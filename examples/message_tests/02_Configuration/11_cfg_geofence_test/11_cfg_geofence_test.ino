/*!
 * @file 11_cfg_geofence_test.ino
 *
 * Message test: Configure geofence and check status.
 * Uses a test position; does not require a fix.
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

  Serial.println(F("=== UBX-CFG-GEOFENCE Message Test ==="));

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

  runTests();
  tests_run = true;
}

void loop() {
  UBX_NAV_GEOFENCE_header_t header;
  UBX_NAV_GEOFENCE_fence_t fences[4];

  uint8_t numFences = ubx.pollNavGeofence(&header, fences, 4);
  if (numFences == 0 && header.status == 0) {
    Serial.println(F("NAV-GEOFENCE: no active geofence"));
    delay(5000);
    return;
  }

  Serial.println(F("--- NAV-GEOFENCE ---"));
  Serial.print(F("iTOW: "));
  Serial.print(header.iTOW);
  Serial.println(F(" ms"));

  Serial.print(F("status: "));
  Serial.println(header.status == 1 ? F("active") : F("not available"));

  Serial.print(F("numFences: "));
  Serial.println(header.numFences);

  Serial.print(F("combState: "));
  Serial.print(header.combState);
  Serial.print(F(" ("));
  Serial.print(getStateStr(header.combState));
  Serial.println(F(")"));

  for (uint8_t i = 0; i < numFences; i++) {
    Serial.print(F("  Fence "));
    Serial.print(i);
    Serial.print(F(": state="));
    Serial.print(fences[i].state);
    Serial.print(F(" ("));
    Serial.print(getStateStr(fences[i].state));
    Serial.print(F("), id="));
    Serial.println(fences[i].id);
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

void printTestResult(const __FlashStringHelper *name, bool pass) {
  Serial.print(F("  ["));
  Serial.print(pass ? F("PASS") : F("FAIL"));
  Serial.print(F("] "));
  Serial.print(name);
  Serial.print(F(": "));
}

const char *getStateStr(uint8_t state) {
  switch (state) {
    case 0:
      return "Unknown";
    case 1:
      return "Inside";
    case 2:
      return "Outside";
    default:
      return "Invalid";
  }
}

void runTests() {
  uint8_t passed = 0;
  const uint8_t total = 4;

  Serial.println();
  Serial.println(F("Running CFG-GEOFENCE tests..."));

  // Use a test position (New York City area)
  int32_t test_lat = 407128000;  // 40.7128 deg * 1e7
  int32_t test_lon = -740060000; // -74.0060 deg * 1e7

  bool clear_ok = ubx.clearGeofence();
  printTestResult(F("clear_geofence"), clear_ok);
  Serial.println(clear_ok ? F("OK") : F("FAIL"));
  if (clear_ok)
    passed++;

  delay(500);

  // Set a geofence (100m = 10000cm radius)
  bool set_ok = ubx.setGeofence(test_lat, test_lon, 10000, 2);
  printTestResult(F("set_geofence"), set_ok);
  Serial.println(set_ok ? F("OK") : F("FAIL"));
  if (set_ok)
    passed++;

  delay(500);

  UBX_CFG_GEOFENCE_header_t cfgHeader;
  UBX_CFG_GEOFENCE_fence_t cfgFences[4];
  uint8_t numFences = ubx.pollCfgGeofence(&cfgHeader, cfgFences, 4);
  bool cfg_ok = (numFences == 1);
  printTestResult(F("config_poll"), cfg_ok);
  Serial.print(numFences);
  Serial.println(F(" fence(s)"));
  if (cfg_ok)
    passed++;

  bool params_ok = false;
  if (numFences > 0) {
    params_ok = (cfgFences[0].lat == test_lat) && (cfgFences[0].lon == test_lon) &&
                (cfgFences[0].radius == 10000);
    printTestResult(F("fence_params"), params_ok);
    Serial.print(F("lat="));
    Serial.print(cfgFences[0].lat);
    Serial.print(F(", lon="));
    Serial.print(cfgFences[0].lon);
    Serial.print(F(", radius="));
    Serial.print(cfgFences[0].radius);
    Serial.println(F(" cm"));
  } else {
    printTestResult(F("fence_params"), false);
    Serial.println(F("no fences"));
  }
  if (params_ok)
    passed++;

  // Clean up
  ubx.clearGeofence();

  Serial.println();
  Serial.print(F("Results: "));
  Serial.print(passed);
  Serial.print(F("/"));
  Serial.print(total);
  Serial.println(F(" tests passed"));
}
