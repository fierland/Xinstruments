//===================================================================================================================
// Stepper360.h
//
// author: Frank van Ierland
// version: 0.1
//
// stepper interface for a ranged 360 gr stepper based on accelstepper lib
//
//===================================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef Stepper360_h_
#define Stepper360_h_

#include "src\XLibs\src\mydebug.h"

#include <stdlib.h>
#include "IndicatorStepper.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <limits.h>

// define type of control in my project all steppers will now be controled by a VID66-0x controler
#define _USE_STEP_DRIVER

class Stepper360 : public IndicatorStepper {
public:
	// bool powerState = false;

	Stepper360(CanasNodDefaultID canID, float rangeMax = 360, uint16_t stepsCircle = 900, uint8_t stepType = AccelStepper::DRIVER, uint8_t pin1 = 2, uint8_t pin2 = 3, StepperMotorType motorType = IndicatorStepper::TYPE_BKA30);
	// declare constructor baased on accelstepper
	~Stepper360();

	// start connection
	virtual int calibrate(uint8_t pinHal = -1, bool retainPosition = false);

	virtual void powerOn();

	virtual void powerOff();
	virtual int setValue(float newValue);

	// poll for new data need to be in loop()
	//	void halCallback(void);
	static void halCallback(void);

protected:
	bool	_isContinious = true;
private:
	static bool _atZero;
};
#endif
