#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

ESP8266WebServer server(80);

const char* ssid = "Linh";
const char* password = "0901176132@";

#define SERVOPIN 5
Servo myservo;
int pos = 0;

void setup() {
  Serial.begin(9600);
  connectWiFi();

  myservo.attach(SERVOPIN); // Attach servo to pin D5

  server.on("/", HTTP_handleRoot);
  server.on("/control", HTTP_handleControl);

  server.begin();
}

void loop() {
  server.handleClient();
}

void HTTP_handleRoot() {
  String page = "<html><body><h1>Servo Control</h1>"
                "<form id='controlForm'>"
                "<p>CONTROL PANEL</p>"
                "<label for='angle'>Angle (0-180):</label>"
                "<input type='number' id='angle' name='angle' min='0' max='180' value='" + String(pos) + "' onchange='updateAngleValue()'>"
                "<input type='button' value='Apply' onclick='sendSettings()'>"
                "</form>"
                "<script>"
                "function updateAngleValue() {"
                "  var angle = document.getElementById('angle').value;"
                "  if (angle < 0 || angle > 180) {"
                "    alert('Angle must be between 0 and 180');"
                "    document.getElementById('angle').value = Math.min(Math.max(angle, 0), 180);"
                "  }"
                "}"
                "function sendSettings() {"
                "  var angle = document.getElementById('angle').value;"
                "  var xhr = new XMLHttpRequest();"
                "  xhr.open('GET', '/control?command=move&angle=' + angle, true);"
                "  xhr.send();"
                "}"
                "</script>"
                "</body></html>";

  server.send(200, "text/html", page);
}

void HTTP_handleControl() {
  String command = server.arg("command");

  if (command == "move") {
    int angle = server.arg("angle").toInt();
    myservo.write(angle);
    pos = angle; // Update the position
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
