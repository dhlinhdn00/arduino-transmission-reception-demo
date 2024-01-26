#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AccelStepper.h>
#include <DHT.h>

//Config
ESP8266WebServer server(80);

const char* ssid = "Linh";
const char* password = "0901176132@";

#define IN1 D5
#define IN2 D6
#define IN3 D7
#define IN4 D8
int stepperSpeed = 1000; // Good for 28BYJ-48
bool isRunning = false;
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);

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

void moveStepper(int steps) {
  stepper.move(steps);
  isRunning = true;
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
    String page = "<html><body><h1>Final Exam Check</h1>"
                "<img src=\"https://img2.thuthuatphanmem.vn/uploads/2018/12/31/hinh-anh-dong-de-thuong_112054094.gif\" id=\"gif\">"
                "<form id='controlForm'>"
                "<p>CONTROL PANEL</p>"
                "<select id='direction' name='direction'>"
                "<option value='forward'>Forward</option>"
                "<option value='backward'>Backward</option>"
                "</select>"
                "<br><br>"
                "<label for='stepSize'>STEP SIZE:</label>"
                "<input type='number' id='stepSizeInput' min='0' max='2048' value='2048' onchange='updateStepSizeSlider()'>"
                "<input type='range' id='stepSize' name='stepSize' min='0' max='2048' value='2048' onchange='updateStepSizeInput()'>"
                "<br><br>";

    page += "<input type='button' value='Apply' id='applyButton' onclick='sendSettings()' ";
    page += (isRunning ? "disabled" : "");
    page += ">"
        "<input type='button' value='Stop' id='stopButton' onclick='stopOperation()'>"
        "</form>"
        "<script>"
        "function updateStepSizeInput() {"
        "  var sliderValue = document.getElementById('stepSize').value;"
        "  document.getElementById('stepSizeInput').value = sliderValue;"
        "}"
        "function updateStepSizeSlider() {"
        "  var inputValue = document.getElementById('stepSizeInput').value;"
        "  if (inputValue < 0 || inputValue > 2048) {"
        "    alert('Value must be between 0 and 2048');"
        "    inputValue = Math.min(Math.max(inputValue, 0), 2048);"
        "    document.getElementById('stepSizeInput').value = inputValue;"
        "  }"
        "  document.getElementById('stepSize').value = inputValue;"
        "}"
        "function sendSettings() {"
        "  if (isRunning) return;"
        "  var direction = document.getElementById('direction').value;"
        "  var steps = (direction === 'forward' ? 1 : -1) * document.getElementById('stepSizeInput').value;"
        "  var xhr = new XMLHttpRequest();"
        "  xhr.open('GET', '/control?command=move&steps=' + steps, true);"
        "  xhr.send();"
        "}"
        "function stopOperation() {"
        "  var xhr = new XMLHttpRequest();"
        "  xhr.open('GET', '/control?command=stop', true);"
        "  xhr.send();"
        "}"
        "var isRunning = " + String(isRunning ? "true" : "false") + ";"
        "</script>"
        "</body></html>";


  // Append Sensor Data Section
    page += "<h2>Sensor Data</h2>"
          "<p>Current Value: <span id='sensorValue'>--</span></p>"
          "<script>"
          "setInterval(function() {"
          "  var xhttp = new XMLHttpRequest();"
          "  xhttp.onreadystatechange = function() {"
          "    if (this.readyState == 4 && this.status == 200) {"
          "      document.getElementById('sensorValue').innerHTML = this.responseText;"
          "    }"
          "  };"
          "  xhttp.open('GET', '/getdata1', true);"
          "  xhttp.send();"
          "}, 500);"
          "</script>";

    page += "</body></html>";

    server.send(200, "text/html", page);
}



void HTTP_handleControl() {
  String command = server.arg("command");

  if (command == "move") {
    if (!isRunning) {
      int steps = server.arg("steps").toInt();
      moveStepper(steps);
      stepper.runToPosition();
    }
  } else if (command == "stop") {
    stepper.stop();
    isRunning = false;
  }

  server.send(200, "text/html", "<html><body><h1>Command Processed</h1></body></html>");
}

void senddata() {
    String s = String(data);
    server.send(200, "text/plain", s);
}

void setup() {
    stepper.setMaxSpeed(stepperSpeed);
    stepper.setAcceleration(100);

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
  if (isRunning && !stepper.isRunning()) {
    isRunning = false;
  }
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

