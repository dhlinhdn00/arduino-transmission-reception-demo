#include <IRremote.hpp>

#define LED 13
#define IR_RECEIVE_PIN 2
IRrecv receiver;

//Set up for DC
#define IN1DC 4
#define IN2DC 3
#define ENA 10
#define MAX_SPEED 255
#define MIN_SPEED 0
int dc_speed = 200;

void setup() {
    pinMode(LED, OUTPUT);

    Serial.begin(9600);

    pinMode(IN1DC, OUTPUT);
    pinMode(IN2DC, OUTPUT);
    pinMode(ENA, OUTPUT);

    receiver.begin(IR_RECEIVE_PIN);
    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
}

void loop() {
    if (receiver.decode()) { 
        receiver.printIRResultShort(&Serial); 

        if (receiver.decodedIRData.protocol == UNKNOWN) {
            Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
        } else {
            if (receiver.decodedIRData.command == 0x00) {
                // Backwward
                digitalWrite(LED, HIGH);
                Serial.println(F("LED turned ON"));
                digitalWrite(IN1DC, HIGH);
                digitalWrite(IN2DC, LOW);
                analogWrite(ENA, dc_speed);
                delay(1000);
            } else if (receiver.decodedIRData.command == 0x01) {
                // Forward
                digitalWrite(LED, LOW); 
                Serial.println(F("LED turned OFF"));
                digitalWrite(IN1DC, LOW);
                digitalWrite(IN2DC, HIGH);
                analogWrite(ENA, dc_speed);
                delay(1000);
            }
            // Serial.print("DC speed: ");
            // Serial.println(dc_speed);
        }
        receiver.resume();
    }
}
