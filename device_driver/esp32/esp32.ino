const int ledPin = 15;
int ledState = LOW;
int ledState2 = LOW;
unsigned long previousMillis = 0;

const long interval1 = 500;
const long interval2 = 250;

void setup (){
  pinMode(ledPin,OUTPUT);
  pinMode(15,OUTPUT);
}
void loop(){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval1){
    previousMillis =  currentMillis;
    if(ledState == LOW){
      ledState = HIGH;
    }else{
      ledState = LOW;
    }
    digitalWrite(ledPin,ledState);
  }
  if(currentMillis - previousMillis >= interval2){
    previousMillis =  currentMillis;
    if(ledState2 == LOW){
      ledState = HIGH;
    }else{
      ledState2 = LOW;
    }
    digitalWrite(15,ledState);
  }
}