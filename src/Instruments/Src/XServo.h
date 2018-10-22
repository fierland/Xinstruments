// XServo.h
//
// author: Frank van Ierland
// version: 0.1
//
// stepper interface for a ranged servo based on servo lib
//
//------------------------------------------------------------------------------------
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef XServo_h_
#define XServo_h_

#include "src\XLibs\src\mydebug.h"

#include <stdlib.h>
// include the Servo library

#ifdef ARDUINO_ARCH_ESP32
#include <ESP32Servo/src/ESP32_Servo.h>
#else
#include <Servo.h>
#endif

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <limits.h>

class XServo {
public:

	// declare constructor based on accelstepper
	XServo(float rangeMin = 0, float rangeMax = 300, uint8_t pieMin = 0, uint8_t pieMax = 180, uint8_t pin1 = 2);

	~XServo();

	// ...

	virtual void moveTo(float absolute);
	virtual void moveMin();
	virtual void moveMax();

	virtual void powerOn();

	virtual void powerOff();

protected:
	float 	_currentPos = 0;
	float 	_newPos = 0;
	float 	_moveSize = 0;
	float	_rangeMin = 0;
	float	_rangeMax = 360;
	float	_totalRange = 360;
	float	_stepsPerItem = 1;
	int 	_pieMin = 0;
	int		_pieMax = 0;
	bool	_powerOn = true;
	int		_pinPWM = 2;

private:
	void 	moveTo0(void);
	Servo	*_myServo = nullptr;
};
#endif
