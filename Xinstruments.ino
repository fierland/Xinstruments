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

#include <esp32_can.h>
#include <stdlib.h>
#include <CommandLine.h>
#include <dummy.h>
#include <QList.h>
#include "Xinstruments.h"
#include "src\XLibs\src\mydebug.h"
#include "src\Instruments\src\Instrument.h"
#include "src\Instruments\src\XServo.h"
#include "src\Instruments\src\GenericIndicator.h"
#include "src\Instruments\src\IndicatorStepper.h"
#include "src\Instruments\src\StepperPie.h"
#include "src\Instruments\src\Stepper360.h"
#include "src\XLibs\src\XPUtils.h"
#include "src\CanAs\src\CANaero.h"
#include "src\XLibs\src\Xcomm.h"

//#ifdef USE_PWR_FLAG_SERVO
//#include <ESP32_Servo.h>
//#endif

Instrument	myInstrument(XI_Instrument_Code, XI_Instrument_NodeID, XI_Instrument_Service_Chan, XI_LED_PIN, XI_INSTRUMENT_MAX_ELEMENTS);

#ifdef XI_STEP1_PIE
StepperPie _stepper_Pie_1(XI_DAIL1_CAN_ID, XI_STEP1_MAX_RANGE, XI_STEP1_MIN_RANGE, XI_STEP1_MAX_PIE, XI_STEP1_REVERSED, XI_STEP1_STP, XI_STEP1_DIR, XI_STEP1_MOTORTYPE);
#endif

#ifdef XI_STEP2_PIE
StepperPie _stepper_Pie_2(XI_DAIL2_CAN_ID, XI_STEP2_MAX_RANGE, XI_STEP2_MIN_RANGE, XI_STEP2_MAX_PIE, XI_STEP2_REVERSED, XI_STEP2_STP, XI_STEP2_DIR, XI_STEP2_MOTORTYPE);
_stepper_Pie_2.setConversionFactor(XI_DAIL1_CONVERIONS_FACTOR);
#endif
#ifdef XI_STEP3_PIE
StepperPie _stepper_Pie_3(XI_DAIL3_CAN_ID, XI_STEP1_MAX_RANGE, XI_STEP3_MIN_RANGE, XI_STEP3_MAX_PIE, XI_STEP1_STP, XI_STEP1_DIR, XI_STEP3_MOTORTYPE);
#endif
#ifdef XI_STEP4_PIE
StepperPie _stepper_Pie_4(XI_DAIL4_CAN_ID, XI_STEP4_MAX_RANGE, XI_STEP4_MIN_RANGE, XI_STEP4_MAX_PIE, XI_STEP4_STP, XI_STEP4_DIR, XI_STEP4_MOTORTYPE);
#endif
#ifdef XI_STEP1_360
Stepper360 _stepper_360_1(XI_DAIL1_CAN_ID, XI_STEP1_MAX_RANGE, XI_STEP1_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP1_STP, XI_STEP1_DIR, 0, 0, true);
#endif
#ifdef XI_STEP2_360
Stepper360 _stepper_360_2(XI_DAIL2_CAN_ID, XI_STEP2_MAX_RANGE, XI_STEP2_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP2_STP, XI_STEP2_DIR, 0, 0, true);
#endif
#ifdef XI_STEP3_360
Stepper360 _stepper_360_3(XI_DAIL3_CAN_ID, XI_STEP1_MAX_RANGE, XI_STEP1_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP1_STP, XI_STEP1_DIR, 0, 0, true);
#endif
#ifdef XI_STEP4_360
Stepper360 _stepper_360_4(XI_DAIL4_CAN_ID, XI_STEP4_MAX_RANGE, XI_STEP4_STEPS_CIRCLE, AccelStepper::DRIVER, XI_STEP4_STP, XI_STEP4_DIR, 0, 0, true);
#endif

#ifdef DEBUG_CLI
// CommandLine instance.
CommandLine commandLine(Serial, "> ");
Command cmdUpdate = Command("set", &handleSet);
Command cmdReset = Command("reset", &handleReset);
Command cmdStatus = Command("status", &handleStatus);
Command cmdPulse = Command("pulse", &handlePulse);
Command cmdReverse = Command("reverse", &handleReverse);
Command cmdForward = Command("forward", &handleForward);
Command cmdOff = Command("off", &handleOff);
Command cmdOn = Command("on", &handleOn);
#endif

