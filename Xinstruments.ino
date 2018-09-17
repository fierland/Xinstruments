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

#include <esp32_can.h>
#include <CommandLine.h>
#include <dummy.h>
#include <QList.h>
#include <MultiStepper.h>
#include <AccelStepper.h>
#include "Xinstruments.h"

//#ifdef USE_PWR_FLAG_SERVO
//#include <ESP32_Servo.h>
//#endif  

Xcomm *dataConnection;

// data to control the leds
int freq = 5000;
int ledChannel = 0;
int resolution = 8;

#ifdef DEBUG_CLI
// CommandLine instance.
CommandLine commandLine(Serial, "> ");
Command cmdUpdate	= Command("set",		&handleSet);
Command cmdReset	= Command("reset",		&handleReset);
Command cmdStatus	= Command("status",		&handleStatus);
Command cmdPulse	= Command("pulse",		&handlePulse);
Command cmdReverse	= Command("reverse",	&handleReverse);
Command cmdForward	= Command("forward",	&handleForward);
Command cmdOff		= Command("off",		&handleOff);
Command cmdOn		= Command("on",			&handleOn);
#endif

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
  int (*funcPointer)(float);
  
  DPRINTLN("Start Initiate steppers");

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
	_stepper_Pie_1 = new StepperPie(XI_STEP1_MAX_RANGE, XI_STEP1_MIN_RANGE, XI_STEP1_MAX_PIE, XI_STEP1_REVERSED,  XI_STEP1_STP,XI_STEP1_DIR, XI_STEP1_MOTORTYPE);
	_stepper_Pie_1->calibrate(XI_STEP1_MAX_BACKSTOP);

	dataConnection->addElement(XI_STEP1_ITEM, _stepper_Pie_1, XI_STEP2_MAX_RANGE, XI_STEP2_MIN_RANGE, true);
	#ifdef DEBUG
		_stepper_Pie_1->setValue(26);	
	#endif

#endif
#ifdef XI_STEP2_PIE
	_stepper_Pie_2 = new StepperPie(XI_STEP2_MAX_RANGE, XI_STEP2_MIN_RANGE, XI_STEP2_MAX_PIE, XI_STEP2_REVERSED,  XI_STEP2_STP,XI_STEP2_DIR, XI_STEP2_MOTORTYPE);
	_stepper_Pie_2->calibrate(XI_STEP2_MAX_BACKSTOP);
	dataConnection->addElement(XI_STEP2_ITEM, _stepper_Pie_2, XI_STEP2_MAX_RANGE, XI_STEP2_MIN_RANGE, true); 
	#ifdef DEBUG
		_stepper_Pie_2->setValue(26);
		_stepper_Pie_2->runToPosition();
		_stepper_Pie_2->setValue(10);
	#endif
#endif
#ifdef XI_STEP3_PIE
	_stepper_Pie_3 = new StepperPie(XI_STEP1_MAX_RANGE, XI_STEP3_MIN_RANGE, XI_STEP3_MAX_PIE, XI_STEP1_STP, XI_STEP1_DIR, XI_STEP3_MOTORTYPE);
	_stepper_Pie_3->calibrate(XI_STEP3_MAX_BACKSTOP);
	dataConnection->addElement(XI_STEP3_ITEM, _stepper_Pie_3, XI_STEP3_MAX_RANGE, XI_STEP3_MIN_RANGE, true);
#endif
#ifdef XI_STEP4_PIE
	_stepper_Pie_4 = new StepperPie(XI_STEP4_MAX_RANGE, XI_STEP4_MIN_RANGE, XI_STEP4_MAX_PIE,  XI_STEP4_STP, XI_STEP4_DIR, XI_STEP4_MOTORTYPE);
	_stepper_Pie_4->calibrate(XI_STEP4_MAX_BACKSTOP);
	dataConnection->addElement(XI_STEP4_ITEM, _stepper_Pie_4, XI_STEP4_MAX_RANGE, XI_STEP4_MIN_RANGE, true);
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
	DPRINT("***START SETUP***");
	Serial.begin(115200);
	// start communication with master
