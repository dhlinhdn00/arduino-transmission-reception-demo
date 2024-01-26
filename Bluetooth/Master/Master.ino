#include <SoftwareSerial.h>
#include <DHT.h>

SoftwareSerial master(10,11); // TX, RX

#define LED 13

#define TRIG 8
#define ECHO 7
unsigned long duration;
int distance;  

#define LIGHT A1

#define GAS A0

#define DHTPIN 9   
#define DHTTYPE DHT11
DHT TEMP_HUM(DHTPIN, DHTTYPE);
float temp, hum;

int value; 
void setup() {
    pinMode(LED, OUTPUT);

    pinMode(TRIG, OUTPUT); 
    pinMode(ECHO, INPUT);

    TEMP_HUM.begin();

    Serial.begin(9600);
    Serial.println("Master is ready");
    // Serial.println("Remember to select Both NL & CR in the serial");

    master.begin(9600);
}

String currentState = "";

void loop(){
    while (true){
        if (Serial.available()) {
            currentState = Serial.readStringUntil('\n');
        }
         if (currentState == "distance") {  
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
                Serial.println(F("Master sent 0"));
                master.write('0');
            } 
            else {
                Serial.println(F("Master sent 1"));
                master.write('1');
            }
        }
        else if (currentState == "light") {
            Serial.println();   
            int light_val = analogRead(LIGHT);
            Serial.print("Light value: ");
            Serial.println(light_val);
            if(light_val < 600) {
                Serial.println(F("Master sent 0"));
                master.write('0');
            } 
            else {
                Serial.println(F("Master sent 1"));
                master.write('1');
            }
        } 
        else if (currentState == "gas") {
            Serial.println(); 
            int gas_val = analogRead(GAS);
            Serial.print("Gas value: ");
            Serial.println(gas_val);
            if (gas_val > 300) {
                Serial.println(F("Master sent 0"));
                master.write('0'); 
            } 
            else {
                Serial.println(F("Master sent 1"));
                master.write('1');
            }
        }
        else if (currentState == "temp") {
            Serial.println(); 
            hum = TEMP_HUM.readHumidity();
            temp = TEMP_HUM.readTemperature();
            Serial.print("Temperature: ");
            Serial.println(temp);

            if (temp >= 30) {
                Serial.println(F("Master sent 0"));
                master.write('0');
            } 
            else {
                Serial.println(F("Master sent 1"));
                master.write('1');
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
