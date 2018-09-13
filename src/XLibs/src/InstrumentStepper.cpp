// InstrumentStepper.cpp
//
// author: Frank van Ierland
// version: 0.1
//
// main stepper motor class wrapper for accelstepper class
//
//------------------------------------------------------------------------------------
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#include "InstrumentStepper.h"


InstrumentStepper::InstrumentStepper(uint8_t pinStep, uint8_t pinDir,StepperMotorType motorType) : AccelStepper(AccelStepper::DRIVER,  pinStep, pinDir, 0, 0, true) {

	DPRINTLN("Start Instrument Stepper constructor");
	
	if (motorType == InstrumentStepper::TYPE_BKA30)
	{
		_stepsPerRotation = XI_BKA30_STEPS_CIRCLE;
		setMaxSpeed(XI_BKA30_STEPS_MAXSPEED);
		setAcceleration(XI_BKA30_STEPS_ACCELERATION);
		setSpeed(XI_BKA30_STEPS_SPEED);
		setMinPulseWidth(XI_BKA30_STEPS_MINPULSEWIDTH);
	}
	if (motorType == InstrumentStepper::TYPE_28BYJ)
	{
		_stepsPerRotation = XI_28BYJ_STEPS_CIRCLE;
		setMaxSpeed(XI_28BYJ_STEPS_MAXSPEED);
		setAcceleration(XI_28BYJ_STEPS_ACCELERATION);
		setSpeed(XI_28BYJ_STEPS_SPEED);
		setMinPulseWidth(XI_28BYJ_STEPS_MINPULSEWIDTH);
	}
	_stepsPerDegree = _stepsPerRotation / 360;

	DPRINTLN("End InstrumentStepper constructor");
}


InstrumentStepper::~InstrumentStepper()
{
}

void InstrumentStepper::powerOn() {

	DPRINTLN("Start StepperPie powerOn");
	_powerOn = true;
	setValue(_currentValue);
	//runToPosition();
	DPRINTLN("End StepperPie powerOn");
}
//
// power off actions;
//
void InstrumentStepper::powerOff() {

	DPRINTLN("Start StepperPie powerOff");

	_currentPos = (_offValue - _rangeMin) / _totalRange * _stepsPerItem;
	moveTo(_currentPos);
	//runToPosition();
	_powerOn = false;

	DPRINTLN("End StepperPie powerOff");

}

float InstrumentStepper::setOffPosition(float newValue) {

	float oldValue = _offValue;

	DPRINTLN("Start InstrumentStepper setOffPosition");

	// check if in range and dail is on
	if (newValue > _rangeMax || newValue < _rangeMin) {
		DPRINT("ERR: OutOfRange (");
		DPRINT(_rangeMin);
		DPRINT("->");
		DPRINT(_rangeMax);
		DPRINT("):");
		DPRINTLN(newValue);

		return -1;
	}

	_offValue = newValue;


	DPRINTLN("End InstrumentStepper setOffPosition");
	return (oldValue);

}

void InstrumentStepper::setDirectionInverse(bool reverseDir)
{
	setPinsInverted(reverseDir);
	_isInverse = reverseDir;
}

//
// override step i function for VID660x stepper drivers
/*
void InstrumentStepper::step1(long step)
{
	    (void)(step); // Unused
		bool stepperDirection;
//
// changed to work corectly with VID66 stepper drivers
//
	//stepperDirection	= (_pinInverted[0])?!_direction:_direction;
	stepperDirection= _direction;
	setOutputPins(stepperDirection ? 0b01 : 0b00); 
	setOutputPins(stepperDirection ? 0b11 : 0b10); 
	
	//digitalWrite(_pin[1], stepperDirection?HIGH:LOW);
	//digitalWrite(_pin[0], HIGH);
	delayMicroseconds(_minPulseWidth);
	//digitalWrite(_pin[0], LOW); 
	setOutputPins(stepperDirection ? 0b01 : 0b00); 
};
*/