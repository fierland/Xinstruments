//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================

//=================================================================================================
// Instrument.h
//
// Generic class for the whole instrument will contain all indicators and other stuff
//
//=================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland
#pragma once

#ifndef _INSTRUMENT_H
#define _INSTRUMENT_H

#include "InstrumentMsg.h"
#include "GenericIndicator.h"
#include "CANaero.h"
#include <MultiStepper.h>
#include "mydebug.h"


constexpr auto XI_Max_Indicators = 10;

class Instrument {
public:

	Instrument(char* instrumentCode, uint8_t nodeId, uint8_t srv_channel, int ledPin, int maxIndicators = XI_Max_Indicators);
	~Instrument();

	int initiateCommunication();
	int addIndicator(GenericIndicator& newIndicator);
	virtual void calibrate();
	virtual void toZero();
	/// update value for digital dails , do a step for stepper motors
	virtual void update();
	/// update value for digital dails and stepper dails wait for stepper to get to position
	virtual void updateNow();
	virtual void powerState(boolean setOn);
	virtual void lightState(boolean setOn);
 
  static CANareoInterface* myCANbus;
private:
	static void _canCallback(CAN_FRAME* pframe);
	struct indicatorControl {
		GenericIndicator*	indicator = NULL;
	};

	indicatorControl	myIndicators[10];
	uint8_t				_canasNodeID = 0;
	uint8_t				_canasRedundantNodeID = 0;
	uint8_t				_canasServiceChannel = 0;

	char			_instrumentCode[8];
	bool			_powerOn = false;
	bool			_LightsOn = false;
	int				_numIndicators = 0;
	int				_ledPin;
	int				_maxIndicators = 10;
	//Xcomm			*dataConnection = NULL;
	MultiStepper	allSteppers;

	// data to control the leds
	int _ledFreq = 5000;
	int _ledChannel = 0;
	int _resolution = 8;

	
};
#endif
