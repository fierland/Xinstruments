//===================================================================================================================
// InstrumentStepper.h
//
// author: Frank van Ierland
// version: 0.1
//
// Main configuration file to build instruments configurations 
//
// master stepper class hiding accelstepper class
//
//===================================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef InstrumentStepper_h_
#define InstrumentStepper_h_

#include "mydebug.h"

#include <stdlib.h>
#include <AccelStepper/src/AccelStepper.h>
#include <AccelStepper/src/MultiStepper.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <limits.h>

//define steppermotor types
#define XI_BKA30 0
#define XI_28BYJ 1

// for VID29 and BKA30 steppers using a vid66 driver
// theorecticlay 360*12
#define XI_BKA30_STEPS_CIRCLE 4320
#define XI_BKA30_STEPS_SPEED 500
#define XI_BKA30_STEPS_MAXSPEED 15000
#define XI_BKA30_STEPS_ACCELERATION 2000
#define XI_BKA30_STEPS_MINPULSEWIDTH 500

// for 28BYJ steppers
#define XI_28BYJ_STEPS_CIRCLE 2400
#define XI_28BYJ_STEPS_SPEED 3000
#define XI_28BYJ_STEPS_MAXSPEED 5000
#define XI_28BYJ_STEPS_ACCELERATION 300
#define XI_28BYJ_STEPS_MINPULSEWIDTH 200



class InstrumentStepper : public AccelStepper {
public:
	typedef enum
	{
		TYPE_BKA30 = 0, ///< VID6x or BKA30 type motor
		TYPE_28BYJ = 1 ///< Stepper Driver, 2 driver pins required
	} StepperMotorType;
  
	// declare constructor based on accelstepper

	InstrumentStepper(uint8_t pinStep = 2, uint8_t pinDir = 3, StepperMotorType motorType = InstrumentStepper::TYPE_BKA30);

	~InstrumentStepper();

	// calibrate, move to backrest and set to 0
	virtual void calibrate() =0;
	virtual float setValue(float newValue) =0;
	virtual float setOffPosition(float newValue);
	virtual void powerOn();
	virtual void powerOff();
	/// Sets the inversion for 2, 3 and 4 wire stepper pins	
	/// \param[in] reverseDir True for counter clockwise direction;
	virtual void setDirectionInverse(bool reverseDir = true);


protected:

//    virtual void   step1(long step);

	float 	_currentPos = 0;
	float 	_currentValue = 0;
	float 	_newPos = 0;
	float 	_moveSize = 0;
	float	_rangeMin = 0;
	float	_rangeMax = 10;
	float   _totalValueRange = 10;
	float	_totalRange = 360;
	float	_stepsPerItem = 1;
	float	_stepsPerRotation = 360;
	float	_stepsPerDegree = 1;
	bool	_powerOn = true;
	float	_offValue = 0;
	float	_backstop_pos = 0;
	bool	_isInverse = false;


private:



};
#endif

