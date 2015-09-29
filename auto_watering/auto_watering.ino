#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define PIN_BTN_A 6
#define PIN_RELAY 8
#define WATER_TIMER 120
#define IDLE_TIMER 1800

/* Define for LCD */
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup()
{
	Serial.begin(9600);
	Serial.println("Arduino start!");
	pinMode(PIN_BTN_A, INPUT);
	pinMode(PIN_RELAY, OUTPUT);
	lcd.begin(16, 2);
	for (int i = 0; i < 3; i++) {
		lcd.noBacklight();
		delay(250);
		lcd.backlight();
		delay(250);
	}
}

void loop()
{
	static int btn_a,i,sensor_value,level;
	static int watering_timer = WATER_TIMER;
	static int idle_timer = IDLE_TIMER;
	unsigned long v = 0x0;

	btn_a = digitalRead(PIN_BTN_A);
	if (btn_a)
		Serial.println("button pushed!!!");

	/* Get sensor value and determine level */
	sensor_value = analogRead(A0);
	level = 11 - (sensor_value - 200) / 70;

	if (level > 10)
		level = 10;
	else if (level < 0)
		level = 0;

	Serial.print("sensor = ");
	Serial.print(sensor_value);
	Serial.print(", level = ");
	Serial.println(level);
		
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Level:");
	
	for (i = 0; i < level; i++)
		lcd.print("#");

	/* If button be pushed or level is between 1~5 --> watering 
	   But when level = 0, we determine no sensor */
	if (btn_a || (level > 0 && level <= 5 && watering_timer)) { // status: watering
		lcd.setCursor(7,1);
		lcd.print("watering");
		digitalWrite(PIN_RELAY, HIGH);
		if (btn_a == 0) {
			lcd.setCursor(0,1);
			lcd.print(watering_timer);
			watering_timer--;
		}
		
	} else {
		if (level == 0) { // status:no sensor
			lcd.setCursor(6,1);
			lcd.print("no sensor");
		} else {// status: idle
			lcd.setCursor(0,1);
			lcd.print(idle_timer);
			lcd.setCursor(9,1);
			lcd.print("idle");
		}
		digitalWrite(PIN_RELAY, LOW);
		idle_timer--;
	}

	if (idle_timer == 0) {
		idle_timer = IDLE_TIMER;
		watering_timer = WATER_TIMER;
	}

next:
	 delay(1000);
}
