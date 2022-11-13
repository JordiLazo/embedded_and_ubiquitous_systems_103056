#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>

#define SERVO_PIN 26 // ESP32 pin GIOP26 connected to servo motor
LiquidCrystal_I2C lcd(0x27,16,2);
Servo servoMotor;

void setup() {
  Serial.begin(115200);
  lcd.clear();
  lcd.begin();
  lcd.backlight();
  lcd.print("Hello, world!");
  servoMotor.attach(SERVO_PIN);  // attaches the servo on ESP32 pin
}

void loop() {
  // rotates from 0 degrees to 180 degrees
  for (int pos = 0; pos <= 180; pos += 1) {
    // in steps of 1 degree
    servoMotor.write(pos);
    delay(15); // waits 15ms to reach the position
    if(pos == 180){
      lcd.clear();
      lcd.print("Reached 180 degrees");
    }
  }

  // rotates from 180 degrees to 0 degrees
  for (int pos = 180; pos >= 0; pos -= 1) {
    servoMotor.write(pos);
    delay(15); // waits 15ms to reach the position
    if(pos == 0){
      lcd.clear();
      lcd.print("Reached 0 degrees");
    }
  }
}
