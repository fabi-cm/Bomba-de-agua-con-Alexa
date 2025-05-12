#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "SensorHumedad.h"
#include "ActuadorRiego.h"

#define PIN_SENSOR 39
#define PIN_RELE 4

const char* WIFI_SSID = "Wifi name";
const char* WIFI_PASS = "Wifi password";

const char* MQTT_BROKER = "a2nbbw2lfrp0hi-ats.iot.us-east-2.amazonaws.com";
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP32_Planta_este_si";

// Certificados (¡copia tus certificados aquí!)
const char AWS_ROOT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

const char DEVICE_CERT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

const char PRIVATE_KEY[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
)EOF";

const char* SHADOW_UPDATE = "$aws/things/prueba1/shadow/update";
const char* SHADOW_DELTA = "$aws/things/prueba1/shadow/update/delta";

WiFiClientSecure wiFiClient;
PubSubClient mqttClient(wiFiClient);
SensorHumedad sensor(PIN_SENSOR);
ActuadorRiego bomba(PIN_RELE);

void publishShadowState() {
  StaticJsonDocument<128> doc;
  doc["state"]["reported"]["humedad"] = sensor.leerHumedad();
  doc["state"]["reported"]["bomba"] = digitalRead(PIN_RELE) ? "ON" : "OFF";

  char jsonBuffer[128];
  serializeJson(doc, jsonBuffer);
  mqttClient.publish(SHADOW_UPDATE, jsonBuffer);
  Serial.println("Estado reportado a Shadow");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido en: ");
  Serial.println(topic);

  StaticJsonDocument<128> doc;
  deserializeJson(doc, payload, length);

  if (String(topic) == SHADOW_DELTA) {
    if (doc["state"].containsKey("bomba")) {
      bool estado = doc["state"]["bomba"] == "ON";
      digitalWrite(PIN_RELE, estado ? HIGH : LOW);
      Serial.println("Bomba " + String(estado ? "ENCENDIDA" : "APAGADA"));
      publishShadowState();
    }
  }
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando a AWS IoT...");
    if (mqttClient.connect(CLIENT_ID)) {
      Serial.println("Conectado!");
      mqttClient.subscribe(SHADOW_DELTA);
      Serial.println("Suscrito a " + String(SHADOW_DELTA));
      publishShadowState();
    } else {
      Serial.print("Error, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Reintentando en 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RELE, OUTPUT);
  digitalWrite(PIN_RELE, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");

  wiFiClient.setCACert(AWS_ROOT_CA);
  wiFiClient.setCertificate(DEVICE_CERT);
  wiFiClient.setPrivateKey(PRIVATE_KEY);
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(callback);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 5000) {
    lastUpdate = millis();
    publishShadowState();
    Serial.println("Humedad: " + String(sensor.leerHumedad()) + "%");
  }
}