#include "WiFi.h"
#include <WebServer.h>
#include <ESP32Servo.h>
#include <uri/UriBraces.h>

// const char* WIFI_USER     = "Morada_Passaros Lateral 2.4";
// const char* WIFI_PASSWORD = "maia1234";

const char* WIFI_USER     = "LAB-DIDATICO";
const char* WIFI_PASSWORD = "C1-13#lami#2017";

// const char* WIFI_USER     = "LAB_DIGITAL";
// const char* WIFI_PASSWORD = "C1-17*2018@labdig";

// const char* WIFI_USER     = "fas2612";
// const char* WIFI_PASSWORD = "pambrulurodana";

struct Locker {
  const int pin;
  Servo servo;
  bool unlocked;
};

Servo servo0;
Servo servo1;

Locker locker0 = Locker{27, servo0, false};
Locker locker1 = Locker{26, servo1, false};

WebServer server(80);

void setup() {
  setupSerialConnection();
  setupPinModes();
  setupWifiConnection();
  setupWebServer();
}

void setupSerialConnection() {
  Serial.begin(115200);
  Serial.println("### SETUP ###");
  Serial.println(" - Serial connection: OK");
}

void setupPinModes() {
  locker0.servo.attach(locker0.pin);
  locker1.servo.attach(locker1.pin);

  locker0.servo.write(180);
  locker1.servo.write(0);

  Serial.println(" - Pin modes: OK");
}

void setupWifiConnection() {
  Serial.println("   ### WIFI ###");
  Serial.printf("    - Trying to connect to %s ...", WIFI_USER);
  WiFi.begin(WIFI_USER, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.printf("    - Connected to %s\n", WIFI_USER);
  Serial.print("    - IP address: ");
  Serial.println(WiFi.localIP());
}

void setupWebServer() {
  server.on("/", homeHandler);
  server.on(UriBraces("/locker/{}"), changeState);

  server.begin();
  Serial.println(" - Server: OK");
}

void homeHandler() {
  sendResponse(200, "Welcome to ValuSafe Microcontroller Web Server!");
}

void changeState() {
  String id = server.pathArg(0);

  if (id == "0") {
    Serial.printf("Serial 0 angle: %d", locker0.servo.read());
    if (!locker0.unlocked) {
      locker0.servo.write(0);
      locker0.unlocked = true;
      return sendResponse(200, "Locker 0 unlocked with success.");      
    } else {
      locker0.servo.write(180);
      locker0.unlocked = false;
      return sendResponse(200, "Locker 0 locked with success.");
    }
  } else if (id == "1") {
    Serial.printf("Serial 1 angle: %d", locker1.servo.read());
    if (!locker1.unlocked) {
      locker1.servo.write(180);
      locker1.unlocked = true;
      return sendResponse(200, "Locker 1 unlocked with success.");      
    } else {
      locker1.servo.write(0);
      locker1.unlocked = false;
      return sendResponse(200, "Locker 1 locked with success.");
    }
  }

  return sendResponse(404, "Locker not found.");
}

void sendResponse(int code, String message) {
  String jsonMessage = String("{\"message\": \"" + message + "\"}");

  server.send(code, "application/json", jsonMessage);
}

void loop() {
  server.handleClient();
  delay(2);
}
