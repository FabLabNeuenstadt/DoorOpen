#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include "config.h"

int inputPin = 5; // D1

int seconds_until_trigger = 5;
 
bool door_open = false;
unsigned long last_open_trigger = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

const char* url_open = "http://URL_USER:URL_PASS@door.fablab-neuenstadt.de/open";
const char* url_close = "http://URL_USER:URL_PASS@door.fablab-neuenstadt.de/close";

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(inputPin, INPUT);
  digitalWrite(inputPin, HIGH);

}

void request(const char* url) {
  HTTPClient http;
  http.begin(url); //HTTP
  Serial.printf("[HTTP] GET... %s\n", url);
  // start connection and send HTTP header
  int httpCode = http.GET();
  if(httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void loop() {
  // reboot if not connected to wifi
  if (WiFi.status() == WL_DISCONNECTED) {
    ESP.reset();
  }
  
  unsigned long now = millis();
  bool sensor_status = digitalRead(inputPin);
  if (door_open != sensor_status) {
    lastDebounceTime = now;
  }
  if ((now - lastDebounceTime) > debounceDelay) {
    if ((!door_open) && (sensor_status)) {
      Serial.println("We are open! :-D");
      last_open_trigger = now;
      door_open = true;
    } else if ((door_open) && (!sensor_status)) {
      door_open = false;
      Serial.println("We are closed! :-(");
      request(url_close);
      last_open_trigger = now;
    }
  }

  if ((door_open) && (now > (last_open_trigger + seconds_until_trigger * 1000))) {
    request(url_open);
    last_open_trigger = now; // To re-trigger after a few minutes
  } else if (now > (last_open_trigger + seconds_until_trigger * 1000)) {
    request(url_close);
    last_open_trigger = now;
  }
}
