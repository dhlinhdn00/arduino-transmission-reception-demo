#include <IRremote.hpp>
#include <Servo.h>

#define LED 13
#define IR_RECEIVE_PIN 2
IRrecv receiver;

#define SERVOPIN 5
Servo servo;
int pos = 180;
bool isAtZero = true; 

void setup() {
    pinMode(LED, OUTPUT);
    servo.write(pos); 
    Serial.begin(9600);

    servo.attach(SERVOPIN);

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
                digitalWrite(LED, HIGH);
                Serial.println(F("LED turned ON"));
                if (isAtZero) {
                    servo.write(0);
                    isAtZero = false; 
                } else {
                    servo.write(pos); 
                    isAtZero = true; 
                }
            } else if (receiver.decodedIRData.command == 0x01) {
                digitalWrite(LED, LOW); 
                Serial.println(F("LED turned OFF"));
                if (isAtZero) {
                    servo.write(90);
                    isAtZero = false; 
                } else {
                    servo.write(pos); 
                    isAtZero = true; 
                }
            }
            delay(1200);
        }
        receiver.resume(); 
    }
}
               