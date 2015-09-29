#include "DHT.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define DHTPIN 7     // what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

void setup() {
	Serial.begin(9600); 
	Serial.println("DHT11 Sensor");
	lcd.begin(16, 2);
	lcd.noBlink();
	dht.begin();
}

void loop() {
	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	float h = dht.readHumidity();
	float t = dht.readTemperature();
	
	lcd.clear();

		
	if (isnan(t) || isnan(h)) {

		/* First Line */	
		lcd.setCursor(0, 0);
		lcd.print(" Failed to read  ");

		/* Second Line */		
		lcd.setCursor(0, 1);
		lcd.print(" from sensor!!!  ");

		/* Console */
		Serial.println("Failed to read from DHT");
		
	} else {
	
		/* First Line */
		lcd.setCursor(0, 0);
		lcd.print(" T: ");
		lcd.print(t, 0);
		lcd.print((char)223);
		lcd.print("C");
		
		lcd.setCursor(10, 0);
		if(t >= 30 || t < 20)
			lcd.print(">_<");
		else
			lcd.print("^_^");
 
		/* Second Line */
		lcd.setCursor(0, 1);
		lcd.print(" H: ");
		lcd.print(h, 0);
		lcd.print("%");
		lcd.setCursor(10, 1);
		if (h >= 85 || h < 50)
			lcd.print(">_<");
		else
			lcd.print("^_^");

		/* Console */	
		Serial.print("Humidity: "); 
		Serial.print(h);
		Serial.print(" %\t");
		Serial.print("Temperature: "); 
		Serial.print(t);
		Serial.println(" *C");
	}
	
	delay(10000);
}
