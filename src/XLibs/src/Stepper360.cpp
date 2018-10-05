//===================================================================================================================
/// <summary>
// Stepper360.cpp
//
// author: Frank van Ierland
// version: 0.1
//
// stepper interface for a 360 degree stepper based on accelstepper lib
/// <summary>
//
//===================================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#include "stepper360.h"
#include "mydebug.h"


//-------------------------------------------------------------------------------------------------
// constructor for segmented dails
//-------------------------------------------------------------------------------------------------
Stepper360::Stepper360(CanasNodDefaultID canID, float rangeMax,uint16_t stepsCircle, uint8_t stepType, uint8_t pinStep, uint8_t pinDir, StepperMotorType motorType) : IndicatorStepper(canID, pinStep, pinDir, motorType) {

	DPRINTLN("Start Stepper360 wedge constructor");

	// set internal params
	_rangeMin = 0;
	_rangeMax = rangeMax;
	_totalRange = rangeMax;
	_stepsPerRotation = stepsCircle;

	// define step size for later calculations
	/// first define part of circle used
	_stepsPerItem = _stepsPerRotation / _totalRange;


	DPRINT("New 360 dail (");
	DPRINT(_rangeMin);
	DPRINT("->");
	DPRINT(_rangeMax);
	DPRINT(") stepsize:");
	DPRINTLN(_stepsPerItem);

	// set some defaults
	_currentPos = 0;
	_newPos = 0;
	_moveSize = 0;
	

	DPRINTLN("End Stepper360 wedge constructor");
}


//-------------------------------------------------------------------------------------------------
// deconstructor
//-------------------------------------------------------------------------------------------------
Stepper360::~Stepper360(){
	powerOff();
}



//-------------------------------------------------------------------------------------------------
// set the dail to new value_comp
//-------------------------------------------------------------------------------------------------
int Stepper360::setValue(float newPos){	
   int dir = 1;
   int moveSteps = 0;
   long stepsPos;
   long diffPos;
   long actualPosition;
   
	actualPosition = currentPosition()/_stepsPerItem;

	
    DPRINT("Start Stepper360 moveTo From:");
	DPRINT(_currentPos);
	DPRINT("-");
	DPRINT(actualPosition);
	DPRINT("-To:");
	DPRINTLN(newPos);
	
	// check if in range
	if (!_powerOn || newPos > _rangeMax || newPos < _rangeMin)	{
		DPRINT("ERR: OutOfRange (");
		DPRINT(_rangeMin);
		DPRINT("->");
		DPRINT(_rangeMax);
		DPRINT("):");
		DPRINTLN(newPos);

		return ;
	}		
	// move to actual pos to enable stpping in loop
	_currentPos = actualPosition;
	_newPos = newPos ;
	_moveSize = newPos - _currentPos;
	
		DPRINT(": moving :");
		DPRINTLN(_moveSize);

	
	// now see how we get there the fastes way for continues dails
	// if move more than 50% of range change direction to take shortest way 
	DPRINT("Adapting for Continues dails:");
	if (abs(_moveSize) > (_totalRange / 2)) {
		// make move shortest route
		DPRINT("Adjust to shortest route:");
		if (_moveSize > 0) {
			_moveSize -= (_totalRange );
		}
		else {
			_moveSize += (_totalRange);
		};
		DPRINT(":New MoveSize:");
		DPRINTLN(_moveSize);
	};
	// pointer moves from low thru 0 to high
	if ((_moveSize < 0) && ((_moveSize + _currentPos) < 0)) {
		DPRINT("Reset position + full rotation:");
		DPRINT(":New startpos:");
		DPRINTLN(_currentPos + _totalRange);
		setCurrentPosition((_currentPos + _totalRange) * _stepsPerItem);
	}
	// pointer moves via max thru 0
	if ((_moveSize > 0) && ((_moveSize + _currentPos) >= _rangeMax)) {
		DPRINT("Reset position - full rotation:");
		DPRINT(":New startpos:");
		DPRINTLN(_currentPos - _totalRange);
		setCurrentPosition((_currentPos - _totalRange) * _stepsPerItem);
	}
	DPRINTLN(":DONE");

	DPRINT("Move  from:");
	DPRINT(_currentPos);

	DPRINT(":Move  to new pos:");
	DPRINTLN(newPos);
	moveTo(newPos * _stepsPerItem);   
	_currentPos = newPos ;

	DPRINTLN("End Stepper360 moveTo");
};


//-------------------------------------------------------------------------------------------------
// callback funtion for hal sensor
//-------------------------------------------------------------------------------------------------
void Stepper360::halCallback(void){
	//AccelStepper::stop();
	_atZero =true;
	DPRINTLN("Interup on HAL found");
};

//-------------------------------------------------------------------------------------------------
// set dail to 0 position
//-------------------------------------------------------------------------------------------------
int Stepper360::calibrate(uint8_t pinHal,bool retainPos) {
	long startPos = currentPosition();
	long curSpeed = speed();
	setSpeed(100);

	DPRINTLN("Start Stepper360 calibrate");
	// TODO: code to set zero pos using sensor or by moving to backstop

	// set interupt to hal sensor
	attachInterrupt(pinHal, &halCallback, RISING);
	_atZero = false;
	//runSpeedToPosition();
	//move(-1);
	move(-(_totalRange*_stepsPerItem+10));
	while( !_atZero&&run()){
#if defined(ARDUINO_ARCH_ESP8266)
		ESP.wdtFeed();
#endif
		//move(-1);
		//run() ;
	};
	
	stop();
	detachInterrupt(pinHal);
	long newPos = currentPosition();
	_currentPos=0;
	setCurrentPosition(0);
	if (retainPos)	{
		DPRINTLN("Moving to old postition");
		runToNewPosition(startPos - newPos);
		_currentPos = (startPos - newPos)/_stepsPerItem;
	};

	setSpeed(curSpeed);
	DPRINTLN("End Stepper360 calibrate");

};
//-------------------------------------------------------------------------------------------------
// power on actions;
//-------------------------------------------------------------------------------------------------
void Stepper360::powerOn(){
	
	DPRINTLN("Start Stepper360 powerOn");
	_powerOn=true;
	DPRINTLN("End Stepper360 powerOn");
}
//-------------------------------------------------------------------------------------------------
// power off actions;
//-------------------------------------------------------------------------------------------------
void Stepper360::powerOff(){
	
	DPRINTLN("Start Stepper360 powerOff");

	_powerOn = false;
	
	DPRINTLN("End Stepper360 powerOff");

}
