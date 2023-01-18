

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "LAPTOP-DIDI";
const char* password = "DidacContra";
const char* mqtt_server = "test.mosquitto.org";


#include <Wire.h>
#define ADXL345 0x53

int x,y,z;
char textBuff[20];


WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void writeTo(int device, byte address, byte val){
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.write(val);
  Wire.endTransmission();
}

void readFrom(int device, byte address, int num, byte buff[]){
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.endTransmission();

  Wire.beginTransmission(device);
  Wire.requestFrom(device,num);

  for(int cont=0;cont<num;cont++){
    buff[cont]=Wire.read();    
  }
  Wire.endTransmission();
}

void readADXL(){

  byte buff[6];
  
  writeTo(ADXL345, 0x31, 0);
  readFrom(ADXL345, 0x32, 6, buff);

  x=(((int)buff[1]<<8) | buff[0]);

  y=(((int)buff[3]<<8) | buff[2]);

  z=(((int)buff[5]<<8) | buff[4]);
  
}


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

/*void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}*/

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
  
  Wire.begin(2,0); //SDA - SCL
  delay(1);
  
  //ADXL345 turn On
  writeTo(ADXL345, 0x2D,0x00);
  writeTo(ADXL345, 0x2D,0x10);
  writeTo(ADXL345, 0x2D,0x08);

  Serial.println("ADXL ON");
  
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  readADXL();
  
  sprintf(textBuff, "X:%d Y:%d Z:%d",x,y,z);

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    Serial.print("Publish message: ");
    Serial.println(textBuff);
    client.publish("SensorDidacLazo/Heart", "Hola Caracola");
  }
}
