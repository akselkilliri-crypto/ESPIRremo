#include <IRremote.h>

#define RECV_PIN  15   // GPIO для приёмника
#define SEND_PIN  4    // GPIO для передатчика

IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);

// Флаги и данные
bool hasCapturedData = false;
uint32_t lastAddress = 0;
uint16_t lastCommand = 0;
decode_type_t lastProtocol = UNKNOWN;
uint16_t lastFlags = 0;

void setup() {
  Serial.begin(115200);
  
  // Инициализация приёма
  irrecv.enableIRIn();
  
  Serial.println("=== Covert IR Remote ===");
  Serial.println("Board: NodeMCU-32S (ESP32)");
  Serial.println("Ready. Press any IR button, then type GO.");
  Serial.println();
}

void loop() {
  // Проверяем, пришли ли данные
  if (irrecv.decode()) {
    // Сохраняем данные
    lastProtocol = irrecv.decodedIRData.protocol;
    lastAddress = irrecv.decodedIRData.address;
    lastCommand = irrecv.decodedIRData.command;
    lastFlags = irrecv.decodedIRData.flags;
    hasCapturedData = true;
    
    // Выводим информацию
    Serial.print("✅ Captured! Protocol: ");
    Serial.print(lastProtocol);
    Serial.print(" Address: 0x");
    Serial.print(lastAddress, HEX);
    Serial.print(" Command: 0x");
    Serial.println(lastCommand, HEX);
    
    // Повторный запуск приёма
    irrecv.resume();
  }

  // Обработка команд из Serial
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    s.trim();
    
    if (s.equalsIgnoreCase("GO")) {
      if (hasCapturedData && lastProtocol != UNKNOWN && lastProtocol < 27) {
        Serial.println("📡 Sending...");
        
        // Отправляем данные
        irsend.write(lastProtocol, lastAddress, lastCommand, lastFlags);
        
        Serial.println("✅ Sent!");
      } else {
        Serial.println("❌ No valid code captured.");
      }
    } else if (s.equalsIgnoreCase("TEST")) {
      // Тестовая команда для проверки связи
      Serial.println("🔊 Serial connection OK!");
      Serial.print("Has data: ");
      Serial.println(hasCapturedData ? "YES" : "NO");
    }
  }
  
  // Небольшая задержка для стабильности
  delay(10);
}
