//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================

//-------------------------------------------------------------------------------------------------------------------
//	 Basic Instrument Control

//   Franks Flightsim Intruments project
//	by Frank van Ierland
//
//	This code is in the public domain.
//
//	VERSION HISTORY:
//
//
//-------------------------------------------------------------------------------------------------------------------
// TODO: move setup to eeprom
// TODO: OTA software update

//#ifdef USE_PWR_FLAG_SERVO
//#include <ESP32_Servo.h>
//#endif

/* will move to instrument code

#ifdef USE_PWR_FLAG_SERVO
boolean _powerIsOn = false;
boolean _communicationIsOn = false;
boolean _flagIsUp = true;
//XServo *flagServo; // create servo object to control a servo

#endif

#ifdef USE_PWR_FLAG_SERVO
//===================================================================================================================
//  make sure power/staus flag is in open flagServoPosition when starting the device
//===================================================================================================================
void _InitiateFlagStatus()
{
	flagServo = new XServo(0, 300, 0, 50, PWR_FLAG_SERVO_PIN);  // create servo object to control a servo
   //flagServo->attach(PWR_FLAG_SERVO_PIN, 500, 2400);   // attaches t0 servo on pin 18 to the servo object
													  // using default min/max of 1000us and 2000us
													  // different servos may require different min/max settings
													  // for an accurate 0 to 180 sweep
	flagServo->moveMax();
	flagServo->powerOff();
}

//===================================================================================================================
// section to control a power/ready flag by a servo
//===================================================================================================================
void _UpdateFlagStatus()
{
	uint32_t read_raw = analogRead(XI_PWR_MON_PIN);
	DPRINTLN(read_raw);

	_powerIsOn = (read_raw > 4000);
	DPRINT("Power state:");
	DPRINTLN(_powerIsOn);

	if (!_flagIsUp && (_powerIsOn && _communicationIsOn))
	{
		// change state of flag from off to on
		DPRINT("flag to on :");
		DPRINTLN(flagServoPos);

		_flagIsUp = false;

		flagServo->powerOn();
		flagServo->moveMin();
		flagServo->powerOff();
	}

	if (_flagIsUp && !(_powerIsOn && _communicationIsOn))
	{
		// change state of flag from off to off
		DPRINT("flag to off :");
		DPRINTLN(flagServoPos);

		_flagIsUp = true;

		flagServo->powerOn();
		flagServo->moveMax();
		flagServo->powerOff();
	}
}
#endif
*/
//===================================================================================================================
// MAIN SETUP PROC
//===================================================================================================================
#include <Instrument.h>
void setup()
{}

//===================================================================================================================
// MAIN LOOP
//===================================================================================================================

void loop()
{
	//
	// check for new commands ad set instrument to latest values
	//

//Serial.println("***LOOP***");

	// section to control a power/ready flag by a servo

	//
	// move commands for all steppers
	//
	//delay(10);
}