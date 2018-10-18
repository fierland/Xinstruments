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

//-------------------------------------------------------------------------------------------------
//	constructor
//-------------------------------------------------------------------------------------------------
StepperPie::StepperPie(CanasNodDefaultID canID, float rangeMax, float rangeMin, uint8_t pieSize, bool reverse, uint8_t pinStep, uint8_t pinDir, StepperMotorType motorType)
	: IndicatorStepper(canID, pinStep, pinDir, motorType)
{
	DPRINTLN("Start StepperPie constructor");

	// set internal params
	_rangeMin = rangeMin;
	_rangeMax = rangeMax;
	_totalRange = pieSize;

	// define step size for later calculations
	/// first define part of circle used

	// TODO: fix for negative rangemin
	if (rangeMin < 0) {
		_totalValueRange = rangeMax + rangeMin;
	}
	else {
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

	DPRINTLN("End StepperPie constructor");
}
//-------------------------------------------------------------------------------------------------
// deconstructor
//-------------------------------------------------------------------------------------------------//
StepperPie::~StepperPie() {
	powerOff();
}
//-------------------------------------------------------------------------------------------------
// set dail to 0 position
//-------------------------------------------------------------------------------------------------

void StepperPie::calibrate() {
	DPRINTLN("Start StepperPie calibrate");

	//move(- _stepsPerRotation);
	//runToPosition();
	_currentPos = 0;
	setCurrentPosition(0);
	_newPos = 0;

	DPRINTLN("End StepperPie calibrate");
};
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void StepperPie::moveToBackstop()
{
	move(-(_stepsPerRotation * 360 / _totalRange)*1.1);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void StepperPie::calibrate(float backstopPos) {
	DPRINTLN(backstopPos);
	//calibrate();
	setCurrentPosition(backstopPos);
	moveTo(0);
	//runToPosition();
};
//-------------------------------------------------------------------------------------------------
// power off actions;
//-------------------------------------------------------------------------------------------------

void StepperPie::powerOff() {
	DPRINTLN("Start StepperPie powerOff");

	_currentPos = (_offValue - _rangeMin) / _totalRange * _stepsPerItem;
	moveTo(_currentPos);
	//runToPosition();
	_powerOn = false;

	DPRINTLN("End StepperPie powerOff");
}
//-------------------------------------------------------------------------------------------------
// set new value;
//-------------------------------------------------------------------------------------------------

int StepperPie::setValue(float newValue) {
	float tempValue;
	float oldValue = _currentValue;
	long newPosition;

	DPRINTLN("Start StepperPie setValue");

	// check if in range and dail is on
	if (newValue > _rangeMax || newValue < _rangeMin) {
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

	moveTo(newPosition);

	DPRINTLN("End StepperPie setValue");

	return 0;
}