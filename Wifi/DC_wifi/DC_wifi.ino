#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

const char* ssid = "Linh";
const char* password = "0901176132@";

const int ENA = D6;
const int IN1 = D4;
const int IN2 = D5;

String command;

int speedMotor = 150;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  Serial.begin(9600);

  connectWiFi();

  server.on("/", HTTP_handleRoot);
  server.on("/control", HTTP_handleControl);
  server.begin();
}

void loop() {
  server.handleClient();
}

void tien() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speedMotor);

  Serial.println("TIEN");
}

void lui() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speedMotor);

  Serial.println("Lui");
}

void dung() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  Serial.println("DUNG");
}

void HTTP_handleRoot() {
  String page ="<html><head><title>DC Control</title></head><body>"
               "<h1>DC CONTROL</h1>"
               "<form id='controlForm'>"
               "<button type='button' onclick='sendCommand(\"tien\")'>Forward</button>"
               "<button type='button' onclick='sendCommand(\"lui\")'>Backward</button>"
               "<button type='button' onclick='sendCommand(\"dung\")'>Stop</button>"
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
               "</body></html>";
  server.send(200, "text/html", page);
}


void HTTP_handleControl() {
  command = server.arg("command");

  if (command == "tien") {
    tien();
  } else if (command == "lui") {
    lui();
  } else if (command == "dung") {
    dung();
  } else if (command == "apply") {
    if (server.hasArg("speed")) {
      speedMotor = server.arg("speed").toInt();
      Serial.print("New Speed: ");
      Serial.println(speedMotor);
    }
  }

  server.send(200, "text/html", "<html><body><h1>Command Received</h1></body></html>");
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
