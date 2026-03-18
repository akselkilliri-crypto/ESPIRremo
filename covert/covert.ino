#include <IRremote.h>

// 🔧 ПИНЫ ДЛЯ ESP32 NodeMCU-32S
#define RECV_PIN  4    // GPIO 4 для приёмника
#define SEND_PIN  2    // GPIO 2 для передатчика

IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);

bool hasCapturedData = false;
uint32_t lastAddress = 0;
uint16_t lastCommand = 0;
decode_type_t lastProtocol = UNKNOWN;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println();
  Serial.println("=== RGB IR Remote Debugger ===");
  Serial.print("Receiver on GPIO ");
  Serial.println(RECV_PIN);
  
  irrecv.enableIRIn();
  Serial.println("✅ IR Ready. Press buttons...");
  Serial.println("Commands: TEST, GO");
  Serial.println();
}

void loop() {
  if (irrecv.decode()) {
    hasCapturedData = true;
    lastProtocol = irrecv.decodedIRData.protocol;
    lastAddress = irrecv.decodedIRData.address;
    lastCommand = irrecv.decodedIRData.command;
    
    Serial.println("📡 SIGNAL DETECTED!");
    Serial.print("Protocol: ");
    Serial.print(lastProtocol);
    if (lastProtocol == UNKNOWN) {
      Serial.print(" (UNKNOWN - Normal for RGB remotes)");
    }
    Serial.println();
    
    Serial.print("Address: 0x");
    Serial.println(lastAddress, HEX);
    Serial.print("Command: 0x");
    Serial.println(lastCommand, HEX);
    Serial.print("Command (DEC): ");
    Serial.println(lastCommand, DEC);
    Serial.println("-------------------");
    
    irrecv.resume();
  }

  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    s.trim();
    
    if (s.equalsIgnoreCase("TEST")) {
      Serial.println("🔊 Connection OK");
      Serial.print("Data captured: ");
      Serial.println(hasCapturedData ? "YES" : "NO");
      Serial.print("Last Protocol: ");
      Serial.println(lastProtocol);
    }
    else if (s.equalsIgnoreCase("GO")) {
      if (hasCapturedData) {
        Serial.println("📡 Re-sending...");
        if (lastProtocol == UNKNOWN || lastProtocol == 0) {
          // Для UNKNOWN пробуем отправить как NEC (работает для RGB)
          irsend.write(NEC, lastAddress, lastCommand);
          Serial.println("✅ Sent as NEC!");
        } else {
          irsend.write(lastProtocol, lastAddress, lastCommand);
          Serial.println("✅ Sent!");
        }
      } else {
        Serial.println("❌ No data captured yet");
      }
    }
  }
  delay(10);
}
