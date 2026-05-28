// r503fp_wipe.ino — emergency one-shot EEPROM wipe.
//
// USE WHEN: you've lost the host-side key file (/var/lib/r503d/key and its .bak)
// and need to re-pair the D1 Mini. The authenticated unpair (r503d --unpair)
// needs the key to authorize, so without it your only options are:
//   1. Flash this sketch via arduino-cli over the existing USB cable
//      (no box opening, no soldering, no hardware access beyond USB).
//   2. Flash the real firmware (r503fp/) back. The D1 Mini boots unpaired.
//   3. r503d --pair to establish a fresh pairing.
//
// What it does:
//   - On boot, clears 1024 bytes of emulated EEPROM to 0xFF (factory state).
//     The real firmware only uses bytes 0-191 (SPEC §13.4 / eeprom.h), but
//     wiping 1024 bytes covers any future schema changes and any leftover
//     experimental data. EEPROM.commit() makes the wipe atomic from a flash
//     perspective — the sector is only written once at the end, not per byte.
//   - Prints progress lines so an attached r503ctl.py or terminal sees the
//     operation complete.
//   - Blinks LED_BUILTIN forever (200 ms on / 200 ms off) as a visual
//     "I am NOT the real firmware, please reflash" reminder. The D1 Mini will
//     respond to NOTHING over serial after the initial messages.
//
// SECURITY note: this sketch requires PHYSICAL USB ACCESS to the D1 Mini. An
// attacker with that access could reflash to anything anyway (the ESP8266
// bootloader has no signing), and would still need root on the host to
// re-pair (/etc/r503d/allow-pair is root-owned). So this isn't an additional
// attack vector — it's the only laptop-only recovery path.

#include <EEPROM.h>

// D1 Mini has an active-LOW LED_BUILTIN (GPIO2).
#define LED_ON  LOW
#define LED_OFF HIGH

const uint16_t EEPROM_BYTES = 1024; // allocation size passed to EEPROM.begin()
const long PC_BAUD = 115200;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_ON); // solid on during wipe

  Serial.begin(PC_BAUD);
  while (!Serial) { ; }
  delay(500);

  EEPROM.begin(EEPROM_BYTES);

  Serial.println(F("r503fp_wipe v1 — clearing EEPROM"));
  for (uint16_t a = 0; a < EEPROM_BYTES; ++a) {
    // write() fills the RAM buffer; commit() below flushes to flash in one
    // atomic sector write. On ESP8266 emulated EEPROM the wipe is therefore
    // all-or-nothing — power loss before commit() leaves flash unchanged.
    EEPROM.write(a, 0xFF);
  }
  EEPROM.commit();
  Serial.print(F("WIPED bytes="));
  Serial.println(EEPROM_BYTES);
  Serial.println(F("Now flash firmware/r503fp/ and run `r503d --pair` for a fresh pairing."));
}

void loop() {
  // Visible "this isn't the real firmware" indicator.
  digitalWrite(LED_BUILTIN, LED_ON);
  delay(200);
  digitalWrite(LED_BUILTIN, LED_OFF);
  delay(200);
}
