#include <IRremote.h>

// ⚠️ ПОПРОБУЙТЕ ЭТИ ПИНЫ ВМЕСТО 15/4
#define RECV_PIN  25   // GPIO 25 (вместо 15)
#define SEND_PIN  26   // GPIO 26 (вместо 4)

IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);

bool hasCapturedData = false;
uint32_t lastAddress = 0;
uint16_t lastCommand = 0;
decode_type_t lastProtocol = UNKNOWN;
uint16_t lastFlags = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Ждём подключения Serial (важно для отладки)
  
  Serial.println();
  Serial.println("=== Covert IR Remote v2 ===");
  Serial.print("Init IR Receiver on GPIO ");
  Serial.println(RECV_PIN);
  
  // Инициализация (функция возвращает void)
  irrecv.enableIRIn();
  Serial.println("✅ IR Receiver initialized");
  
  Serial.println("Ready. Press any IR button...");
  Serial.println("Type TEST to check connection");
  Serial.println();
}

void loop() {
  // Проверка данных
  if (irrecv.decode()) {
    lastProtocol = irrecv.decodedIRData.protocol;
    lastAddress = irrecv.decodedIRData.address;
    lastCommand = irrecv.decodedIRData.command;
    lastFlags = irrecv.decodedIRData.flags;
    hasCapturedData = true;
    
    Serial.print("✅ CAPTURED! Protocol: ");
    Serial.print(lastProtocol);
    Serial.print(" Addr: 0x");
    Serial.print(lastAddress, HEX);
    Serial.print(" Cmd: 0x");
    Serial.println(lastCommand, HEX);
    
    irrecv.resume();
  }

  // Команды
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    s.trim();
    
    if (s.equalsIgnoreCase("GO")) {
      if (hasCapturedData && lastProtocol != UNKNOWN) {
        Serial.println("📡 Sending...");
        irsend.write(lastProtocol, lastAddress, lastCommand, lastFlags);
        Serial.println("✅ Sent!");
      } else {
        Serial.println("❌ No data captured yet");
      }
    } 
    else if (s.equalsIgnoreCase("TEST")) {
      Serial.println("🔊 Serial OK!");
      Serial.print("📥 Has Data: ");
      Serial.println(hasCapturedData ? "YES" : "NO");
      Serial.print("📍 Protocol: ");
      Serial.println(lastProtocol);
    }
    else if (s.equalsIgnoreCase("SCAN")) {
      Serial.println("🔍 Scanning for IR signals...");
      Serial.println("Point remote at receiver and press any button");
      delay(1000);
    }
  }
  
  delay(10);
}
