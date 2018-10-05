//=================================================================================================
//
// Instrument.cpp
//
// author: Frank van Ierland
// version: 0.1
//
// Class to control the whole instrument
//
//=================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#include "Instrument.h"



Instrument::Instrument(char* newCode, int ledPin, int maxIndicators )
{
	DPRINT("CREATE Instrument for:");
	DPRINTLN(newCode);

	strcpy(_instrumentCode, newCode);
	_ledPin = ledPin;

	if (maxIndicators < _maxIndicators)
		_maxIndicators = maxIndicators;

	// set up control of leds
	ledcSetup(_ledChannel, _ledFreq, _resolution);
	ledcAttachPin(_ledPin, _ledChannel);
	ledcWrite(_ledChannel, 0);

	DPRINTLN("CREATE Instrument DONE");
}


Instrument::~Instrument()
{
	if (dataConnection != NULL)
		delete dataConnection;
}

int Instrument::initiateCommunucation()
{
	if (dataConnection != NULL)
		return -1;

	dataConnection = new Xcomm(_instrumentCode, _maxIndicators);
	
	return 0;
}

int Instrument::addIndicator(GenericIndicator * newIndicator)
{
	if (_numIndicators = _maxIndicators)
		return -1;

	myIndicators[_numIndicators].indicator = newIndicator;
	_numIndicators++;
	dataConnection->addElement(newIndicator, true);
	if (newIndicator->type = GenericIndicator::INDICATOR_STEPPER) {
		AccelStepper* tempStepper = (AccelStepper *) newIndicator;
		allSteppers.addStepper(*tempStepper);
	}
	
	return 0;
}

void Instrument::calibrate()
{
	for (int i = 0; i < _numIndicators; i++) {
		myIndicators[i].indicator->calibrate();
	}
}

void Instrument::toZero()
{
	/// TODO: Code
}

void Instrument::update()
{
	if (dataConnection != NULL)
		dataConnection->checkQueue();
	allSteppers.run();
}

void Instrument::updateNow()
{
	if (dataConnection != NULL)
		dataConnection->checkQueue();
	allSteppers.runSpeedToPosition();
}

void Instrument::powerState(boolean setOn)
{
	_powerOn = setOn;
	for (int i = 0; i < _numIndicators; i++) {
		myIndicators[i].indicator->powerState(setOn);
	}
}

void Instrument::lightState(boolean setOn)
{
	_LightsOn = setOn;
	ledcWrite(_ledChannel, (_LightsOn) ? 255 : 0);
}
