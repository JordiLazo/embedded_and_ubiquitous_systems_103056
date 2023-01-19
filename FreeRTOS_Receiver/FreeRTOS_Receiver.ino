#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
//VARIABLES GLOBALS
#define SERVO_PIN 26
#define BUZZZER_PIN  18
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servoMotor;
const char *ssid = "LAPTOP-DIDI";
const char *password = "DidacContra";
const char *mqtt_server = "test.mosquitto.org";
unsigned long messageTimeColdoown = millis();
long intervalTimeMQTT = 1500;


int melody[] = {
  440, 1760, 440, 1760, 440, 1760, 440, 1760
};

int noteDurations[] = {
  12, 12, 8, 8, 4, 4, 4, 4
};

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//DEFINICIO TASQUES
void MQTTReceiver(void *pvParameters);
void ActivityMonitor(void *pvParameters);

QueueHandle_t HeartQueue;
QueueHandle_t AccelerometerQueue;

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
      client.subscribe("didacLazo/Heart");
      client.subscribe("didacLazo/Accelerometer");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  payload[length] = '\0';  // Add a NULL to the end of the char* to make it a string.
  int aNumber = atoi((char *)payload);
  String message = (char *)payload;
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  String topic_string = topic;
  if (strcmp(topic_string.c_str(), "didacLazo/Accelerometer") == 0) {
    Serial.println("Guardado en queue Accelerometer");
    xQueueSend(AccelerometerQueue, &aNumber, pdMS_TO_TICKS(50));
  } else if (strcmp(topic_string.c_str(), "didacLazo/Heart") == 0) {
    Serial.println("Guardado en queue Heart");
    xQueueSend(HeartQueue, &aNumber, pdMS_TO_TICKS(50));
    if(aNumber>90){
      servoMotor.write(45);
    }else{
      servoMotor.write(160);
    }
  }

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void MQTTReceiver(void *pvParameters) {
  Serial.begin(115200);
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

  client.subscribe("didacLazo/Heart");
  client.subscribe("didacLazo/Accelerometer");
  for (;;) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }
}
void ActivityMonitor(void *pvParameters) {
  
  int *heartReceivedValue = 0;
  int *accelerometerReceivedValue = 0;
  lcd.begin();

  for (;;) {
    xQueueReceive(HeartQueue, &heartReceivedValue, pdMS_TO_TICKS(100));
    xQueueReceive(AccelerometerQueue, &accelerometerReceivedValue, pdMS_TO_TICKS(100));
    lcd.setCursor(0, 0);
    if (accelerometerReceivedValue == 0) {
      lcd.print("STATE: NORMAL");
    } else {
      lcd.print("STATE: FALL  ");
      for (int thisNote = 0; thisNote < 8; thisNote++) {
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(BUZZZER_PIN, melody[thisNote], noteDuration);

        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(BUZZZER_PIN);
      }
      vTaskDelay(1500);
    }
    lcd.setCursor(0, 1);
    char myStg[20];
    sprintf(myStg, "HEART: %d  ", heartReceivedValue);
    Serial.print(myStg);
    lcd.print(myStg);


    vTaskDelay(300);
  }
}

void setup() {
  HeartQueue = xQueueCreate(10, sizeof(uint32_t));
  AccelerometerQueue = xQueueCreate(10, sizeof(uint32_t));
  servoMotor.attach(SERVO_PIN);

  xTaskCreatePinnedToCore(
    MQTTReceiver, "MQTTReceiver", 2048, NULL, 1, NULL, ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    ActivityMonitor, "ActivityMonitor", 2048, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
}

void loop() {
  // put your main code here, to run repeatedly:
}
