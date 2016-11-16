#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>
#include <Ticker.h>

#ifndef AP_SSID
#define AP_SSID "Nelson-AP"
#endif

#ifndef AP_PASSWORD
#define AP_PASSWORD "nelson is great"
#endif

WiFiManager wifiManager;
HTTPClient client;

Servo myservo;
int pos = 180;
Ticker ticker;
String order_url;
const char* position;


struct OrderData {
  int last_modified;
  int position;
};

void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.6, tick);
  wifiManager.setDebugOutput(true);
  wifiManager.setAPCallback(configModeCallback); // When couldn't connect to a WiFi access point.

  if (!wifiManager.autoConnect(AP_SSID, AP_PASSWORD)) {
    Serial.println("Failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }

  Serial.println("Connected to the WiFi");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);

  String password = WiFi.macAddress();
  order_url = "http://token:" + password + "@nelson.alwaysdata.net/v1/buckets/default/collections/order/records?_sort=-last_modified&_limit=1";
  Serial.println("Order url: " + order_url);
  myservo.attach(D1);
}

void tick() {
  // Toggle led state.
  int state = digitalRead(BUILTIN_LED);
  digitalWrite(BUILTIN_LED, !state);
}

void configModeCallback(WiFiManager *wm) {
  ticker.attach(0.2, tick);
  Serial.println("Entered config mode");
  Serial.print("SSID: ");
  Serial.println(AP_SSID);
  Serial.print("Password: ");
  Serial.println(AP_PASSWORD);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println(wm->getConfigPortalSSID());
}


bool deserialize(OrderData& data, String json) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  data.position = root["data"][0]["position"];
  data.last_modified = root["data"][0]["last_modified"];
  return root.success();
}

void loop() {
  client.begin(order_url);
  int httpCode = client.GET();
  OrderData data;

  if (httpCode > 0) {
    Serial.println("Http header send and server answered");
    Serial.print("Status code received: ");
    Serial.println(httpCode);
    Serial.println("Payload received:");
    String payload = client.getString();
    Serial.println(payload);
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Parsing the json received");
      if (!deserialize(data, payload)) {
        Serial.println("parseObject() failed");
        return;
      }

      Serial.println("Decoded the json:");
      Serial.print("position: ");
      Serial.println(data.position);
      Serial.print("last modified: ");
      Serial.println(data.last_modified);
    }
  } else {
    Serial.println("Http GET failed");
  }
  client.end();

  Serial.print("moving the servo to: ");
  Serial.println(data.position);
  myservo.write(data.position);

  delay(1000);
}
