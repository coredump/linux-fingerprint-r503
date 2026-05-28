// r503fp_txspam.ino — continuously transmit 0x55 on D6 at 57600 baud.
//
// Measure D6 (GPIO12) voltage with a multimeter:
//   ~1.65V → TX is alive, ~50% duty cycle on the line (3.3V × 50%)
//   ~3.3V  → TX never drives (firmware bug or D6 dead)
//   ~0.0V  → D6 stuck low

#include <SoftwareSerial.h>

const long PC_BAUD = 115200;
const long FP_BAUD = 57600;
const uint8_t PIN_RX = 14; // GPIO14 = D5
const uint8_t PIN_TX = 12; // GPIO12 = D6

SoftwareSerial soft(PIN_RX, PIN_TX);

void setup() {
  Serial.begin(PC_BAUD);
  soft.begin(FP_BAUD);
  while (!Serial) { ; }
  delay(200);
  Serial.println(F("TX SPAM @ 57600 on D6 — measure D6 voltage"));
  Serial.println(F("Expected ~1.65V if TX is working (3.3V × 50% duty)"));
}

void loop() {
  soft.write(0x55);  // 01010101 — alternating bits, 50% duty
  yield();           // feed ESP8266 WDT; soft.write() is blocking at 57600
}
