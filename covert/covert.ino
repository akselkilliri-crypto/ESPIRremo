#include <IRremote.h>

#define RECV_PIN  15   // GPIO для приёмника
#define SEND_PIN  4    // GPIO для передатчика

IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);

IRData capturedData;
bool hasCapturedData = false;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();
  Serial.println("=== Covert IR Remote ===");
  Serial.println("Board: NodeMCU-32S (ESP32)");
  Serial.println("Ready. Press any IR button, then type GO.");
}

void loop() {
  if (irrecv.decode()) {
    IRData* data = irrecv.read();  // Возвращает указатель
    if (data) {
      capturedData = *data;         // Разыменовываем указатель
      hasCapturedData = true;
      
      Serial.print("Captured: protocol=");
      Serial.print(capturedData.protocol);
      Serial.print(" address=0x");
      Serial.print(capturedData.address, HEX);
      Serial.print(" command=0x");
      Serial.println(capturedData.command, HEX);
    }
    irrecv.resume();
  }

  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    s.trim();
    if (s.equalsIgnoreCase("GO")) {
      if (hasCapturedData && capturedData.protocol > 0 && capturedData.protocol < 27) {
        Serial.println("Sending...");
        irsend.write(&capturedData);
      } else {
        Serial.println("No valid code captured.");
      }
    }
  }
}
