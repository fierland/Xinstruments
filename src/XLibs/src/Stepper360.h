// Stepper360.h
//
// author: Frank van Ierland
// version: 0.1
//
// stepper interface for a ranged 360 gr stepper based on accelstepper lib
//
//------------------------------------------------------------------------------------
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef Stepper360_h_
#define Stepper360_h_

#include <mydebug.h>

#include <stdlib.h>
#include <AccelStepper.h>


#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <limits.h>

			
		
class Stepper360 : public AccelStepper {
public:
	// bool powerState = false;

	// declare constructor baased on accelstepper
	Stepper360( float rangeMax = 360, uint8_t pinZero = 6, uint16_t stepsCircle = 900, uint8_t interface = AccelStepper::FULL4WIRE, uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5, bool enable = true ) ;
	
	~Stepper360();

	// start connection
	virtual int calibrate(uint8_t pinHal = -1,bool retainPosition = false);

	virtual void runToNewPosition(float newPos);
	
	virtual void moveTo(float absolute);
	
	virtual void powerOn();
	
	virtual void powerOff();
	
	// poll for new data need to be in loop()
	//	void halCallback(void);

protected:
	float 	_currentPos = 0;
	float 	_newPos = 0;
	float 	_moveSize = 0;
	float	_rangeMin = 0;
	float	_rangeMax = 360;
	float	_totalRange = 360;
	float	_stepsPerItem = 1;
	float	_stepsPerRotation = 360;
	bool	_isContinious = true;
	bool	_powerOn = true;					


private:
	//bool _atZero = false;




};
#endif

