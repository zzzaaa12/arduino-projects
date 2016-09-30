#define PIN_RELAY 8
#define PIN_IN 2
#define PIN_OUT 3
String cmd = "";
String relay_status = "up";
boolean stringComplete = false;

void setup() {
	pinMode(PIN_RELAY, OUTPUT);
	digitalWrite(PIN_RELAY, HIGH);
	Serial.begin(115200);
	Serial.println("# Relay ---> Up");
	Serial.print("\ncmd: ");
}

void loop() {
	read_serial();
	if (stringComplete) {
		if (cmd == "up") {
			if (relay_status == "up") {
				Serial.println("# Error: Relay was up already!!!");
				goto out;
			}
			Serial.println("# Please wait 3 secs");
			delay(3000);
			Serial.println("# Relay --> Up");
			digitalWrite(PIN_RELAY, HIGH);
			relay_status = "Up";

		} else if (cmd == "down") {
			if (relay_status == "down") {
				Serial.println("Error: Relay was down already!!!");
				goto out;
			}
			Serial.println("# Relay --> Down");
			digitalWrite(PIN_RELAY, LOW);
			relay_status = "Down";

		} else if (cmd == "reset") {
			Serial.println("# Relay --> Down");
			Serial.println("# Please wait 3 secs");
			digitalWrite(PIN_RELAY, LOW);
			relay_status = "Down";
			delay(3000);
			Serial.println("# Relay --> Up");
			digitalWrite(PIN_RELAY, HIGH);
			relay_status = "Up";

		} else if (cmd == "status") {
			Serial.println("# Relay status: " + relay_status);

		} else if (cmd == "help" || cmd == "?") {
			Serial.println("up:\tSet relay up");
			Serial.println("down:\tSet relay down");
			Serial.println("reset:\tSet relay down and set up after 3 secs");
			Serial.println("status:\tShow relay status");
			Serial.println("help:\tShow this page");

		} else {
			Serial.println("Error command!!!");
		}
out:
		cmd = "";
		stringComplete = false;
		Serial.print("\ncmd: ");
	}

	delay(1000);
}

void read_serial() {
	while (Serial.available()) {
		char inChar = (char)Serial.read();
		if (inChar == '\n' || inChar == '\r') {
			stringComplete = true;
			Serial.println("");
			return;
		}
		Serial.print(inChar);
		cmd += inChar;
	}
}
