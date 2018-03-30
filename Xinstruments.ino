//-------------------------------------------------------------------------------------------------------------------
/* Basic Instrument Control
*  Franks Flightsim Intruments project
by Frank van Ierland

This code is in the public domain.

VERSION HISTORY:

*/
//-------------------------------------------------------------------------------------------------------------------
#include <Xinstruments.h>



#ifdef USE_PWR_FLAG_SERVO
#include <ESP32_Servo.h>
#endif  


#ifdef USE_PWR_FLAG_SERVO
Servo flagServo;  // create servo object to control a servo                
int flagServoflagServoPos = 0;    // variable to store the servo flagServoPosition
boolean _powerIsOn = false;
boolean _communicationIsOn = false;
boolean _flagIsUp = true;
#endif  


#ifdef USE_PWR_FLAG_SERVO

/*
*    make sure power/staus flag is in open flagServoPosition when starting the device
*/
void _InitiateFlagStatus() {
	flagServo.attach(PWR_FLAG_SERVO_PIN, 500, 2400);   // attaches t0 servo on pin 18 to the servo object
													   // using default min/max of 1000us and 2000us
													   // different servos may require different min/max settings
													   // for an accurate 0 to 180 sweep
	for (flagServoflagServoPos = 0; flagServoflagServoPos <= PWR_FLAG_SERVO_MAXPOS; flagServoflagServoPos += 1) { // goes from 180 degrees to 0 degrees
		flagServo.write(flagServoflagServoPos);
		delay(15);
	}
	flagServo.detach();
}

/*
* section to control a power/ready flag by a servo
*/
void _UpdateFlagStatus() {
	uint32_t read_raw = analogRead(PWR_FLAG_ADC_PIN);
	DPRINTLN(read_raw);

	_powerIsOn = (read_raw >4000);
	DPRINT("Power state:");
	DPRINTLN(_powerIsOn);

	if (!_flagIsUp && (_powerIsOn && __communicationIsOn))
	{
		// change state of flag from off to on
		DPRINT("flag to on :");
		DPRINTLN(flagServoPos);

		_flagIsUp = false;

		flagServo.attach(PWR_FLAG_SERVO_PIN, 500, 2400);
		for (; flagServoPos >= 0; flagServoPos -= 1) {
			flagServo.write(flagServoPos);
			delay(15);
		} // tell servo to go to flagServoPosition in variable 'flagServoPos'
		flagServo.detach();
	}

	if (_flagIsUp && !(_powerIsOn && __communicationIsOn)) {
		// change state of flag from off to off
		DPRINT("flag to off :");
		DPRINTLN(flagServoPos);

		_flagIsUp = true;

		flagServo.attach(PWR_FLAG_SERVO_PIN, 500, 2400);
		for (; flagServoPos <= PWR_FLAG_SERVO_MAXPOS; flagServoPos += 1) {
			flagServo.write(flagServoPos);
			delay(15);
		} // tell servo to go to flagServoPosition in variable 'flagServoPos'
		flagServo.detach();
	}
}
#endif                


void setup() {
	Serial.begin(115200);


#ifdef USE_PWR_FLAG_SERVO
	_InitiateFlagStatus()
#endif


}



void loop() {

	// section to control a power/ready flag by a servo
#ifdef USE_PWR_FLAG_SERVO
	_UpdateFlagStatus()
#endif

}

