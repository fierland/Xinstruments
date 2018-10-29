// IndicatorStepper.cpp
//
// author: Frank van Ierland
// version: 0.1
//
// main stepper motor class wrapper for accelstepper class
//
//------------------------------------------------------------------------------------
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#include "IndicatorStepper.h"
#include <AccelStepper.h>
//#include <MultiStepper.h>
#define NO_DEBUG 1

IndicatorStepper::IndicatorStepper(CanasNodDefaultID canID, uint8_t pinStep, uint8_t pinDir, StepperMotorType motorType) : GenericIndicator(canID)
{
	DPRINTLN("Start Instrument Stepper constructor");

	_myStepper = new AccelStepper(AccelStepper::DRIVER, pinStep, pinDir, 0, 0, true);

	_type = INDICATOR_STEPPER;
	if (motorType == IndicatorStepper::TYPE_BKA30)
	{
	  DPRINTLN(">TYPE_BKA30");
		_stepsPerRotation = XI_BKA30_STEPS_CIRCLE;
		_myStepper->setMaxSpeed(XI_BKA30_STEPS_MAXSPEED);
		_myStepper->setAcceleration(XI_BKA30_STEPS_ACCELERATION);
		_myStepper->setSpeed(XI_BKA30_STEPS_SPEED);
		_myStepper->setMinPulseWidth(XI_BKA30_STEPS_MINPULSEWIDTH);
	}
	if (motorType == IndicatorStepper::TYPE_28BYJ)
	{
    DPRINTLN(">TYPE_28BYJ");
		_stepsPerRotation = XI_28BYJ_STEPS_CIRCLE;
		_myStepper->setMaxSpeed(XI_28BYJ_STEPS_MAXSPEED);
		_myStepper->setAcceleration(XI_28BYJ_STEPS_ACCELERATION);
		_myStepper->setSpeed(XI_28BYJ_STEPS_SPEED);
		_myStepper->setMinPulseWidth(XI_28BYJ_STEPS_MINPULSEWIDTH);
	}

	_stepsPerDegree = _stepsPerRotation / 360;

	DPRINTLN("End IndicatorStepper constructor");
}

IndicatorStepper::~IndicatorStepper()
{}

void IndicatorStepper::powerOn()
{
	DPRINTLN("Start StepperPie powerOn");
	_powerOn = true;
	setValue(_currentValue);
	//runToPosition();
	DPRINTLN("End StepperPie powerOn");
}
//
// power off actions;
//
void IndicatorStepper::powerOff()
{
	DPRINTLN("Start StepperPie powerOff");

	_currentPos = (_offValue - _rangeMin) / _totalRange * _stepsPerItem;
	_myStepper->moveTo(_offValue);
	//runToPosition();
	_powerOn = false;

	DPRINTLN("End StepperPie powerOff");
}

float IndicatorStepper::setOffPosition(float newValue)
{
	float oldValue = _offValue;

	DPRINTLN("Start IndicatorStepper setOffPosition");

	// check if in range and dail is on
	if (newValue > _rangeMax || newValue < _rangeMin)
	{
		DPRINT("ERR: OutOfRange (");
		DPRINT(_rangeMin);
		DPRINT("->");
		DPRINT(_rangeMax);
		DPRINT("):");
		DPRINTLN(newValue);

		return -1;
	}

	_offValue = newValue;

	DPRINTLN("End IndicatorStepper setOffPosition");
	return (oldValue);
}

void IndicatorStepper::setDirectionInverse(bool reverseDir)
{
	_myStepper->setPinsInverted(reverseDir);
	_isInverse = reverseDir;
}

void IndicatorStepper::setMaxSpeed(float speed)
{
	_myStepper->setMaxSpeed(speed);
}

AccelStepper * IndicatorStepper::stepper()
{
	return _myStepper;
}

//
// override step i function for VID660x stepper drivers
/*
void IndicatorStepper::step1(long step)
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
