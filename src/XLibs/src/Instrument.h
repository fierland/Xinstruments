//=================================================================================================
// Instrument.h
//
// Generic class for the whole instrument will contain all indicators and other stuff
//
//=================================================================================================
#pragma once

#include "mydebug.h"
#include "GenericIndicator.h"
#include "Xinstruments.h"


class Instrument
{
public:
	Instrument();
	~Instrument();

	int initiateCommunucation();
	int addIndicator(GenericIndicator* newIndicator);
	virtual void calibrate();
	virtual void toZero();
	virtual void powerState(boolean setOn);	
	virtual void lightState(boolean setOn);

private:
	struct indicatorControl
	{
		GenericIndicator*	indicator = NULL;

	};

	indicatorControl	myIndicators[XI_INDICATORS_USED];

	bool	_powerOn = false;
	bool	_LightsOn = false;
	int		numIndicators = 0;
};

