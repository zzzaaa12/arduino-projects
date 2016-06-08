#include "DHT.h"
#include <LiquidCrystal.h>

/* lcd */
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/* dht11 */
#define DHTPIN 7
#define DHTTYPE DHT11
#define DATA_LEN 10 /* keep the last 10 data of DHT11 */
DHT dht(DHTPIN, DHTTYPE);

struct {
	int index;
	float t[DATA_LEN];
	float h[DATA_LEN];
} data;

int timer_sec = 0;
float last_t = -1, last_h = -1;
String apiKey = "HOVD8QWP3JG1E0R6";

void dht11_fail()
{
	/* First Line */
	lcd.setCursor(0, 0);
	lcd.print(" Failed to read  ");

	/* Second Line */
	lcd.setCursor(0, 1);
	lcd.print(" from sensor!!!  ");

	/* Console */
	Serial.println("Failed to read from DHT");
}

void print_to_lcd(float t, float h)
{
	lcd.clear();

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
}

void setup()
{
	int i;

	/* initial serial, lcd, and dht11 */
	Serial.begin(9600);
	lcd.begin(16, 2);
	lcd.noBlink();
	dht.begin();

	/* initial empty data */
	data.index = 0;
	for (i = 0; i < DATA_LEN; i++) {
		data.t[i] = 0;
		data.h[i] = 0;
	}
}

void loop()
{
	float t, h, total_t, total_h, average_t, average_h;
	char parameters[150];
	char str_t[3], str_h[3];
	int value_count = 0;
	int i;

	t = dht.readTemperature();
	h = dht.readHumidity();

	if (isnan(t) || isnan(h) || t < 1 || t > 50 || h < 0 || h > 100) {
		dht11_fail();
		goto out;
	}

	print_to_lcd(t, h);

	/* Print Serial /
	while (Serial.available())
		Serial.write(Serial.read()); */

	/* every 20 min: upload to thingspeak */
	if (timer_sec >= 1200) {
		total_t = 0;
		total_h = 0;

		/* get the average values from data */
		for (i = 0; i < DATA_LEN; i++) {
			if (data.t[i] > 0 && data.h[i] > 0) {
				total_t += data.t[i];
				total_h += data.h[i];
				value_count++;
			}
		}

		if (value_count > 0) {

			average_t = total_t/value_count;
			average_h = total_h/value_count;

			/* check error value */
			if (abs(average_t - t) >= 3) {
				t = average_t;
				Serial.println("get wrong temperature, replace by average_t");
			}

			if (abs(average_h - h) >= 4) {
				h = average_h;
				Serial.println("get wrong humidity, replace by average_h");
			}

		}

		/* send to thingspeak */
		dtostrf(h, 2, 0, str_h);
		dtostrf(t, 2, 0, str_t);
		sprintf(parameters, "&field1=%s&field2=%s", str_t, str_h);
		Serial.println("WTPOST+HOST:api.thingspeak.com,PORT:80,URI:update.json,DATA:api_key=HOVD8QWP3JG1E0R6" + String(parameters));
		timer_sec = 0;
	}

	/* every 2 min: save data */
	if ( timer_sec % 120 == 0) {
		data.t[timer_sec/120] = t;
		data.h[timer_sec/120] = h;
	}

out:
	timer_sec += 10;
	delay(10000);
}
