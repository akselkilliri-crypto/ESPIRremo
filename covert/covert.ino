#include <IRremote.h>

// 🔧 БЕЗОПАСНЫЕ ПИНЫ ДЛЯ ESP32
// Попробуем GPIO 4 (как в оригинале), так как 25/26 могут быть не разведены на вашей плате
#define RECV_PIN  4    
#define SEND_PIN  2    

IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);

bool hasCapturedData = false;
uint32_t lastAddress = 0;
uint16_t lastCommand = 0;
decode_type_t lastProtocol = UNKNOWN;
uint16_t lastFlags = 0;
uint16_t lastRawLen = 0;
uint16_t* lastRawBuf = nullptr;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println();
  Serial.println("=== RGB IR Remote Debugger ===");
  Serial.print("Receiver on GPIO ");
  Serial.println(RECV_PIN);
  
  irrecv.enableIRIn();
  Serial.println("✅ IR Ready. Press buttons...");
  Serial.println("Commands: TEST, SCAN, RAW");
  Serial.println();
}

void loop() {
  if (irrecv.decode()) {
    hasCapturedData = true;
    lastProtocol = irrecv.decodedIRData.protocol;
    lastAddress = irrecv.decodedIRData.address;
    lastCommand = irrecv.decodedIRData.command;
    lastFlags = irrecv.decodedIRData.flags;
    
    // Сохраняем RAW данные (если есть)
    if (irrecv.decodedIRData.rawDataPtr) {
      lastRawLen = irrecv.decodedIRData.rawDataPtr->rawLen;
      // Копируем буфер (упрощенно, для отладки хватит длины)
      // В реальном проекте нужно malloc/free
    }
    
    Serial.println("📡 SIGNAL DETECTED!");
    Serial.print("Protocol: ");
    Serial.print(lastProtocol);
    if (lastProtocol == UNKNOWN) Serial.print(" (UNKNOWN - Normal for RGB remotes)");
    Serial.println();
    
    Serial.print("Address: 0x");
    Serial.println(lastAddress, HEX);
    Serial.print("Command: 0x");
    Serial.println(lastCommand, HEX);
    
    // Для RGB пультов часто важно именно RAW значение команды
    Serial.print("Raw Command (DEC): ");
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
    }
    else if (s.equalsIgnoreCase("GO")) {
      if (hasCapturedData) {
        Serial.println("📡 Re-sending...");
        // Для UNKNOWN протокола лучше отправлять как NEC или RAW
        if (lastProtocol == UNKNOWN) {
          // Пытаемся отправить как NEC (часто работает для RGB)
          irsend.write(NEC, lastAddress, lastCommand);
        } else {
          irsend.write(lastProtocol, lastAddress, lastCommand);
        }
        Serial.println("✅ Sent!");
      } else {
        Serial.println("❌ No data");
      }
    }
  }
  delay(10);
}
