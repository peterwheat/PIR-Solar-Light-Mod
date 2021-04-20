#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define POWERPIN 2 
const char* ssid0 = "SSID";
const char* password0 = "password";
const char* mqtt_server = "192.168.0.202";
const char* security_area = "InFrontOfGarage";
WiFiClient espClient;

PubSubClient client(espClient);

void setup_wifi() { 
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid0, password0);
  unsigned long timeout = millis();
  while ((WiFi.status() != WL_CONNECTED) && (millis() - timeout < 20000)) {
    delay(500);
  }
  if (WiFi.status() != WL_CONNECTED) digitalWrite(POWERPIN, LOW);   //Turn power off
}

void callback(char* topic, byte* payload, unsigned int length) 
{
} 

void reconnect() {
  unsigned long timeout = millis();
  while ((!client.connected()) && (millis() - timeout < 30000)) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (!client.connect(clientId.c_str())) {
      delay(3000);
    }
  }
}

void setup() {
  //Latch power on
  pinMode(POWERPIN, OUTPUT);
  digitalWrite(POWERPIN,HIGH); 
  randomSeed(micros());
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void loop() {
  if (!client.connected()) {
    reconnect();
    if (!client.connected())digitalWrite(POWERPIN, LOW);//Turn power off
  }
  client.publish("security/area",security_area);
  client.loop();
  client.disconnect();
  //Turn power off
  digitalWrite(POWERPIN, LOW);
}
