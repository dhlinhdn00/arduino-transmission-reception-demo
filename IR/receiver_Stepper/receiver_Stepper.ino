#include <IRremote.hpp>
#include <Stepper.h>

#define LED 13
#define IR_RECEIVE_PIN 2
IRrecv receiver;

//Set up for STEP
#define IN1STEP 8
#define IN2STEP 9
#define IN3STEP 10
#define IN4STEP 11
const int fullstep = 2048;
const int halfstep = fullstep/4;  // Respect to the degree like: 360/n.
const int step;
const int stepper_speed = 15;
Stepper myStepper(fullstep, IN1STEP, IN3STEP, IN2STEP, IN4STEP);

void setup() {
    pinMode(LED, OUTPUT);

    Serial.begin(9600);

    myStepper.setSpeed(stepper_speed);

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
                Serial.println("CASE1");
                myStepper.step(fullstep/4);
            } else if (receiver.decodedIRData.command == 0x01) {
                digitalWrite(LED, LOW); 
                Serial.println("CASE2");
                myStepper.step(-fullstep/6);
            } 
            delay(1000); 
        }
        receiver.resume();
        delay(1200);
    }
}
