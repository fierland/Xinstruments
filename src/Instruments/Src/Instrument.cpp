//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================
//
// Instrument.cpp
// version: 0.1
//
// Class to control the whole instrument
//
//=================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#include "Instrument.h"

//-------------------------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------------------------
Instrument::Instrument(char* newCode, uint8_t nodeId, uint8_t srv_channel, int ledPin, int maxIndicators)
{
	DPRINT("CREATE Instrument for:");
	DPRINTLN(newCode);

	strcpy(_instrumentCode, newCode);

	_canasNodeID = nodeId;
	_ledPin = ledPin;

	myCANbus = new CANaero(nodeId, srv_channel, XI_Hardware_Revision, XI_Software_Revision);

	if (maxIndicators < _maxIndicators)
		_maxIndicators = maxIndicators;

	// set up control of leds
	ledcSetup(_ledChannel, _ledFreq, _resolution);
	ledcAttachPin(_ledPin, _ledChannel);
	ledcWrite(_ledChannel, 0);

	DPRINTLN("CREATE Instrument DONE");
}
//-------------------------------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------------------------------

Instrument::~Instrument()
{
	/*
		if (dataConnection != NULL)
			delete dataConnection;
	 */
	delete myCANbus;
}
//-------------------------------------------------------------------------------------------------
// Start communication
//-------------------------------------------------------------------------------------------------
int Instrument::initiateCommunication()
{
	//if (dataConnection != NULL)
	//	return -1;
	//dataConnection = new Xcomm(_instrumentCode, _maxIndicators);

	/// TODO: Get instrument code
	//myCANbus->setNodeId(_canasNodeID);
	//myCANbus->setServiceChannel(_canasServiceChannel);
	myCANbus->start();

	return 0;
}
//-------------------------------------------------------------------------------------------------
// add new indicator
//-------------------------------------------------------------------------------------------------
int Instrument::addIndicator(GenericIndicator * newIndicator)
{
	if (_numIndicators = _maxIndicators)
		return -1;

	myIndicators[_numIndicators].indicator = newIndicator;
	_numIndicators++;
	newIndicator->startCommunication(&myCANbus);
	if (newIndicator->type = GenericIndicator::INDICATOR_STEPPER) {
		AccelStepper* tempStepper = (AccelStepper *)newIndicator;
		allSteppers.addStepper(*tempStepper);
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Instrument::calibrate()
{
	for (int i = 0; i < _numIndicators; i++) {
		myIndicators[i].indicator->calibrate();
	}
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Instrument::toZero()
{
	/// TODO: Code
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Instrument::update()
{
	//if (dataConnection != NULL)
	//	dataConnection->checkQueue();
	myCANbus->Update();
	allSteppers.run();
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Instrument::updateNow()
{
	//if (dataConnection != NULL)
	//	dataConnection->checkQueue();
	myCANbus->Update();
	allSteppers.runSpeedToPosition();
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Instrument::powerState(boolean setOn)
{
	_powerOn = setOn;
	for (int i = 0; i < _numIndicators; i++) {
		myIndicators[i].indicator->powerState(setOn);
	}
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Instrument::lightState(boolean setOn)
{
	_LightsOn = setOn;
	ledcWrite(_ledChannel, (_LightsOn) ? 255 : 0);
}