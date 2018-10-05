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
#include "IndicatorStepper.h"
#include "XComm.h"


#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <limits.h>

		
class StepperPie : public IndicatorStepper {
public:

	 /// <summary>
	 /// Create new Pie sized stepper
	 /// </summary>
	 /// <param name="canID">canID of the vlaue to display.</param>
	 /// <param name="rangeMax">max display value.</param>
	 /// <param name="rangeMin">minimal display value.</param>
	 /// <param name="pieSize">size of the pie in degree.</param>
	 /// <param name="reverse">is indicator working in reverse (anti-clockwise) direction.</param>
	 /// <param name="pinStep">pin for step signal.</param>
	 /// <param name="pinDir">pin for directional signal.</param>
	 /// <param name="motorType">Stepper motor type.</param>

	StepperPie(CanasNodDefaultID canID, float rangeMax = 100, float rangeMin = 0, uint8_t pieSize = 120, bool reverse = false, uint8_t pinStep = 3, uint8_t pinDir = 2, StepperMotorType motorType = IndicatorStepper::TYPE_BKA30);

	~StepperPie();

	// calibrate, move to backrest and set to 0
	virtual void calibrate();
	// calibarate and set position of backstop
	virtual void moveToBackstop();
	virtual void calibrate(float backstopPos);
	/// <summary>
	/// set a new value to display
	/// </summary>
	/// <param name="newValue">the new value.</param>
	/// <returns>the current value.</returns>
	virtual int setValue(float newValue);	
	void powerOff();

protected:


private:



};
#endif

