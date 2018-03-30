//-------------------------------------------------------------------------------------------------------------------
/* Basic Instrument Control
*  Franks Flightsim Intruments project
by Frank van Ierland

This code is in the public domain.

VERSION HISTORY:

*/
//-------------------------------------------------------------------------------------------------------------------

#include <QList.h>
#include <MultiStepper.h>
#include <AccelStepper.h>

#include <Xinstruments.h>


//#ifdef USE_PWR_FLAG_SERVO
//#include <ESP32_Servo.h>
//#endif  


#ifdef USE_PWR_FLAG_SERVO
ServoPie flagServo(0,300,0,50,PWR_FLAG_SERVO_PIN);  // create servo object to control a servo                
int flagServoPos = 0;    // variable to store the servo flagServoPosition
boolean _powerIsOn = false;
boolean _communicationIsOn = false;
boolean _flagIsUp = true;
#endif  
Stepper360 _stepper_360_1;
Stepper360 _stepper_360_2;
Stepper360 _stepper_360_3;
Stepper360 _stepper_360_4;

StepperPie _stepper_Pie_1;
StepperPie _stepper_Pie_2;
StepperPie _stepper_Pie_3;
StepperPie _stepper_Pie_4;


#ifdef USE_PWR_FLAG_SERVO

/*
*    make sure power/staus flag is in open flagServoPosition when starting the device
*/
void _InitiateFlagStatus() {
	//flagServo.attach(PWR_FLAG_SERVO_PIN, 500, 2400);   // attaches t0 servo on pin 18 to the servo object
													   // using default min/max of 1000us and 2000us
													   // different servos may require different min/max settings
													   // for an accurate 0 to 180 sweep
	for (flagServoPos = 0; flagServoPos <= PWR_FLAG_SERVO_MAXPOS; flagServoPos += 1) { // goes from 180 degrees to 0 degrees
		flagServo.moveTo(flagServoPos);
		delay(15);
	}
	flagServo.powerOff();
}

/*
* section to control a power/ready flag by a servo
*/
void _UpdateFlagStatus() {
	uint32_t read_raw = analogRead(XI_PWR_MON_PIN);
	DPRINTLN(read_raw);

	_powerIsOn = (read_raw >4000);
	DPRINT("Power state:");
	DPRINTLN(_powerIsOn);

	if (!_flagIsUp && (_powerIsOn && _communicationIsOn))
	{
		// change state of flag from off to on
		DPRINT("flag to on :");
		DPRINTLN(flagServoPos);

		_flagIsUp = false;

		flagServo.powerOn();
		flagServo.moveMin();
		flagServo.powerOff();
	}

	if (_flagIsUp && !(_powerIsOn && _communicationIsOn)) {
		// change state of flag from off to off
		DPRINT("flag to off :");
		DPRINTLN(flagServoPos);

		_flagIsUp = true;

		flagServo.powerOn();
		flagServo.moveMax();
		flagServo.powerOff();
	}
}
#endif                

void _InitiateSteppers() {
	// for full 360 steppers
#ifdef XI_STEP1_360
	_stepper_360_1 =Stepper360(XI_STEP1_MAX_RANGE, XI_HAL1_PIN, XI_STEP1_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP1_STP, XI_STEP1_DIR, 0, 0,true);
#endif
#ifdef XI_STEP2_360
	_stepper_360_2 = Stepper360(XI_STEP2_MAX_RANGE, XI_HAL2_PIN, XI_STEP2_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP2_STP, XI_STEP2_DIR, 0, 0, true);
#endif
#ifdef XI_STEP1_360
	_stepper_360_3 = Stepper360(XI_STEP1_MAX_RANGE, XI_HAL1_PIN, XI_STEP1_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP1_STP, XI_STEP1_DIR, 0, 0, true);
#endif
#ifdef XI_STEP4_360
	_stepper_360_4 = Stepper360(XI_STEP4_MAX_RANGE, XI_HAL2_PIN, XI_STEP4_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP4_STP, XI_STEP4_DIR, 0, 0, true);
#endif
// for pie type steppers


#ifdef XI_STEP1_360
	_stepper_Pie_1 = StepperPie(XI_STEP1_MAX_RANGE, XI_STEP1_MIN_RANGE, XI_STEP1_MAX_PIE, XI_STEP1_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP1_STP, XI_STEP1_DIR, 0, 0, true);
#endif
#ifdef XI_STEP2_360
	_stepper_Pie_2 = StepperPie(XI_STEP2_MAX_RANGE, XI_STEP2_MIN_RANGE, XI_STEP2_MAX_PIE, XI_STEP2_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP2_STP, XI_STEP2_DIR, 0, 0, true);
#endif
#ifdef XI_STEP1_360
	_stepper_Pie_3 = StepperPie(XI_STEP1_MAX_RANGE, XI_STEP3_MIN_RANGE, XI_STEP3_MAX_PIE, XI_STEP1_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP1_STP, XI_STEP1_DIR, 0, 0, true);
#endif
#ifdef XI_STEP4_360
	_stepper_Pie_4 = StepperPie(XI_STEP4_MAX_RANGE, XI_STEP4_MIN_RANGE, XI_STEP4_MAX_PIE, XI_STEP4_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP4_STP, XI_STEP4_DIR, 0, 0, true);
#endif

}

void _InitiateCommunication(){
}

void _PingMaster() {

}



void setup() {
	Serial.begin(115200);
	// start communication with master
	_InitiateCommunication();


#ifdef USE_PWR_FLAG_SERVO
		// setup the power state flag
	_InitiateFlagStatus()
#endif


}



void loop() {

	_PingMaster();
	// section to control a power/ready flag by a servo
#ifdef USE_PWR_FLAG_SERVO
	_UpdateFlagStatus()
#endif

#if defined(XI_STEP1_360) || defined(XI_STEP1_PIE)
		allSteppers.Run();
#endif

}

