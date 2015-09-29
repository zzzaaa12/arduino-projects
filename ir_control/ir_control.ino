#include <IRremote.h>
#include <IRremoteInt.h>

#define PIN_LED 7
#define PIN_RELAY 9
#define PIN_IR_SEND 3 /* Define in library */
#define PIN_IR_RECV 6
#define PIN_BTN 10

IRsend irsend;
IRrecv irrecv(PIN_IR_RECV);

#define FAN_BUF_SIZE 23
unsigned int fanOpen[]		= {1200,500,1200,500,350,1250,1250,450,1250,450,350,1300,350,1300,350,1250,350,1300,350,1300,350,1300,1200};
unsigned int fanMode[]		= {1250,450,1250,450,350,1300,1200,500,1200,450,350,1300,350,1300,350,1300,350,1250,1250,450,350,1300,350};
unsigned int fanECO[] 		= {1250,500,1200,450,350,1300,1250,450,1200,500,1200,500,350,1250,350,1300,350,1300,350,1300,1200,450,1250};
unsigned int fanTimeToClose[]	= {1200,500,1200,500,350,1250,1250,450,1250,450,350,1300,350,1250,350,1300,1250,450,350,1300,350,1300,350};
unsigned int fanTimeToStart[]	= {1200,450,1250,450,350,1300,1200,500,1200,500,350,1250,1250,450,350,1300,350,1300,350,1250,350,1300,350};
unsigned int fanUp[]		= {1250,450,1200,500,350,1300,1200,450,1250,450,350,1300,350,1300,350,1250,350,1300,350,1300,1200,500,350};
unsigned int fanDown[]		= {1250,450,1250,450,350,1300,1200,500,1200,450,1250,450,350,1300,350,1300,350,1250,1250,450,1250,450,350};

void setup()
{
	Serial.begin(9600);
	Serial.println("Arduino start!");

	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_RELAY, OUTPUT);
	pinMode(PIN_BTN, INPUT);
	digitalWrite(PIN_RELAY, HIGH);

	irrecv.enableIRIn();
}

void loop()
{
	unsigned long v = 0x0;
	decode_results  results;
	int i;
	static int light_on = 1;
	static int relay_idle = 0;
	static int btn_on = 0;

	if (digitalRead(PIN_BTN)) {
		btn_on = 1;
		Serial.println("Button on");
	} else {
		btn_on = 0;
	}
	
	// read IR and Button
	if (!btn_on && !irrecv.decode(&results)) {
		digitalWrite(PIN_LED, LOW);
		goto next;
	}

	digitalWrite(PIN_LED, HIGH);
	Serial.println(results.value, HEX);
  
	if (results.value == 0xCE9400FF || results.value == 0xCE9434CB)		// Speak Open --> enter
		irsend.sendNEC(0x8E7D42B,32);
		
	else if (results.value == 0xCE9410EF || results.value == 0xCE945CA3)	// Speak Up --> right & up1
		irsend.sendNEC(0x8E73CC3,32);
		
	else if (results.value == 0xCE9450AF || results.value == 0xCE94DC23)	// Speak Down --> down & down1
		irsend.sendNEC(0x8E7B24D,32);
		
	else if ((results.value == 0xCE94B847 || btn_on) && relay_idle == 0) {
	  
		if (light_on) {
			digitalWrite(PIN_RELAY, LOW);
			light_on = 0;
			Serial.println("relay off");
		} else {
			digitalWrite(PIN_RELAY, HIGH);
			light_on = 1;
			Serial.println("relay on");
			
		}
		
		relay_idle = 15; // 15 * 100ms = 1.5 sec
	
	} else { /* For TECO FAN: need send many times */
		
		if (results.value == 0xCE9402FD || results.value == 0xCE94D42B) {// Fan LED disable --> blue
			for (i = 0; i < 60; i++) {
				irsend.sendRaw(fanECO, FAN_BUF_SIZE, 38);
				delay(20);
			}
		}
					
		for (i = 0; i < 10; i++) {
		  
			if (results.value == 0xCE94D02F)					// Fan Open --> play
				irsend.sendRaw(fanOpen, FAN_BUF_SIZE, 38);
				
			else if (results.value == 0xCE94C03F || results.value == 0xCE94B44B)	// Fan Time to Close --> cancel
				irsend.sendRaw(fanTimeToClose, FAN_BUF_SIZE, 38);
				
			else if (results.value == 0xCE94906F || results.value == 0xCE9452AD)	// Fan UP --> up & up2
				irsend.sendRaw(fanUp, FAN_BUF_SIZE, 38);
				
			else if (results.value == 0xCE94807F|| results.value == 0xCE94D22D)	// Fan Down --> down & down2
				irsend.sendRaw(fanDown, FAN_BUF_SIZE, 38);
				
			else
				break;
				
			delay(40);
		}
	}
 
	irrecv.resume();
	irrecv.enableIRIn();

next:
	if (relay_idle)
		relay_idle--;
		
	delay(100);
}
