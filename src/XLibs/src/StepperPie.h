// StepperPie.h
//
// author: Frank van Ierland
// version: 0.1
//
// stepper interface for a ranged stepper based on accelstepper lib
//
//------------------------------------------------------------------------------------
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef StepperPie_h_
#define StepperPie_h_

#include "mydebug.h"

#include <stdlib.h>
#include <AccelStepper.h>


#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <limits.h>

			
		
class StepperPie : public AccelStepper {
public:

// declare constructor baased on accelstepper
	StepperPie( float rangeMax = 100, float rangeMin = 0, uint8_t pieSize = 120, uint16_t stepsCircle = 900, uint8_t interface = AccelStepper::DRIVER, uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5, bool enable = true ) ;

	~StepperPie();

	// ...
	virtual void calibrate();
	
	virtual void runToNewPosition(float newPos);	
	virtual float setValue(float newValue);	
	virtual float setOffPosition(float newValue);
	virtual void powerOn();
	virtual void powerOff();


protected:
	float 	_currentPos = 0;
	float 	_currentValue = 0;
	float 	_newPos = 0;
	float 	_moveSize = 0;
	float	_rangeMin = 0;
	float	_rangeMax = 360;
	float	_totalRange = 360;
	float	_stepsPerItem = 1;
	float	_stepsPerRotation = 360;
	bool	_powerOn = true;
	float	_offValue =0;


private:



};
#endif

