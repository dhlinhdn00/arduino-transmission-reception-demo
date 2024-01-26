#include <SoftwareSerial.h>

#define TX 10
#define RX 11
SoftwareSerial slave(TX, RX);  // TX, RX

//Set up for DC
#define IN1DC 4
#define IN2DC 3
#define ENA 10
#define MAX_SPEED 255
#define MIN_SPEED 0
int dc_speed = 200;

void Stop() { 
    digitalWrite(IN1DC, LOW);
    digitalWrite(IN2DC, LOW);
}

void Forward(int speed) {
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);  // Ensure that the value in range [0;255]
    digitalWrite(IN1DC, HIGH);
    digitalWrite(IN2DC, LOW);
    analogWrite(ENA, speed);
}

void Backward(int speed) {
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    digitalWrite(IN1DC, LOW);
    digitalWrite(IN2DC, HIGH);
    analogWrite(ENA, speed);
}

void setup() {
    pinMode(IN1DC, OUTPUT);
    pinMode(IN2DC, OUTPUT);
    pinMode(ENA, OUTPUT);

    Serial.begin(9600);
    Serial.println("Slave is ready");
    slave.begin(9600);
}


void loop() {
  if (slave.available()) {
      char value = slave.read();
      Serial.print("Value: ");
      Serial.println(value);
      if (value == '0') {
          Forward(200);
          delay(1000);
      } else if (value == '1') {
          Backward(200);
          delay(1000);
      }
  }
}
