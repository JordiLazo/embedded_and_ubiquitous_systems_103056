#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "MAX30105.h"

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
MAX30105 particleSensor;
#define debug Serial

void setup()
{
  Serial.begin(9600);
	// initialize the LCD
	lcd.begin();

  debug.println("MAX30105 Basic Readings Example");

  // Initialize sensor
  if (particleSensor.begin() == false)
  {
    debug.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }

  particleSensor.setup(); //Configure sensor. Use 6.4mA for LED drive
}
	// Turn on the blacklight and print a message.

void loop()
{
  debug.print(" R[");
  debug.print(particleSensor.getRed());
  lcd.clear();
  debug.print("] IR[");
  debug.print(particleSensor.getIR());
  debug.print("] G[");
  debug.print(particleSensor.getGreen());
  debug.print("]");
  lcd.print(particleSensor.getGreen());
  delay(2000);
  debug.println();
}