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

#include <Servo.h>
#include <QList.h>
#include <MultiStepper.h>
#include <AccelStepper.h>

#include <Xinstruments.h>


//#ifdef USE_PWR_FLAG_SERVO
//#include <ESP32_Servo.h>
//#endif  

Xcomm *dataConnection;

#ifdef USE_PWR_FLAG_SERVO         
boolean _powerIsOn = false;
boolean _communicationIsOn = false;
boolean _flagIsUp = true;
XServo *flagServo; // create servo object to control a servo      

#endif  
#ifdef XI_STEP1_360
Stepper360 *_stepper_360_1;
#endif
#ifdef XI_STEP2_360
Stepper360 *_stepper_360_2;
#endif
#ifdef XI_STEP3_360
Stepper360 *_stepper_360_3;
#endif
#ifdef XI_STEP4_360
Stepper360 *_stepper_360_4;
#endif
#ifdef XI_STEP1_PIE
StepperPie *_stepper_Pie_1;
#endif
#ifdef XI_STEP2_PIE
StepperPie *_stepper_Pie_2;
#endif
#ifdef XI_STEP3_PIE
StepperPie *_stepper_Pie_3;
#endif
#ifdef XI_STEP4_PIE
StepperPie *_stepper_Pie_4;
#endif



#ifdef USE_PWR_FLAG_SERVO
//===================================================================================================================
//  make sure power/staus flag is in open flagServoPosition when starting the device
//===================================================================================================================
void _InitiateFlagStatus() {
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

		flagServo->powerOn();
		flagServo->moveMin();
		flagServo->powerOff();
	}

	if (_flagIsUp && !(_powerIsOn && _communicationIsOn)) {
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

//===================================================================================================================
// initiate all used steppers 
//===================================================================================================================
void _InitiateSteppers() {
	// for full 360 steppers
#ifdef XI_STEP1_360
	_stepper_360_1 = new Stepper360(XI_STEP1_MAX_RANGE,  XI_STEP1_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP1_STP, XI_STEP1_DIR, 0, 0,true);
	_stepper_360_1->calibrate(XI_HAL1_PIN,true);
#endif
#ifdef XI_STEP2_360
	_stepper_360_2 = new Stepper360(XI_STEP2_MAX_RANGE,  XI_STEP2_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP2_STP, XI_STEP2_DIR, 0, 0, true);
	_stepper_360_2->calibrate(XI_HAL2_PIN,true);
#endif
#ifdef XI_STEP3_360
	_stepper_360_3 = new Stepper360(XI_STEP1_MAX_RANGE,  XI_STEP1_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP1_STP, XI_STEP1_DIR, 0, 0, true);
	_stepper_360_4->calibrate(XI_HAL1_PIN, true);
#endif
#ifdef XI_STEP4_360
	_stepper_360_4 = new Stepper360(XI_STEP4_MAX_RANGE,  XI_STEP4_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP4_STP, XI_STEP4_DIR, 0, 0, true);
	_stepper_360_4->calibrate(XI_HAL1_PIN, true);
#endif

	//-------------------------------------------------------------------------------------------------------------------
	// for pie type steppers
	//-------------------------------------------------------------------------------------------------------------------


#ifdef XI_STEP1_PIE
	_stepper_Pie_1 = new StepperPie(XI_STEP1_MAX_RANGE, XI_STEP1_MIN_RANGE, XI_STEP1_MAX_PIE, XI_STEP1_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP1_STP, XI_STEP1_DIR, 0, 0, true);
#endif
#ifdef XI_STEP2_PIE
	_stepper_Pie_2 = new StepperPie(XI_STEP2_MAX_RANGE, XI_STEP2_MIN_RANGE, XI_STEP2_MAX_PIE, XI_STEP2_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP2_STP, XI_STEP2_DIR, 0, 0, true);
#endif
#ifdef XI_STEP1_PIE
	_stepper_Pie_3 = new StepperPie(XI_STEP1_MAX_RANGE, XI_STEP3_MIN_RANGE, XI_STEP3_MAX_PIE, XI_STEP1_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP1_STP, XI_STEP1_DIR, 0, 0, true);
#endif
#ifdef XI_STEP4_PIE
	_stepper_Pie_4 = new StepperPie(XI_STEP4_MAX_RANGE, XI_STEP4_MIN_RANGE, XI_STEP4_MAX_PIE, XI_STEP4_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP4_STP, XI_STEP4_DIR, 0, 0, true);
#endif

}

//===================================================================================================================
// set up the communications with master
//===================================================================================================================

void _InitiateCommunication(){

	// TODO: write init comm code
	dataConnection = new Xcomm(XI_INSTRUMENT_CODE,XI_INSTRUMENT_MAX_ELEMENTS);
}



//===================================================================================================================
// MAIN SETUP PROC
//===================================================================================================================
void setup() {
	Serial.begin(115200);
	// start communication with master
	_InitiateCommunication();


#ifdef USE_PWR_FLAG_SERVO
		// setup the power state flag
	_InitiateFlagStatus();
#endif

	_InitiateSteppers();

}

//===================================================================================================================
// MAIN LOOP
//===================================================================================================================


void loop() {
//
// check for new commands ad set instrument to latest values
//
	dataConnection->checkQueue();

	// section to control a power/ready flag by a servo
#ifdef USE_PWR_FLAG_SERVO
	_UpdateFlagStatus();
#endif

// 
// move commands for all steppers
//
#ifdef XI_STEP1_360
	_stepper_360_1->run();
#endif
#ifdef XI_STEP2_360
	_stepper_360_2->run();
#endif
#ifdef XI_STEP3_360
	_stepper_360_3->run();
#endif
#ifdef XI_STEP4_360
	_stepper_360_4->run();
#endif
	// for pie type steppers


#ifdef XI_STEP1_PIE
	_stepper_Pie_1->run();
#endif
#ifdef XI_STEP2_PIE
	_stepper_Pie_2->run();
#endif
#ifdef XI_STEP1_PIE
	_stepper_Pie_3->run();
#endif
#ifdef XI_STEP4_PIE
	_stepper_Pie_4->run();
#endif

}

