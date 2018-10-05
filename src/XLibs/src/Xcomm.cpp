//===================================================================================================================
// Xcomm.cpp
//
// author: Frank van Ierland
// version: 0.1
//
// Communication module for comm between interface master and instruments
//
//===================================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland


#include "mydebug.h"
#include "Xcomm.h"

//-------------------------------------------------------------------------------------------------------------------
//	constructor
//-------------------------------------------------------------------------------------------------------------------
Xcomm::Xcomm(char * instrumentCode, int maxElements)
{

	// TODO: Start sesion with master
	CAN0.begin(500000);
	

	// send can message to connect to master

	// XP_CAN_startConnection(instrumentCode);
}


//-------------------------------------------------------------------------------------------------------------------
//	data reader for in loop() pols the udp port for new messages and dispaces them to controls
//-------------------------------------------------------------------------------------------------------------------
/*
int Xcomm::dataReader()
{
	int message_size =0;
	int noBytes =0 ;
	int curRecord = -1;

	_DataRefs* tmpRef;
	_DataRefs* curRef = NULL;
	




	return _LastError;
}
*/

//-------------------------------------------------------------------------------------------------------------------
// create new data referece link with X-Plane
//-------------------------------------------------------------------------------------------------------------------

int Xcomm::addElement( GenericIndicator *stepObject,  boolean readOnly)
{


	DPRINTLN("XCOMM:addElement");
	DPRINT("adding:");
	DPRINTLN(stepObject->getCanAeroId);

//int XpUDP::registerDataRef(int iFreq, const char *sCode, void(*callBack)(float)) {
	int curRecord = -1;
	_DataLinks* newRef;
	

	DPRINT("RegisterDataRef:START:");

	curRecord = _findInList(stepObject->getCanAeroId);
		
	if (curRecord == -1) {
		//not found so create new item
		DPRINTLN("New record");
		newRef = new _DataLinks;
		DPRINTLN("added object");
		_listRefs.push_back(newRef);
		DPRINTLN("in list");
		newRef->linkId = _listRefs.size();
		DPRINTLN("id adeded");
		strcpy(newRef->elementId, stepObject->getCanAeroId);
		DPRINTLN("type added");
		newRef->readOnly = readOnly;
		//newRef->rangeMax = rangeMax;
		//newRef->rangeMin = rangeMin;
		newRef->myStepper = stepObject;
	}
	else {
		// found a record so update item 
		DPRINT("existing reccord");
		newRef = _listRefs[curRecord];

	}
	DPRINTLN("record processed");

	// check if active and for last timestamp
	if (!newRef->subscribed || (millis() - newRef->timestamp) > _MAX_INTERVAL) {
		// create new request
		DPRINT("create x-plane request");
		newRef->subscribed = true;
		// read first returned value 
		// TODO
		//dataReader();
	}

	DPRINTLN("XCOMM:addElement done");
	return 0;
}


int Xcomm::setValue(char * type, float value)
{
	int curRecord = -1;
	_DataLinks* newRef;

	DPRINTLN("Xcomm:setValue Start");

	
	curRecord = _findInList(type);
	if (curRecord > -1)
	{
		newRef = _listRefs[curRecord];
		if (newRef->lastVal != value)
		{
			// TODO send new value to master
			// XP_CAN_send(type,value);
		}
	}
	
	DPRINTLN("Xcomm:setValue End");
	return 0;
}

int Xcomm::processInput(char * type, float value)
{
	DPRINTLN("Xcomm:processInput Start");
	
	return _updateValue(type, value);
	DPRINTLN("Xcomm:processInput END");
	
}

//-------------------------------------------------------------------------------------------------------------------
// internal call if new value from master then update n internal array and call to update physical instrument.
//-------------------------------------------------------------------------------------------------------------------

int Xcomm::_updateValue(char * type, float value)
{
	int curRecord = -1;
	_DataLinks* newRef;
	DPRINTLN("Xcomm:_updateValue START");

	curRecord = _findInList(type);
	if (curRecord > -1)
	{
		newRef = _listRefs[curRecord];
		DPRINT("Xcomm:Current Value:");
		DPRINT(newRef->lastVal);
		DPRINT(":new value:");
		DPRINTLN(value);
		if (newRef->lastVal != value)
		{
			DPRINTLN("value changed");
			newRef->lastVal = value;
			DPRINTLN("Check for stepper");
			if (newRef->myStepper != NULL)
				DPRINTLN("Call update function");
				newRef->myStepper->setValue(value);
		}
	}
	
	DPRINTLN("Xcomm:_updateValue END");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

void Xcomm::checkQueue()
{
	//TODO check for new messages in CANBUS

}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

int Xcomm::_findInList(char* toFind) {
	int curRecord = -1;
	_DataLinks* tmpRef;
	
	DPRINT("find in list:");
	DPRINTLN(toFind); 
	DPRINT("items in list:"); 
	DPRINTLN(_listRefs.size());


	for (int i = 0; i< _listRefs.size(); i++) {
		DPRINT("check item:"); DPRINTLN(i);
		tmpRef = _listRefs[i];
	DPRINT("Compaire:");
	DPRINT(toFind); 
	DPRINT(":with:"); 
	DPRINTLN(tmpRef->elementId);		
		if (strcmp(tmpRef->elementId, toFind) == 0) {
			curRecord = i;
			break;
		}
	}

	DPRINT("find in list done:");
	DPRINTLN(curRecord);

	return curRecord;
}


