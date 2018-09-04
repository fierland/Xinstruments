// VidStepper.h
//
// author: Frank van Ierland
// version: 0.1
//
// stepper interface for VID66-08/06 type stepper driver with VID type instrument steppers.
//
//------------------------------------------------------------------------------------
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef VIDStepper_h
#define VIDStepper_h

#include "mydebug.h"

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

// These defs cause trouble on some versions of Arduino
#undef round

class VidStepper
{
public:
	/// create a new VidStepper object
	/// \param[in] pinDir The direction pin for the controler. 
	/// \param[in] pinStep The step pin for the controler. 
	/// \param[in] is360 If the indicator has full 360 degree range. 
	/// \param[in] isReverse If the indicator is anti clockwise. 
	VidStepper(uint8_t pinDir, uint8_t pinStep, bool is360 = false, bool isReverse = false );
	~VidStepper();

	virtual int   setStepsPerRotation(int steps);
	virtual bool  set360(bool is360);
	virtual bool  setReverse(bool isReverse);
	virtual void  runToPosition();
	virtual void  calibrate();
	virtual void  calibrate(float backstopPos);

	// set the range for the indicator in degree of the full circle for non full 360 indicators
	virtual float setRange(float rangeDegree = 120);

	virtual int   setRangeValues(float rangeMax, float rangeMin = 0);


	virtual float move(float newPos = 0);
	// move to a new position on the stepper motor
	virtual float moveTo(float newPos = 0);
	// set the value the indicator should show
	virtual int setValue(float newValue = 0);

	/// Sets the minimum pulse width allowed by the stepper driver. The minimum pulse width for the VID60-08/06 controlers is
	/// 450 nanoseconds. = 0,45 miliseconds 
	/// will usually result in 20 microseconds or so.
	/// \param[in] minWidth The minimum pulse width in microseconds. 
	virtual void    setMinPulseWidth(unsigned int minWidth);
	
	virtual int run();

	virtual void powerOn();

	virtual void powerOff();

protected:

	virtual void _doStep(bool moveFroward = true);
	
	bool	_is360 = false;				// has the instrument a full 360 degree range and no stops 
	bool	_isReverse = false;			// inverse direction of steppr, normal direction is clockwise
	int		_pinStep;					// pin used for step signal
	int		_pinDir;					// pin used for directional signal
	int		_pinSensor = -1;			// pin used for zero sensor
	int		_stepsPerRotation = 4320;	// steps needed to do a full rotation
	int		_stepsForRange = 4320;		// steps in the range used for the indicator
	float   _rangeInDegree = 360;		// part of circle used for the indicator in degrees 
	float	_currentPosition = 0;		// position in steps
	float	_targetPosition = 0;		// position in steps
	float	_currentValue = 0;			// curent vlaue the dail should be showing
	float	_valueMin = 0;				// minimun value on the indicator
	float	_valueMax = 360;			// max value on the indicator
	float	_rangeMin = 0;				// minimun stepper position
	float	_rangeMax = 360;			// max stepper position on the indicator
	float	_totalValueRange = 360;		// difference between min and max value
	float	_stepsPerDegree = 1;		// steps to make for 1 unit on the indicator
	bool	_isPowered = false;			// virtual power switch for instrument is power is 0 go to zero value
	int		_pulseWidth = 175;			// pulse width for stepper controler in miliseconds is 450 nano seconds minimum / 175 was optimum in testing.
	int		_pulseSetupWidth = 100;		// wait time between setting dir and pulse  = 100 nanoS minimum
	int		_speed = 10;				// max steps per run call
};
#endif
