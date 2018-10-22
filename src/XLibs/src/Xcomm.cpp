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

//
//	constructor
//
Xcomm::Xcomm(char * instrumentCode, int maxElements)
{
	// TODO: Start sesion with master
	// XP_CAN_startConnection(instrumentCode);
}

//
//	data reader for in loop() pols the udp port for new messages and dispaces them to controls
//
int Xcomm::dataReader()
{
	int message_size = 0;
	int noBytes = 0;
	int curRecord = -1;
	_DataRefs* tmpRef;
	_DataRefs* curRef = NULL;

	return _LastError;
}

//
// create new data referece link with X-Plane
//

int Xcomm::addElement(char * type, void(*callBack)(float), uint32_t rangeMax, uint32_t rangeMin, boolean readOnly)
{
	//int XpUDP::registerDataRef(int iFreq, const char *sCode, void(*callBack)(float)) {
	int curRecord = -1;
	_DataLinks* newRef;

	DPRINT("RegisterDataRef:START:");

	curRecord = _findInList(type);

	if (curRecord == -1) {
		//not found so create new item
		DPRINT("New record");
		newRef = new _DataLinks;
		_listRefs.push_back(newRef);
		newRef->linkId = _listRefs.size();
		strcpy(newRef->elementId, sCode);
		newRef->setdata = callBack;
		newRef->readOnly = readOnly;
		newRef->rangeMax = rangeMax;
		newRef->rangeMin = rangeMin;
	}
	else {
		// found a record so update item
		DPRINT("existing reccord");
		newRef = _listRefs[curRecord];
	}

	// check if active and for last timestamp
	if (!newRef->subscribed || (millis() - newRef->timestamp) > _MAX_INTERVAL) {
		// create new request
		DPRINT("create x-plane request");
		newRef->subscribed = true;
		// read first returned value
		// TODO
		//dataReader();
	}

	return 0;
}

int Xcomm::setValue(char * type, uint32_t value)
{
	int curRecord = -1;
	_DataLinks* newRef;

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

	return 0;
}

void Xcomm::checkQueue()
{
	//TODO check for new messages in CANBUS
}

int Xcomm::_findInList(char* toFind) {
	int curRecord = -1;
	_DataLinks* tmpRef;

	for (int i = 0; i < _listRefs.size(); i++) {
		tmpRef = _listRefs[i];
		if (strcmp(tmpRef->elementId, type) == 0) {
			curRecord = i;
			break;
		}
	}

	return curRecord;
}