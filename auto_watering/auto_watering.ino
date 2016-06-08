#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define PIN_BTN_A 6
#define PIN_RELAY 8
#define WATER_MIN_SEC 10
#define WATER_MAX_SEC 60
#define IDLE_MAX_SEC 1800

/* Define for LCD */
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

enum ACRION {WATERING, NO_SENSOR, IDLE};
int watering_timer = WATER_MAX_SEC;
int idle_timer = IDLE_MAX_SEC;
int water_count = 0;
int status;

void do_action(int action)
{
	switch(action)	{
	case WATERING:
		lcd.setCursor(7,1);
		lcd.print("watering");
		digitalWrite(PIN_RELAY, HIGH);
		if (status != WATERING)
			water_count++;
		status = WATERING;
		break;
	case NO_SENSOR:
		lcd.setCursor(6,1);
		lcd.print("no sensor");
		status = NO_SENSOR;
		break;
	case IDLE:
		lcd.setCursor(0,1);
		lcd.print(idle_timer);
		lcd.setCursor(6,1);
		lcd.print(water_count);
		lcd.setCursor(9,1);
		lcd.print("idle");
		status = IDLE;
		break;
	default:
		lcd.setCursor(0,1);
		lcd.print("ERROR");
	}
}

void setup()
{
	pinMode(PIN_BTN_A, INPUT);
	pinMode(PIN_RELAY, OUTPUT);
	Serial.begin(115200);
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
	static int btn_a, i, sensor_value, level, ratio;
	unsigned long v = 0x0;

	btn_a = digitalRead(PIN_BTN_A);
	if (btn_a)
		Serial.println("button pushed!!!");

	/* Get sensor value and determine level */
	sensor_value = analogRead(A0);
	ratio = 110 - (sensor_value - 150) / 8;

	if (ratio > 100)
		ratio = 100;
	else if (ratio < 0)
		ratio = 0;

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Level:");
	level = ratio / 10;
	
	for (i = 0; i < level; i++)
		lcd.print("#");

	/* If button be pushed or level is between 1~5 --> watering
	   But when level = 0, we determine no sensor */
	if (btn_a) {
		/* Push button: watering and reset idle_timer */
		do_action(WATERING);
		idle_timer = 0;
	} else if (level > 0 && level < 5 && watering_timer) {
		/* Auto watering */
		do_action(WATERING);
		lcd.setCursor(0,1);
		lcd.print(watering_timer);
		delay(WATER_MIN_SEC * 1000);
		watering_timer -= WATER_MIN_SEC;
	} else {
		if (level == 0) /* No sensor */
			do_action(NO_SENSOR);
		else /* Idle */
			do_action(IDLE);
		digitalWrite(PIN_RELAY, LOW);
		idle_timer--;
	}

	if (idle_timer == 0) {
		idle_timer = IDLE_MAX_SEC;
		watering_timer = WATER_MAX_SEC;
	}
next:
	delay(1000);
}
