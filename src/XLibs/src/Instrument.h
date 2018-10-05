//=================================================================================================
// Instrument.h
//
// Generic class for the whole instrument will contain all indicators and other stuff
//
//=================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#pragma once

#include "mydebug.h"
#include "GenericIndicator.h"
#include "Xinstruments.h"

class Instrument
{
public:
	Instrument(char* instrumentCode, int ledPin, int maxIndicators= XI_MAX_INDICATORS);
	~Instrument();

	int initiateCommunucation();
	int addIndicator(GenericIndicator* newIndicator);
	virtual void calibrate();
	virtual void toZero();
	/// update value for digital dails , do a step for stepper motors
	virtual void update();
	/// update value for digital dails and stepper dails wait for stepper to get to position
	virtual void updateNow();
	virtual void powerState(boolean setOn);	
	virtual void lightState(boolean setOn);

private:
	struct indicatorControl
	{
		GenericIndicator*	indicator = NULL;

	};

	indicatorControl	myIndicators[10];

	char			_instrumentCode[8];
	bool			_powerOn = false;
	bool			_LightsOn = false;
	int				_numIndicators = 0;
	int				_ledPin;
	int				_maxIndicators = 10;
	Xcomm			*dataConnection = NULL; 
	MultiStepper	allSteppers;

	// data to control the leds
	int _ledFreq = 5000;
	int _ledChannel = 0;
	int _resolution = 8;
};