#ifdef USE_PWR_FLAG_SERVO
boolean _powerIsOn = false;
boolean _communicationIsOn = false;
boolean _flagIsUp = true;
XServo *flagServo; // create servo object to control a servo

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
	int(*funcPointer)(float);

	DPRINTLN("Start Initiate steppers");

	//-------------------------------------------------------------------------------------------------------------------
	// first for for pie type steppers we can calibrate them at the same time
	//-------------------------------------------------------------------------------------------------------------------

#ifdef XI_STEP1_PIE
	_stepper_Pie_1.setConversionFactor(XI_DAIL1_CONVERIONS_FACTOR);
	myInstrument.addIndicator(&_stepper_Pie_1);
	_stepper_Pie_1.moveToBackstop();

#ifdef DEBUG
	_stepper_Pie_1.setValue(26);
#endif

#endif
#ifdef XI_STEP2_PIE
	_stepper_Pie_2.setConversionFactor(XI_DAIL2_CONVERIONS_FACTOR);
	myInstrument.addIndicator(&_stepper_Pie_2);
	_stepper_Pie_2.moveToBackstop();

#ifdef DEBUG
	_stepper_Pie_2.setValue(26);
	_stepper_Pie_2.runToPosition();
	_stepper_Pie_2.setValue(10);
#endif
#endif
#ifdef XI_STEP3_PIE
	_stepper_Pie_3.setConversionFactor(XI_DAIL3_CONVERIONS_FACTOR);
	myInstrument.addIndicator(&_stepper_Pie_3);
	_stepper_Pie_3.moveToBackstop();

#endif
#ifdef XI_STEP4_PIE
	_stepper_Pie_4.setConversionFactor(XI_DAIL4_CONVERIONS_FACTOR);
	myInstrument.addIndicator(&_stepper_Pie_4);
	_stepper_Pie_4.moveToBackstop();

#endif
	// run all pie steppers to backstop
	myInstrument.updateNow();

	myInstrument.calibrate();

	//-------------------------------------------------------------------------------------------------------------------
	// for full 360 steppers they have their own calibartion routines
	//-------------------------------------------------------------------------------------------------------------------
#ifdef XI_STEP1_360
	_stepper_360_1.calibrate(XI_HAL1_PIN, true);
	allSteppers.addStepper(_stepper_360_1);
	dataConnection->addElement(XI_STEP1_ITEM, &_stepper_360_1, XI_STEP1_MAX_RANGE, XI_STEP1_MIN_RANGE, true);
#endif
#ifdef XI_STEP2_360
	_stepper_360_2.calibrate(XI_HAL2_PIN, true);
	allSteppers.addStepper(_stepper_360_2);
	dataConnection->addElement(XI_STEP2_ITEM, &_stepper_360_2, XI_STEP2_MAX_RANGE, XI_STEP2_MIN_RANGE, true);
#endif
#ifdef XI_STEP3_360
	_stepper_360_4.calibrate(XI_HAL1_PIN, true);
	allSteppers.addStepper(_stepper_360_3);
	dataConnection->addElement(XI_STEP3_ITEM, &_stepper_360_3, XI_STEP3_MAX_RANGE, XI_STEP3_MIN_RANGE, true);
#endif
#ifdef XI_STEP4_360
	_stepper_360_4.calibrate(XI_HAL1_PIN, true);
	allSteppers.addStepper(_stepper_360_4);
	dataConnection->addElement(XI_STEP4_ITEM, &_stepper_360_4, XI_STEP4_MAX_RANGE, X4_STEP1_MIN_RANGE, true);
#endif

	// run all steppers to start position
	myInstrument.updateNow();
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
#endif

	// set up can bus communication
	myInstrument.initiateCommunication();

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

	// section to control a power/ready flag by a servo
#ifdef USE_PWR_FLAG_SERVO
	_UpdateFlagStatus();
#endif

	//
	// move commands for all steppers
	//
	myInstrument.update();
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
	_stepper_Pie_1.setMinPulseWidth(value);
#endif
#ifdef XI_STEP2_PIE
	_stepper_Pie_2.setMinPulseWidth(value);
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
	_stepper_Pie_2.setDirectionInverse(true);
	Serial.println("Reverse DONE");
}

void handleForward(char* tokens)
{
	_stepper_Pie_2.setDirectionInverse(false);
	Serial.println("Forward DONE");
}

void handleOff(char* tokens)
{
	_stepper_Pie_1.powerOff();
	_stepper_Pie_2.powerOff();
	ledcWrite(ledChannel, 0);
	Serial.println("PowerOFF DONE");
}

void handleOn(char* tokens)
{
	_stepper_Pie_1.powerOn();
	_stepper_Pie_2.powerOn();
	ledcWrite(ledChannel, 255);
	Serial.println("PowerON DONE");
}

#endif