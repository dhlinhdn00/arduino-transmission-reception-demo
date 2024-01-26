#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// Config
ESP8266WebServer server(80);

const char* ssid = "Linh";
const char* password = "0901176132@";

const int ENA = D6;
const int IN1 = D4;
const int IN2 = D5;
int speedMotor = 150;

const int LED = D0;

const int TRIG = D1;
const int ECHO = D2;
unsigned long duration;

const int LIGHT_OR_GAS = A0;

const int DHTPIN = D3; 
const int DHTTYPE = DHT11; 

DHT TEMP_HUM(DHTPIN, DHTTYPE);


float data;
String command;
unsigned long lastSensorReadTime = 0;
String currentState ="";

void connectWiFi() {
    Serial.println("Connecting to WiFi");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting...");
    }

    Serial.println("Connected to WiFi");
    Serial.println("IP Address: ");
    Serial.println(WiFi.localIP());
}

void DC_forward() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, speedMotor);
    Serial.println("Status: Forward");
}

void DC_backward() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, speedMotor);
    Serial.println("Status: Backward");
}

void DC_stop() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
    Serial.println("Status: Stop");
}

void measureDistance() {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    duration = pulseIn(ECHO, HIGH);
    data = duration / 2.0 / 29.1;
}

void measureLightOrGas() {
    data = analogRead(LIGHT_OR_GAS);
}

void measureTemperature() {
    data = TEMP_HUM.readTemperature();
}

void HTTP_handleRoot() {
    String page = "<html><head><title>Final Exam</title></head><body>"
                  "<img src=\"https://img2.thuthuatphanmem.vn/uploads/2018/12/31/hinh-anh-dong-de-thuong_112054094.gif\" id=\"gif\">"
                  "<h1>DC CONTROL</h1>"
                  "<form id='controlForm'>"
                  "<button type='button' onclick='sendCommand(\"forward\")'>Forward</button>"
                  "<button type='button' onclick='sendCommand(\"backward\")'>Backward</button>"
                  "<button type='button' onclick='sendCommand(\"stop!!!\")'>Stop</button>"
                  "<br><br>"
                  "<label for='speed'>SPEED:</label>"
                  "<input type='range' id='speed' name='speed' min='50' max='550' value='" + String(speedMotor) + "' onchange='updateSpeedDisplay(this.value)'>"
                  "<span id='speedDisplay'>" + String(speedMotor) + "</span>"
                  "<br><br>"
                  "<input type='button' value='Apply' onclick='sendSpeed()'>"
                  "</form>"
                  "<script>"
                  "function sendCommand(command) {"
                  "  var xhr = new XMLHttpRequest();"
                  "  xhr.open('GET', '/control?command=' + command, true);"
                  "  xhr.send();"
                  "}"
                  "function updateSpeedDisplay(value) {"
                  "  document.getElementById('speedDisplay').innerText = value;"
                  "}"
                  "function sendSpeed() {"
                  "  var speedValue = document.getElementById('speed').value;"
                  "  var xhr = new XMLHttpRequest();"
                  "  xhr.open('GET', '/control?command=apply&speed=' + speedValue, true);"
                  "  xhr.send();"
                  "}"
                  "</script>"
                  "<h1>Sensor Data</h1>"
                  "<p id=\"data\">Sensor Value: <span id=\"k\"> </span> </p>"
                  "<script>"
                  "setInterval(function() {"
                  "  data1();"
                  "}, 500);"
                  "function data1() {"
                  "  var xhttp = new XMLHttpRequest();"
                  "  xhttp.onreadystatechange = function() {"
                  "    if (this.readyState == 4 && this.status == 200) {"
                  "      document.getElementById(\"k\").innerHTML = this.responseText;"
                  "    }"
                  "  };"
                  "  xhttp.open(\"GET\", \"getdata1\", true);"
                  "  xhttp.send();"
                  "}"
                  "</script>"
                  "</body></html>";
    server.send(200, "text/html", page);
}

void HTTP_handleControl() {
    command = server.arg("command");

    if (command == "forward") {
        DC_forward();
    } else if (command == "backward") {
        DC_backward();
    } else if (command == "stop!!!") {
        DC_stop();
    } else if (command == "apply") {
        if (server.hasArg("speed")) {
            speedMotor = server.arg("speed").toInt();
            Serial.print("Current Speed: ");
            Serial.println(speedMotor);
        }
    }

    server.send(200, "text/html", "<html><body><h1>Command Received</h1></body></html>");
}

void senddata() {
    String s = String(data);
    server.send(200, "text/plain", s);
}

void setup() {
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    pinMode(LED, OUTPUT);

    pinMode(TRIG, OUTPUT); 
    pinMode(ECHO, INPUT);

    TEMP_HUM.begin();
    
    Serial.begin(9600);
    connectWiFi();

    server.on("/", HTTP_handleRoot);
    server.on("/control", HTTP_handleControl);
    server.on("/getdata1", senddata);

    server.begin();
}

void loop() {
    server.handleClient();
    if (Serial.available()) {
        currentState = Serial.readStringUntil('\n');
    }

    if (millis() - lastSensorReadTime > 500) {
        if (currentState == "distance") {
            measureDistance();
            Serial.print("Distance value: ");
            Serial.print(data);
            Serial.println(" cm");
        } else if (currentState == "light" || currentState == "gas") {
            measureLightOrGas();
            Serial.print(currentState == "light" ? "Light" : "Gas");
            Serial.print(" value: ");
            Serial.println(data);
        } else if (currentState == "temp") {
            measureTemperature();
            Serial.print("Temperature: ");
            Serial.println(data);
        } else if (currentState == "stop") {
            Serial.println("All modes stopped.");
            currentState = "";
        }
        lastSensorReadTime = millis();
    }
}
