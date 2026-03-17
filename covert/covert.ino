#include <IRremote.h>

#define RECV_PIN  15   // GPIO для приёмника
#define SEND_PIN  4    // GPIO для передатчика

IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);

// Флаги и данные
bool hasCapturedData = false;
uint32_t lastAddress = 0;
uint16_t lastCommand = 0;
uint8_t lastProtocol = 0;
uint16_t lastFlags = 0;

void setup() {
  Serial.begin(115200);
  
  // Инициализация приёма
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  
  Serial.println("=== Covert IR Remote ===");
  Serial.println("Board: NodeMCU-32S (ESP32)");
  Serial.println("Ready. Press any IR button, then type GO.");
  Serial.println();
}

void loop() {
  // Проверяем, пришли ли данные
  if (IrReceiver.decode()) {
    // Сохраняем данные
    lastProtocol = IrReceiver.decodedIRData.protocol;
    lastAddress = IrReceiver.decodedIRData.address;
    lastCommand = IrReceiver.decodedIRData.command;
    lastFlags = IrReceiver.decodedIRData.flags;
    hasCapturedData = true;
    
    // Выводим информацию
    Serial.print("✅ Captured! Protocol: ");
    Serial.print(IrReceiver.protocolStr(lastProtocol));
    Serial.print(" (");
    Serial.print(lastProtocol);
    Serial.print(") Address: 0x");
    Serial.print(lastAddress, HEX);
    Serial.print(" Command: 0x");
    Serial.println(lastCommand, HEX);
    
    // Повторный запуск приёма
    IrReceiver.resume();
  }

  // Обработка команд из Serial
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    s.trim();
    
    if (s.equalsIgnoreCase("GO")) {
      if (hasCapturedData && lastProtocol > 0 && lastProtocol < 27) {
        Serial.println("📡 Sending...");
        
        // Создаём данные для отправки
        IRData sendData;
        sendData.protocol = lastProtocol;
        sendData.address = lastAddress;
        sendData.command = lastCommand;
        sendData.flags = lastFlags;
        
        // Отправляем
        IrSender.write(&sendData);
        
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
