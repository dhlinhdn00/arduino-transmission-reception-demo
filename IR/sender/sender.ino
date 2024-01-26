// 9600 baud
#include <IRremote.hpp> 
#include <DHT.h>

#define IR_SEND_PIN 3
IRsend sender;

#define LED 13

#define TRIG 8
#define ECHO 7

#define LIGHT A1

#define GAS A0

#define DHTPIN 9   
#define DHTTYPE DHT11 

DHT TEMP_HUM(DHTPIN, DHTTYPE);

void setup() {
    pinMode(LED, OUTPUT);

    pinMode(TRIG, OUTPUT); 
    pinMode(ECHO, INPUT);

    TEMP_HUM.begin();

    Serial.begin(9600);
    Serial.print(F("Send IR signals at pin "));
    Serial.println(IR_SEND_PIN);

    sender.begin(IR_SEND_PIN);
}


int sCommand1 = 0x01;
int sCommand2 = 0x00;

String currentState = "";

unsigned long duration;
int distance;  

float temp, hum;

void loop() {
    while (true){
        if (Serial.available()) {
            currentState = Serial.readStringUntil('\n');
        }
        if (currentState == "demo1") {
            Serial.println();
            Serial.println(F("Sending command 0x01"));
            sender.sendNEC(0x00, sCommand1, 1);
        }
        else if (currentState == "demo2") {
            Serial.println();
            Serial.println(F("Sending command 0x00"));
            sender.sendNEC(0x00, sCommand2, 1); 
        }
        else if (currentState == "distance") {  
            Serial.println();       
            digitalWrite(TRIG, 0); 
            delayMicroseconds(2);
            digitalWrite(TRIG, 1);   
            delayMicroseconds(5);   
            digitalWrite(TRIG, 0);  
        
            duration = pulseIn(ECHO, HIGH);  

            distance = int(duration/2/29.412);
            
            Serial.print("Distance value: ");
            Serial.print(distance);
            Serial.println("cm");
            if (distance >= 10) {
                Serial.println(F("Sending command 0x01"));
                sender.sendNEC(0x00, sCommand1, 4); 
            } 
            else {
                Serial.println(F("Sending command 0x00"));
                sender.sendNEC(0x00, sCommand2, 4); 
            }
        }
        else if (currentState == "light") {
            Serial.println();   
            int light_val = analogRead(LIGHT);
            Serial.print("Light value: ");
            Serial.println(light_val);
            if(light_val < 600) {
                Serial.println(F("Sending command 0x01"));
                sender.sendNEC(0x00, sCommand1, 4); 
            } 
            else {
                Serial.println(F("Sending command 0x00"));
                sender.sendNEC(0x00, sCommand2, 4);
            }
        } 
        else if (currentState == "gas") {
            Serial.println(); 
            int gas_val = analogRead(GAS);
            Serial.print("Gas value: ");
            Serial.println(gas_val);
            if (gas_val > 300) {
                Serial.println(F("Sending command 0x01"));
                sender.sendNEC(0x00, sCommand1, 4); 
            } 
            else {
                Serial.println(F("Sending command 0x00"));
                sender.sendNEC(0x00, sCommand2, 4);
            }
        }
        else if (currentState == "temp") {
            Serial.println(); 
            // hum = TEMP_HUM.readHumidity();
            temp = TEMP_HUM.readTemperature();
            Serial.print("Temperature: ");
            Serial.println(temp);
            Serial.print("Humidity: ");
            Serial.println(hum);
            // if (hum >= 60)
            if (temp >= 30) {
                Serial.println(F("Sending command 0x01"));
                sender.sendNEC(0x00, sCommand1, 4); 
            } 
            else {
                Serial.println(F("Sending command 0x00"));
                sender.sendNEC(0x00, sCommand2, 4);
            }
        }
        else if (currentState == "stop") {
            Serial.println();
            Serial.println("All modes stopped.");
            currentState = "";
        }
    delay(1000); 
    }
}
