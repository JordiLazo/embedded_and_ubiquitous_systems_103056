#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

char textBuff[20];

const char* ssid = "LAPTOP-DIDI";
const char* password = "DidacContra";
const char* mqtt_server = "test.mosquitto.org";

unsigned long messageTimeColdoown = millis();
long intervalTimeMQTT= 1500;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  Wire.begin(0,2);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  

}

void loop(){
  if (!client.connected()) {
    reconnect();
  }
  Wire.requestFrom(8,1);
  if (Wire.available()){
    textBuff[0]=Wire.read();
    if( textBuff[0] == 'F')
    {
      client.publish("didacLazo/accelerometer", "Fall");
    }
    else{
      client.publish("didacLazo/accelerometer", &textBuff[0]);
    }
  }
  delay(2000);

}