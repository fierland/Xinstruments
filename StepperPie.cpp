//===================================================================================================================
/// <summary>
// StepperPie.h
//
// author: Frank van Ierland
// version: 0.1
//
// stepper interface for a ranged stepper based on accelstepper lib
/// <summary>
//
//===================================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland
#include "StepperPie.h"
#include "mydebug.h"
#include <AccelStepper.h>

//-------------------------------------------------------------------------------------------------
//	constructor
//-------------------------------------------------------------------------------------------------
StepperPie::StepperPie(CanasNodDefaultID canID, float rangeMax, float rangeMin, uint8_t pieSize, bool reverse, uint8_t pinStep, uint8_t pinDir, StepperMotorType motorType)
	: IndicatorStepper(canID, pinStep, pinDir, motorType)
{
	DPRINTINFO("START");

	// set internal params
	_rangeMin = rangeMin;
	_rangeMax = rangeMax;
	_totalRange = pieSize;

	// define step size for later calculations
	/// first define part of circle used

	// TODO: fix for negative rangemin
	if (rangeMin < 0)
	{
		_totalValueRange = rangeMax + rangeMin;
	}
	else
	{
		_totalValueRange = rangeMax - rangeMin;
	}

	setDirectionInverse(reverse);

	DPRINT("Min:");
	DPRINT(_rangeMin);
	DPRINT(":Max:");
	DPRINT(_rangeMax);
	DPRINT(":Total range:");
	DPRINTLN(_totalValueRange);

	_stepsPerItem = (_stepsPerRotation * 360 / pieSize) / _totalRange;

	DPRINT("New wedge dail (");
	DPRINT(_rangeMin);
	DPRINT("->");
	DPRINT(_rangeMax);
	DPRINT(" :Range:");
	DPRINT(_totalRange);
	DPRINT(") stepsize:");
	DPRINT(_stepsPerItem);
	DPRINT(":stepsCircle:");
	DPRINTLN(_stepsPerRotation);

	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------
// deconstructor
//-------------------------------------------------------------------------------------------------//
StepperPie::~StepperPie()
{
	DPRINTINFO("START");
	powerOff();
	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------
// set dail to 0 position
//-------------------------------------------------------------------------------------------------

void StepperPie::calibrate()
{
	DPRINTINFO("START");

	//move(- _stepsPerRotation);
	//runToPosition();
	_currentPos = 0;
	_myStepper->setCurrentPosition(0);
	_newPos = 0;
	DPRINTINFO("STOP");
};
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void StepperPie::moveToBackstop()
{
	DPRINTINFO("START");
  long toMove = -(_stepsPerRotation / 360 * _totalRange)*1.2;
  DPRINT("Moving:");
  DPRINTLN(toMove);
	_myStepper->move(toMove);
	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void StepperPie::calibrate(float backstopPos)
{
	DPRINTINFO("START");
  DPRINT("backstopPos:");
	DPRINTLN(backstopPos);
	//calibrate();
	_myStepper->setCurrentPosition(backstopPos);
	_myStepper->moveTo(0);
	//runToPosition();
	DPRINTINFO("STOP");
};
//-------------------------------------------------------------------------------------------------
// power off actions;
//-------------------------------------------------------------------------------------------------

void StepperPie::powerOff()
{
	DPRINTINFO("START");

	_currentPos = (_offValue - _rangeMin) / _totalRange * _stepsPerItem;
	_myStepper->moveTo(_currentPos);
	//runToPosition();
	_powerOn = false;

	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------
// set new value;
//-------------------------------------------------------------------------------------------------

int StepperPie::setValue(float newValue)
{
	float tempValue;
	float oldValue = _currentValue;
	long newPosition;

	DPRINTINFO("START");

	// check if in range and dail is on
	if (newValue > _rangeMax || newValue < _rangeMin)
	{
		DPRINT("ERR: OutOfRange (");
		DPRINT(_rangeMin);
		DPRINT("->");
		DPRINT(_rangeMax);
		DPRINT("):");
		DPRINTLN(newValue);

		return 1;
	}

	_currentValue = newValue;

	tempValue = newValue - _rangeMin;

	DPRINT("step1:");
	DPRINT(newValue);
	DPRINT("-");
	DPRINT(_rangeMin);
	DPRINT("=");
	DPRINTLN(tempValue);

	tempValue /= _totalValueRange;

	DPRINT("step2: divide by");
	DPRINT(_totalValueRange);
	DPRINT("=");
	DPRINTLN(tempValue);

	DPRINT("step3: *");
	DPRINT(_totalRange);
	DPRINT("*");
	DPRINTLN(_stepsPerDegree);

	newPosition = round(tempValue * _totalRange * _stepsPerDegree);

	DPRINT("New position:");
	DPRINTLN(newPosition);

	_myStepper->moveTo(newPosition);

	DPRINTINFO("STOP");

	return 0;
}
