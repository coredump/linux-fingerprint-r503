// r503fp_listen.ino — pure passive listener on SoftSerial D5 (GPIO14).
//
// Reports every byte received from the sensor. Used to capture the
// R503's 0x55 boot handshake when power-cycling the sensor (NOT the D1 Mini).
//
// Procedure:
//   1. Upload this firmware. SoftSerial @ 57600 starts immediately.
//   2. Open the serial monitor at 115200.
//   3. Disconnect the sensor's RED wire from the 3.3V rail (kills sensor power).
//      Leave the D1 Mini powered and the firmware running.
//   4. Wait ~2 seconds.
//   5. Reconnect RED. Sensor boots, sends 0x55 ~50ms after power-up.
//   6. We should see "RX: 0x55" in the serial monitor.

#include <SoftwareSerial.h>

#define LED_ON  LOW
#define LED_OFF HIGH

const long PC_BAUD = 115200;
const long FP_BAUD = 57600;
SoftwareSerial soft(14, 12); // RX=GPIO14=D5, TX=GPIO12=D6

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(PC_BAUD);
  soft.begin(FP_BAUD);
  while (!Serial) { ; }
  delay(100);
  Serial.println(F("listening on D5 @ 57600. power-cycle the sensor (disconnect+reconnect RED)."));
}

void loop() {
  if (soft.available()) {
    int b = soft.read();
    digitalWrite(LED_BUILTIN, LED_ON);
    Serial.print(F("RX: 0x"));
    if (b < 0x10) Serial.print('0');
    Serial.print(b, HEX);
    Serial.print(F(" ("));
    Serial.print(b);
    Serial.println(F(")"));
    digitalWrite(LED_BUILTIN, LED_OFF);
  }
}
