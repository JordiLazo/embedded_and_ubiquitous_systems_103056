

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#define ADXL345 0x53

const char* ssid = "LAPTOP-DIDI";
const char* password = "DidacContra";
const char* mqtt_server = "test.mosquitto.org";

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;
int potpin=0;
String beats_str;
String message_json;
char beats[50];
 


unsigned long messageTimeColdoown = millis();
long intervalTimeMQTT= 1500;



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

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop() {

  /*** FUNCION HEART**/
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  

  

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
    beats_str = String(beatsPerMinute); //converting ftemp (the float variable above) to a string 
    message_json="{\"heart\":"+beats_str+"}";
    message_json.toCharArray(beats, message_json.length() + 1); //packaging up the data to publish to mqtt whoa...
   
    client.publish("SensorDidacLazo", beats);
  }
}
