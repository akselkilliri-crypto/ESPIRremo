#include <IRremote.h>

// 🔧 ПИНЫ (попробуйте разные, если не работает)
#define RECV_PIN  4    // GPIO 4 для IR приёмника

IRrecv irrecv(RECV_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println();
  Serial.println("=== IR Receiver Test ===");
  Serial.print("Listening on GPIO ");
  Serial.println(RECV_PIN);
  Serial.println("Point IR remote at receiver and press any button");
  Serial.println();
  
  irrecv.enableIRIn();
  Serial.println("✅ IR Receiver started");
}

void loop() {
  if (irrecv.decode()) {
    Serial.println("━━━━━━━━━━━━━━━━━━━━━━");
    Serial.println("📡 IR SIGNAL RECEIVED!");
    Serial.println("━━━━━━━━━━━━━━━━━━━━━━");
    
    // Протокол
    Serial.print("Protocol: ");
    Serial.print(irrecv.decodedIRData.protocol);
    if (irrecv.decodedIRData.protocol == UNKNOWN) {
      Serial.print(" (UNKNOWN)");
    }
    Serial.println();
    
    // Адрес
    Serial.print("Address: 0x");
    Serial.println(irrecv.decodedIRData.address, HEX);
    
    // Команда (HEX)
    Serial.print("Command: 0x");
    Serial.println(irrecv.decodedIRData.command, HEX);
    
    // Команда (DEC) - важно для RGB пультов!
    Serial.print("Command (DEC): ");
    Serial.println(irrecv.decodedIRData.command, DEC);
    
    // Биты
    Serial.print("Bits: ");
    Serial.println(irrecv.decodedIRData.numberOfBits);
    
    Serial.println("━━━━━━━━━━━━━━━━━━━━━━");
    Serial.println();
    
    irrecv.resume();
  }
  
  delay(50);
}
