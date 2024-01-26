#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AccelStepper.h>

ESP8266WebServer server(80);

const char* ssid = "Linh";
const char* password = "0901176132@";

int stepperSpeed = 1000; // Tốc độ mặc định phù hợp với 28BYJ-48
bool isRunning = false;

#define IN1 D5
#define IN2 D6
#define IN3 D7
#define IN4 D8

AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(9600);
  connectWiFi();

  stepper.setMaxSpeed(stepperSpeed);
  stepper.setAcceleration(100);

  server.on("/", HTTP_handleRoot);
  server.on("/control", HTTP_handleControl);

  server.begin();
}

void loop() {
  server.handleClient();
  if (isRunning && !stepper.isRunning()) {
    isRunning = false;
  }
}

void moveStepper(int steps) {
  stepper.move(steps);
  isRunning = true;
}

void HTTP_handleRoot() {
  String page = "<html><body><h1>Final Exam Check</h1>"
                "<form id='controlForm'>"
                "<p>CONTROL PANEL</p>"
                "<select id='direction' name='direction'>"
                "<option value='forward'>Forward</option>"
                "<option value='backward'>Backward</option>"
                "</select>"
                "<br><br>"
                "<label for='stepSize'>STEP SIZE:</label>"
                "<span id='stepSizeValue'>2048</span>"
                "<input type='range' id='stepSize' name='stepSize' min='0' max='2048' value='2048' onchange='updateStepSizeValue(this.value)'>"
                "<br><br>";

  page += "<input type='button' value='Apply' id='applyButton' onclick='sendSettings()' ";
  page += (isRunning ? "disabled" : "");
  page += ">"
          "<input type='button' value='Stop' id='stopButton' onclick='stopOperation()'>"
          "</form>"
          "<script>"
          "function updateStepSizeValue(value) {"
          "  document.getElementById('stepSizeValue').innerText = value;"
          "}"
          "function sendSettings() {"
          "  if (isRunning) return;"
          "  var direction = document.getElementById('direction').value;"
          "  var steps = (direction === 'forward' ? 1 : -1) * document.getElementById('stepSize').value;"
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
