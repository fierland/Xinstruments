//===================================================================================================================
// IndicatorStepper.h
//
// author: Frank van Ierland
// version: 0.1
//
//
// master stepper class hiding accelstepper class
//
//===================================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef IndicatorStepper_h_
#define IndicatorStepper_h_

#include "mydebug.h"
#include <stdlib.h>
#include "GenericIndicator.h"
#include <AccelStepper.h>

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
#define XI_BKA30_STEPS_SPEED 2000
#define XI_BKA30_STEPS_MAXSPEED 10000
#define XI_BKA30_STEPS_ACCELERATION 5000
#define XI_BKA30_STEPS_MINPULSEWIDTH 400

// for 28BYJ steppers
#define XI_28BYJ_STEPS_CIRCLE 2400
#define XI_28BYJ_STEPS_SPEED 3000
#define XI_28BYJ_STEPS_MAXSPEED 5000
#define XI_28BYJ_STEPS_ACCELERATION 300
#define XI_28BYJ_STEPS_MINPULSEWIDTH 200

class IndicatorStepper : public GenericIndicator {
public:
	typedef enum {
		TYPE_BKA30 = 0, ///< VID6x or BKA30 type motor
		TYPE_28BYJ = 1 ///< Stepper Driver, 2 driver pins required
	} StepperMotorType;

	/// declare constructor based on accelstepper
	///
	/// Create new generic stepper
	/// @param [in]  canID		canID of the vlaue to display
	/// @param [in]  pinStep	    pin for step signal
	/// @param [in]  pinDir		pin for direction signal
	/// @param [in]  motorType	stepper motor type to use
	///
	IndicatorStepper(CanasNodDefaultID canID, uint8_t pinStep = 2, uint8_t pinDir = 3, StepperMotorType motorType = IndicatorStepper::TYPE_BKA30);

	~IndicatorStepper();

	/**
	 * Set the default off position of the instrument if not _rangeMin.
	 * \param [in]	newValue  new value for the off position
	 * \return      old position
	 */
	virtual float setOffPosition(float newValue);

	virtual void powerOn();
	virtual void powerOff();

	/// Sets the inversion for 2, 3 and 4 wire stepper pins
	/// \param[in] reverseDir True for counter clockwise direction;CanasNodDefaultIDCanasNodDefaultID
	virtual void setDirectionInverse(bool reverseDir = true);

	/// Sets the maximum permitted speed. The run() function will accelerate
	/// up to the speed set by this function.
	/// Caution: the maximum speed achievable depends on your processor and clock speed.
	/// \param[in] speed The desired maximum speed in steps per second. Must
	/// be > 0. Caution: Speeds that exceed the maximum speed supported by the processor may
	/// Result in non-linear accelerations and decelerations.
	void    setMaxSpeed(float speed);

	AccelStepper* stepper();

protected:

	//    virtual void   step1(long step);

	float 	_currentPos = 0;
	float 	_newPos = 0;
	float 	_moveSize = 0;
	float   _totalValueRange = 10;
	float	_totalRange = 360;
	float	_stepsPerItem = 1;
	float	_stepsPerRotation = 360;
	float	_stepsPerDegree = 1;
	float	_offValue = 0;
	float	_backstop_pos = 0;
	bool	_isInverse = false;
	AccelStepper* _myStepper = NULL;

private:
};
#endif