#ifdef DEBUG_CLI
	// CommandLine instance.
	commandLine.add(cmdUpdate);
	commandLine.add(cmdReset);	
	commandLine.add(cmdStatus);
	commandLine.add(cmdPulse);
  commandLine.add(cmdReverse);
  commandLine.add(cmdForward);
  commandLine.add(cmdOff);
  commandLine.add(cmdOn);
  
	// On-the-fly commands -- instance is allocated dynamically
	commandLine.add("help", handleHelp);
  // set up control of leds
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(XI_LED_PIN, ledChannel);
  ledcWrite(ledChannel, 255);
  
#endif

	_InitiateCommunication();


#ifdef USE_PWR_FLAG_SERVO
		// setup the power state flag
	_InitiateFlagStatus();
#endif

	_InitiateSteppers();
	DPRINT("***END SETUP***");
}

//===================================================================================================================
// MAIN LOOP
//===================================================================================================================


void loop() {
//
// check for new commands ad set instrument to latest values
//
//DPRINT("***LOOP***");


#ifdef DEBUG_CLI
	commandLine.update();
#endif
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
#ifdef XI_STEP3_PIE
	_stepper_Pie_3->run();
#endif
#ifdef XI_STEP4_PIE
	_stepper_Pie_4->run();
#endif

}

/**
* EXTRA DEBUG CODE 
* Handle the count command. The command has one additional argument that can be the integer to set the count to.
*
* @param tokens The rest of the input command.
*/

#ifdef DEBUG_CLI
void handleSet(char* tokens)
{
	char item[32];
	float value;

DPRINT("*in set function:");
DPRINT(tokens);
DPRINTLN(":");

	char* token = strtok(NULL, " ");

	if (token != NULL) {
		strcpy(item, token);		 
	}
	
	token = strtok(NULL, " ");

	if (token != NULL) {
		value = atof(token);

	}
	DPRINT("Command Update Item:");
	DPRINT(item);
	DPRINT(":");
	DPRINT(value);
	DPRINTLN(":");

	// call update
	dataConnection->processInput(item, value);

}

void handlePulse(char* tokens)
{
	float value;

	DPRINT("*in set function:");
	DPRINT(tokens);
	DPRINTLN(":");

	char* token = strtok(NULL, " ");


	if (token != NULL) {
		value = atof(token);
	}
	DPRINT("Pulse Update Item:");

	DPRINT(value);
	DPRINTLN(":");

	// call update
#ifdef XI_STEP1_PIE
	_stepper_Pie_1->setMinPulseWidth(value);
#endif
#ifdef XI_STEP2_PIE
	_stepper_Pie_2->setMinPulseWidth(value);
#endif
}

void handleReset(char* tokens)
{
	Serial.println("RESET DONE");
}

void handleStatus(char* tokens)
{
	Serial.println("STATUS DONE");
}

void handleHelp(char* tokens)
{
	Serial.println("Use the commands 'help', 'update <item> <value>', or 'reset'.");
}

void handleReverse(char* tokens)
{
  _stepper_Pie_2->setDirectionInverse(true);
  Serial.println("Reverse DONE");
}

void handleForward(char* tokens)
{
  _stepper_Pie_2->setDirectionInverse(false);
  Serial.println("Forward DONE");
}

void handleOff(char* tokens)
{
  _stepper_Pie_1->powerOff();
  _stepper_Pie_2->powerOff();
  ledcWrite(ledChannel, 0);
  Serial.println("PowerOFF DONE");
}

void handleOn(char* tokens)
{
  _stepper_Pie_1->powerOn();
  _stepper_Pie_2->powerOn();
 ledcWrite(ledChannel, 255);
  Serial.println("PowerON DONE");
}

#endif
