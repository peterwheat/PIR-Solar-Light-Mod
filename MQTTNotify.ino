#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ds3231.h>
#define BUZZERPIN 13
const char* ssid0 = "SSID";
const char* password0 = "password";
const char* mqtt_server = "192.168.0.202";
unsigned long lastMessageMillis = 0; 
unsigned long currentMillis = 0;
const long interval = 60000; 
ts t; 

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

WiFiClient espClient;

PubSubClient client(espClient);

void buzzBeep() {
  digitalWrite(BUZZERPIN,HIGH);   
  delay(200);
  digitalWrite(BUZZERPIN, LOW);
}

void setup_wifi() {
  //Serial.println("WIFI");  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid0, password0);
  unsigned long timeout = millis();
  while ((WiFi.status() != WL_CONNECTED) && (millis() - timeout < 20000)) {
    delay(500);
  }
  if (WiFi.status() != WL_CONNECTED) {
        //Display WIFI connection failed message
          lcd.clear();
          lcd.backlight();
          lcd.setCursor(0,0);
          lcd.print("WIFI failed to");
          lcd.setCursor(0,1);
          lcd.print("connect.");
          buzzBeep();
        lastMessageMillis = millis();
      }
   }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  DS3231_get(&t);
  lcd.clear();
  lcd.backlight();
  char dtTm[16];
  sprintf(dtTm, "%02u/%02u/%04u-%02u:%02u", t.mday, t.mon, t.year,t.hour,t.min);
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i,0);
    lcd.print((char)dtTm[i]);
  }
  for (int i = 0; i < length; i++) {   
    lcd.setCursor(i,1);
    lcd.print((char)payload[i]);
  }
  buzzBeep();
  lastMessageMillis = millis();
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe("#");
    } else {
          lcd.clear();
          lcd.backlight();
          lcd.setCursor(0,0);
          lcd.print("MQTT failed");
          lcd.setCursor(0,1);
          lcd.print("to connect.");
          buzzBeep();
          delay(5000);
    }
  }
}

void setup() {
  pinMode(BUZZERPIN, OUTPUT);     
  randomSeed(micros());
  Wire.begin(); 
  lcd.init();  
  DS3231_init(DS3231_INTCN);
  /*
  t.hour=12; 
  t.min=52;
  t.sec=0;
  t.mday=5;
  t.mon=2;
  t.year=2021;
  DS3231_set(t);  
  */
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  currentMillis = millis();
  if (lastMessageMillis != 0) {
   if (currentMillis - lastMessageMillis >= interval) {
    //Clear Display
    lcd.clear();
    lcd.noBacklight();
    lastMessageMillis = 0;
   }
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
