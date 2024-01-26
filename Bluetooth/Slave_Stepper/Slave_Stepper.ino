#include <Stepper.h>
#include <SoftwareSerial.h>

#define TX 10
#define RX 11
SoftwareSerial slave(TX, RX); //TX, RX

//Set up for STEP
#define IN1STEP 4
#define IN2STEP 5
#define IN3STEP 6
#define IN4STEP 7

const int fullstep = 2048;
const int halfstep = fullstep / 4;
const int step;
const int stepper_speed = 15;
Stepper myStepper(fullstep, IN1STEP, IN3STEP, IN2STEP, IN4STEP);

void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(stepper_speed);
  slave.begin(9600);
}

void loop() {
  if (slave.available()) {
    char value = slave.read();
    Serial.print("Value: ");
    Serial.println(value);
    if (value == '0') {
      myStepper.step(fullstep / 4);
    } else if (value == '1') {
      myStepper.step(-fullstep / 6);
    }
  }
  delay(1000);
}
