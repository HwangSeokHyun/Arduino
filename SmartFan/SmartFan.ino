#define ON true
#define OFF false

int REED = 11;
int CDS = 7;
int PIR = 0;

int relay = 48;
int buzzer = 42;

int cds;
int reed;
int pir;

int command;
int mode;
int timerCount;

boolean power;
boolean light;
boolean timer;
boolean exception;

void setup() {
	pinMode(relay, OUTPUT);
	pinMode(buzzer, OUTPUT);
	init_param();
        exception = OFF;
}

void loop() {
	delay(1000);

        if(power == ON)
         {
        	if (mode == 4) {
	  	        timerMode();
        	} 
                else 
                {
	        	setMode();
		        notTimerMode();
        	}
        }
        else
        {
                reed = analogRead(REED);
                if(reed>10)
                {
                        power = ON;
	                Serial.begin(9600);
                	printExplain();
                }
        }
}

void init_param() {
	light = OFF;
	power = OFF;
	timer = OFF;
	timerCount = 0;
	mode = 1;
}
void timerMode() {
	if (mode == 4 && timer == OFF) {
		if (Serial.available() > 0) {
			command = Serial.read();
			if (command == '1' || command == '2' || command == '3'
					|| command == '4' || command == '5' || command == '6'
					|| command == '7' || command == '8' || command == '9') {

                                      timerCount = (command-48)*10*60;
//				timerCount = (command - 48) * 10;
				timer = ON;
			} else {
				Serial.println("Unknown command. Please, Retype time(1~9)");
			}
		}		
	}
        if (timer) {
		if (timerCount == 0) {
			digitalWrite(relay, LOW);
			timer = OFF;
		}
		Serial.println(timerCount);
		timerCount--;
	}
	reed = analogRead(REED);
	if (reed < 10) {
		init_param();
	}
}

void notTimerMode() {
	readAnalog();
	setLight();
	setPower();

	if (power && light && exception == OFF) {
		digitalWrite(relay, HIGH);
		detectMotion();
	} else {
		digitalWrite(relay, LOW);
	}
}

void setMode() {
	if (Serial.available() > 0) {
		command = Serial.read();
		if (command == '1') {
			mode = 1;
			Serial.println("Default mode");
		} else if (command == '2') {
			mode = 2;
			Serial.println("No baby mode");
		} else if (command == '3') {
			mode = 3;
			Serial.println("In dark mode");
		} else if (command == '4') {
			mode = 4;
			Serial.println("Timer mode");
			Serial.println("Please, Type time(1~9)");
			Serial.println("The timer will be counted time which is multiplied ten minutes");
		} else {
			Serial.print("Unknown command:");
			Serial.println(command);
		}
	}
}

void printExplain() {
	Serial.println("1:Default. 2:No baby. 3:In dark. 4:Timer mode.");
}

void readAnalog() {
	cds = analogRead(CDS);
	reed = analogRead(REED);
	pir = analogRead(PIR);
}

void detectMotion() {
	if (mode == 2) {} 
	else {
		if (pir > 450) {
                        exception = ON;
			Serial.println("Warning!");                        
			digitalWrite(relay, LOW);
			init_param();
                        digitalWrite(buzzer, HIGH);
                        delay(1000);
                        digitalWrite(buzzer, LOW);
		}
	}
}

void setLight() {
	if (mode == 3) {} 
	else {
		if (cds > 300) {
			light = ON;
		} else {
			light = OFF;
		}
	}
}

void setPower() {
	if (reed > 10) {
		power = ON;
	} else {
		power = OFF;
                exception = OFF;
                Serial.end();
	}
}

