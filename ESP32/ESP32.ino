#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

#define SERVO_PIN 26 // ESP32 pin GIOP26 connected to servo motor
LiquidCrystal_I2C lcd(0x27,16,2);
Servo servoMotor;
const char* ssid = "LAPTOP-DIDI";
const char* password = "DidacContra";
const char* mqtt_server = "test.mosquitto.org";
unsigned long messageTimeColdoown = millis();
long intervalTimeMQTT= 1500;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg=0;
char msg[50];

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("didacLazo");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0'; // Add a NULL to the end of the char* to make it a string.
  //int aNumber = atoi((char *)payload);
  String message =(char *)payload;
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  lcd.setCursor(0,1);
  lcd.print(message);
  /*
  if(aNumber != 0){
    lcd.clear();
    lcd.print(aNumber);
    if(aNumber > 90){
      for (int pos = 0; pos <= 180; pos += 1) {
        servoMotor.write(pos);
        delay(15); // waits 15ms to reach the position
        if(pos == 180){
          //lcd.clear();
          //lcd.print("Danger BPM");
        }
      }
    }else{
      for (int pos = 180; pos >= 0; pos -= 1) {
        servoMotor.write(pos);
        delay(15); // waits 15ms to reach the position
        if(pos == 0){
          //lcd.clear();
          //lcd.print("Reached 0 degrees");
        }
      }
    }
  }
  */
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}
void setup() {
  Serial.begin(115200);
  lcd.begin();
  lcd.clear();
  servoMotor.attach(SERVO_PIN);  // attaches the servo on ESP32 pin
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP32Client")) {
 
      Serial.println("connected");
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.subscribe("didacLazo");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
