#include <SoftwareSerial.h>
#include <Servo.h>

#define TX 10
#define RX 11
SoftwareSerial slave(10, 11);  // TX, RX

#define SERVOPIN 5
Servo servo;
int pos = 180;          // Position at starting
bool isAtZero = true;   // Position check at starting

void setup() {
    servo.attach(SERVOPIN);
    servo.write(pos);

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
          Serial.println(F("Case 1")); // Modify it if needed
          if (isAtZero) {
              servo.write(0);          // Modify this value for CASE 1
              isAtZero = false; 
          } else {
              servo.write(pos); 
              isAtZero = true; 
          }
      } else {
          Serial.println(F("Case 2")); // Modify it if needed
          if (isAtZero) {
              servo.write(90);         // Modify this value for CASE 2
              isAtZero = false; 
          } else {
              servo.write(pos); 
              isAtZero = true; 
          }
      }
      delay(500);
  }
}
