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
#include "InstrumentStepper.h"
#include "XComm.h"


#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <limits.h>

		
class StepperPie : public InstrumentStepper {
public:

// declare constructor baased on accelstepper

	StepperPie(uint8_t canID, float rangeMax = 100, float rangeMin = 0, uint8_t pieSize = 120, bool reverse = false, uint8_t pinStep = 3, uint8_t pinDir = 2, StepperMotorType motorType = InstrumentStepper::TYPE_BKA30) ;

	~StepperPie();

	// calibrate, move to backrest and set to 0
	virtual void calibrate();
	// calibarate and set position of backstop
	virtual void moveToBackstop();
	virtual void calibrate(float backstopPos);
	virtual float setValue(float newValue);	
	void powerOff();

protected:


private:



};
#endif

