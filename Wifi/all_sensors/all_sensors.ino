#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include <DHT.h>

ESP8266WebServer server(80);

#define LED D0

#define TRIG D1
#define ECHO D2
unsigned long duration;
// int distance;  

#define LIGHT_OR_GAS A0
// int light_or_gas;


#define DHTPIN D3   
#define DHTTYPE DHT11 
DHT TEMP_HUM(DHTPIN, DHTTYPE);
// float temp, hum;

float data;

void senddata() {
    String s = String(data);
    server.send(200, "text/plain", s);
}

void setup() {
    pinMode(LED, OUTPUT);

    pinMode(TRIG, OUTPUT); 
    pinMode(ECHO, INPUT);

    TEMP_HUM.begin();

    Serial.begin(9600);
    WiFi.begin("Linh", "0901176132@");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Waiting to connect...");
    }

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", []() {
        String html = R"=====(
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <meta http-equiv="X-UA-Compatible" content="IE=edge">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Document</title>
                <style>
                    #k {
                        color: #f1660a;
                        font-size: 20px;
                    }
                  </style>
                <script>
                    setInterval(function() {
                        data1();
                    }, 500);

                    function data1() {
                        var xhttp = new XMLHttpRequest();
                        xhttp.onreadystatechange = function() {
                            if (this.readyState == 4 && this.status == 200) {
                                document.getElementById("k").innerHTML = this.responseText;
                            }
                        };
                        xhttp.open("GET", "getdata1", true);
                        xhttp.send();
                    }
                </script>
            </head>
            <body>
              <img src="https://img2.thuthuatphanmem.vn/uploads/2018/12/31/hinh-anh-dong-de-thuong_112054094.gif" id="gif">
              <p id="data">Value: <span id="k"> </span> </p>
              
            </body>
            </html>
        )=====";
        server.send(200, "text/html", html);
    });

    server.on("/getdata1", senddata);
    server.begin();
}

String currentState ="";

void loop() {
    while (true){
        server.handleClient();
        if (Serial.available()) {
            currentState = Serial.readStringUntil('\n');
        }
        if (currentState == "distance") { 
            // server.handleClient(); 
            Serial.println();       
            digitalWrite(TRIG, 0); 
            delayMicroseconds(2);
            digitalWrite(TRIG, 1);   
            delayMicroseconds(5);   
            digitalWrite(TRIG, 0);  
            duration = pulseIn(ECHO, HIGH);  
            data = int(duration/2/29.412);
            Serial.print("Distance value: ");
            Serial.print(data);
            Serial.println("cm");
        }
        else if (currentState == "light") {
            // server.handleClient(); 
            Serial.println();   
            data = analogRead(LIGHT_OR_GAS);
            Serial.print("Light value: ");
            Serial.println(data);
        } 
        else if (currentState == "gas") {
            // server.handleClient(); 
            Serial.println(); 
            data = analogRead(LIGHT_OR_GAS);
            Serial.print("Gas value: ");
            Serial.println(data);
        }
        else if (currentState == "temp") {
            // server.handleClient(); 
            Serial.println(); 
            // hum = TEMP_HUM.readHumidity();
            data = TEMP_HUM.readTemperature();
            Serial.print("Temperature: ");
            Serial.println(data);
            // Serial.print("Humidity: ");
            // Serial.println(data);
        }
        else if (currentState == "stop") {
            Serial.println();
            Serial.println("All modes stopped.");
            currentState = "";
        }
    delay(500); 
    }
}
