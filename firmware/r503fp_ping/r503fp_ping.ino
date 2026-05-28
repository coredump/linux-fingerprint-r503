// r503fp_ping.ino — Layer 1 partial: prove the D1 Mini can talk to the R503.
//
// Wiring (D1 Mini + R503, direct connection — no voltage divider needed at 3.3V):
//   R503 red    → D1 Mini 3V3       (Power Supply, sensor main)
//   R503 black  → D1 Mini GND
//   R503 yellow → D1 Mini D5        (SoftwareSerial RX; sensor TX, 3.3V TTL)
//   R503 brown  → D1 Mini D6        (SoftwareSerial TX; sensor RX, direct)
//   R503 blue   → D1 Mini D2        (WAKEUP; optional)
//   R503 white  → D1 Mini 3V3       (Touch power, shares rail with red)
//
// Commands (PC link @ 115200, line-terminated):
//   info  → query sensor: capacity, enrolled count, system id, security level, device addr
//   wake  → read WAKEUP pin state (0 or 1)
//   ping  → return "OK pong" without touching the sensor
// Anything else → "ERR unknown_command <token>"

#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

#define LED_ON  LOW
#define LED_OFF HIGH

const long PC_BAUD = 115200;
const long FP_BAUD = 57600;
const uint8_t PIN_RX   = 14; // GPIO14 = D5, sensor TX (yellow)
const uint8_t PIN_TX   = 12; // GPIO12 = D6, sensor RX (brown), direct
const uint8_t PIN_WAKE =  4; // GPIO4  = D2, sensor WAKEUP (blue)

const char BANNER[] = "R503FP READY fw=0.1-ping";

SoftwareSerial sensorSerial(PIN_RX, PIN_TX);
Adafruit_Fingerprint finger(&sensorSerial);

String inbuf;

void emitInfo() {
  if (!finger.verifyPassword()) {
    Serial.println("ERR sensor_unreachable");
    return;
  }
  finger.getParameters();
  finger.getTemplateCount();
  Serial.print(F("OK fw=0.1-ping capacity="));
  Serial.print(finger.capacity);
  Serial.print(F(" enrolled="));
  Serial.print(finger.templateCount);
  Serial.print(F(" sysid=0x"));
  Serial.print(finger.system_id, HEX);
  Serial.print(F(" security="));
  Serial.print(finger.security_level);
  Serial.print(F(" device_addr=0x"));
  Serial.println(finger.device_addr, HEX);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_WAKE, INPUT);
  Serial.begin(PC_BAUD);
  while (!Serial) { ; }
  delay(100);
  Serial.println(BANNER);

  sensorSerial.begin(FP_BAUD);
  delay(200);
  emitInfo();
}

void loop() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (inbuf.length() == 0) continue;
      digitalWrite(LED_BUILTIN, LED_ON);
      if (inbuf == "info") {
        emitInfo();
      } else if (inbuf == "wake") {
        Serial.print(F("OK wake="));
        Serial.println(digitalRead(PIN_WAKE) ? '1' : '0');
      } else if (inbuf == "ping") {
        Serial.println(F("OK pong"));
      } else {
        Serial.print(F("ERR unknown_command "));
        Serial.println(inbuf);
      }
      digitalWrite(LED_BUILTIN, LED_OFF);
      inbuf = "";
    } else {
      inbuf += c;
      if (inbuf.length() > 64) {
        Serial.println(F("ERR bad_args overflow"));
        inbuf = "";
      }
    }
  }
}
