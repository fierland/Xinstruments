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
#include "Xinstruments.h"

CANareoInterface* Instrument::myCANbus=NULL;

void Instrument::_canCallback(CAN_FRAME* pframe)
{
  DPRINTINFO("START");
  
  if (Instrument::myCANbus!= NULL)
	  Instrument::myCANbus->CallBack(pframe);
  DPRINTINFO("STOP");
}

//-------------------------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------------------------
Instrument::Instrument(char* newCode, uint8_t nodeId, uint8_t srv_channel, int ledPin, int maxIndicators)
{
  DPRINTINFO("START");
	DPRINT("CREATE Instrument for:");
	DPRINTLN(newCode);

	strcpy(_instrumentCode, newCode);

	_canasNodeID = nodeId;
	_ledPin = ledPin;

	myCANbus = new CANareoInterface(nodeId, srv_channel, XI_Hardware_Revision, XI_Software_Revision, _canCallback);

	if (maxIndicators < _maxIndicators)
		_maxIndicators = maxIndicators;

	// set up control of leds
	ledcSetup(_ledChannel, _ledFreq, _resolution);
	ledcAttachPin(_ledPin, _ledChannel);
	ledcWrite(_ledChannel, 0);

 DPRINTINFO("STOP");
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
  DPRINTINFO("START");
	
	/// TODO: Get instrument code
	//myCANbus->setNodeId(_canasNodeID);
	//myCANbus->setServiceChannel(_canasServiceChannel);
  
  int res = myCANbus->start();
	
  DPRINTINFO("STOP");
	return res;
}
//-------------------------------------------------------------------------------------------------
// add new indicator
//-------------------------------------------------------------------------------------------------
int Instrument::addIndicator(GenericIndicator& newIndicator)
{
  DPRINTINFO("START");
	if (_numIndicators == _maxIndicators)
		return -INSTR_ERR_QUOTA_EXCEEDED;

	myIndicators[_numIndicators].indicator = &newIndicator;
	_numIndicators++;
  
	myCANbus->ParamSubscribe(&newIndicator);
	
	if (newIndicator.type() == IndicatorType::INDICATOR_STEPPER)
	{
    DPRINTLN("adding stepper to multistepper");
		//AccelStepper* tempStepper = (AccelStepper *)newIndicator;
		if (!allSteppers.addStepper((AccelStepper&)newIndicator))
		  return -INSTR_ERR_LOGIC;
	}
 
  
  DPRINTINFO("STOP");
	return INSTR_ERR_OK;
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Instrument::calibrate()
{  
  DPRINTINFO("START");
	for (int i = 0; i < _numIndicators; i++)
	{
    DPRINT("Calibrating indicator:");
    DPRINTLN(i);
		myIndicators[i].indicator->calibrate();
	}
 DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Instrument::toZero()
{
  DPRINTINFO("START");
	/// TODO: Code
  DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Instrument::update()
{
  DPRINTINFO("START");
	
	myCANbus->Update();
	allSteppers.run();
  
  DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Instrument::updateNow()
{
  DPRINTINFO("START");
	
	myCANbus->Update();
  DPRINTLN("Start run speed");
	allSteppers.runSpeedToPosition();
  DPRINTLN("End run speed");
 
  DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Instrument::powerState(boolean setOn)
{
    DPRINTINFO("START");
	_powerOn = setOn;
	for (int i = 0; i < _numIndicators; i++)
	{
		myIndicators[i].indicator->powerState(setOn);
	}
 DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Instrument::lightState(boolean setOn)
{
    DPRINTINFO("START");
	_LightsOn = setOn;
	ledcWrite(_ledChannel, (_LightsOn) ? 255 : 0);
  DPRINTINFO("STOP");
}
