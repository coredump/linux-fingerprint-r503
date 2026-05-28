// r503fp_loopback.ino — D1 Mini SoftwareSerial loopback test.
//
// Setup: jumper directly between D6 (GPIO12, TX) and D5 (GPIO14, RX) —
// no breadboard, no sensor. Confirms the D1 Mini's SoftSerial TX/RX path
// works end-to-end at the same baud we use for the sensor.
//
// Sends a known pattern. Reads back. Reports match/mismatch over PC link.

#include <SoftwareSerial.h>

#define LED_ON  LOW
#define LED_OFF HIGH

const long PC_BAUD = 115200;
const long FP_BAUD = 57600;
const uint8_t PIN_RX = 14; // GPIO14 = D5
const uint8_t PIN_TX = 12; // GPIO12 = D6

SoftwareSerial soft(PIN_RX, PIN_TX);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(PC_BAUD);
  soft.begin(FP_BAUD);
  while (!Serial) { ; }
  delay(200);
  Serial.println(F("LOOPBACK TEST @ 57600 — bridge D6 to D5 with a jumper"));

  // Known pattern: 0x55 0xAA 0x33 0xCC 0xEF 0x01 — sync word + a few interesting bytes
  static const byte pattern[] = {0x55, 0xAA, 0x33, 0xCC, 0xEF, 0x01, 0x12, 0x34, 0x56, 0x78};
  const uint8_t nPattern = sizeof(pattern);

  Serial.print(F("TX: "));
  for (uint8_t i = 0; i < nPattern; i++) {
    soft.write(pattern[i]);
    if (pattern[i] < 0x10) Serial.print('0');
    Serial.print(pattern[i], HEX);
    Serial.print(' ');
  }
  Serial.println();

  delay(150);  // give SoftSerial time to clock all bytes out + back in

  Serial.print(F("RX: "));
  byte received[32];
  uint8_t nReceived = 0;
  unsigned long deadline = millis() + 500;
  while (millis() < deadline && nReceived < sizeof(received)) {
    if (soft.available()) {
      byte b = soft.read();
      received[nReceived++] = b;
      digitalWrite(LED_BUILTIN, LED_ON);
      if (b < 0x10) Serial.print('0');
      Serial.print(b, HEX);
      Serial.print(' ');
      deadline = millis() + 100;  // extend if still receiving
    }
  }
  digitalWrite(LED_BUILTIN, LED_OFF);
  Serial.println();

  bool matches = (nReceived == nPattern);
  if (matches) {
    for (uint8_t i = 0; i < nPattern; i++) {
      if (received[i] != pattern[i]) { matches = false; break; }
    }
  }

  if (matches) {
    Serial.println(F("VERDICT: PASS — SoftSerial loopback works perfectly"));
  } else if (nReceived == 0) {
    Serial.println(F("VERDICT: FAIL — no bytes received. Jumper D6↔D5 missing, or SoftSerial broken"));
  } else {
    Serial.print(F("VERDICT: FAIL — received "));
    Serial.print(nReceived);
    Serial.print(F(" bytes, expected "));
    Serial.print(nPattern);
    Serial.println(F(". SoftSerial degraded — bit timing issue?"));
  }
}

void loop() { }
