// XPdefs.h
//
// author: Frank van Ierland
// version: 0.1
//
// stepper interface for a ranged stepper based on accelstepper lib
//
//------------------------------------------------------------------------------------
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef RangeStepper_h_
#define RangeStepper_h_

#include <stdlib.h>
#include <QList.h>
#include <mydebug.h>
#include <AccelStepper.h>


#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <limits.h>

			
		
class RangeStepper 
{
public:
	bool _powerState = false;
	// typedef to declare the type of stepper motor to use
	typedef enum {
			typeVID27	= 900,
			type28BYJ	= 4096
	} MotorType;
			
 
	// declare same 
	RangeStepper (uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5, float RangeMax, float RangeMin = 0, uint8_t stepperType = RangeStepper::typeVID27);
	
	RangeStepper (uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5, float RangeMax, float RangeMin = 0, float wedgeSize =90 , uint8_t stepperType = RangeStepper::typeVID27);
	
	
	// start connection
	int calibrate() ;
	void moveTo(long absolute);
				// poll for new data need to be in loop()
	//int setZero();
	void powerState(bool isOn) ;
	
	//int registerDataRef(int iFreq, const char *sCode );
	//int registerDataRef(int iFreq, const char *sCode, void (*callbackFunc)(float par) = NULL );
	

protected:
	float 	_currentPos = 0 ;
	float 	_newPos = 0 ;
	float 	_moveSize = 0 ;
	float	_rangeMin = 0;
	float	_rangeMax = 360;
	float	_totalRange=360;
	float	_stepsPerItem;
	float 	_stepsPerRotation;	
	bool	_isContinious = true;
	bool	_isOn		 = false;
	
private:
	void constructAll(int8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5, uint8_t stepperType );

#endif

