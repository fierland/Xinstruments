//-------------------------------------------------------------------------------------------------------------------
//	 CAN Aerospace class
//
//  Franks Flightsim Intruments project
//	by Frank van Ierland
//
// A light implementation of the CAN Aerospace protocol to manage simulated instruments.
//	This code is in the public domain.
//
//	VERSION HISTORY:
//	
//
//-------------------------------------------------------------------------------------------------------------------

#include <stdlib.h>
#include "CANaero.h"
#include "CANdriver.h"
#include <esp32_can.h>


CANaero::CANaero()
{
}

CANaero::~CANaero()
{
}

void CANaero::setCanDriver(CANdriver * canDriver)
{
	_canBus = canDriver;
}



int CANaero::ParamSubscribe(uint16_t msg_id, GenericIndicator* instrument)
{
	DPRINTLN("CANaero:addElement");
	DPRINT("adding:");
	DPRINTLN(msg_id);

	int curRecord = -1;
	_CanDataLinks* newRef;

	DPRINT("RegisterDataRef:START:");

	if (instrument == NULL)
		return -1;

	curRecord = _findInList(msg_id);

	if (curRecord == -1) {
		//not found so create new item
		DPRINTLN("New record");
		newRef = new _CanDataLinks;
		DPRINTLN("added object");
		_listRefs.push_back(newRef);
		DPRINTLN("in list");
		newRef->linkId = _listRefs.size();
		DPRINTLN("id adeded");
		newRef->canAreoId=msg_id;
		DPRINTLN("type added");
		newRef->instrument = instrument;
		
		// set filter in canbus to captture this element
		_canBus->setFilter(msg_id, _CallBack);
	}
	DPRINTLN("record processed");

	DPRINTLN("CANaero:addElement done");
	return 0;
}

int CANaero::ParamUnsubscribe(uint16_t msg_id)
{
	// TODO: code
	return 0;
}

int CANaero::ParamRead(uint16_t msg_id, float* pargs)
{
	// TODO: code
	return 0;
}

void CANaero::_CallBack(CAN_FRAME * message)
{
	CanasCanFrame pframe;
	CanasDataContainer canData;

	_CanDataLinks* tmpRef;
	int	listId;
	float newValue;

	_canBus->can2areo(&pframe, message);
	listId = _findInList(pframe.id);

	if (listId != -1)
	{
		tmpRef = _listRefs[listId];
		for (int i = 0; i < 4; i++)
			canData.ACHAR4[i] = pframe.data[i+4];

		newValue = (float)canData.FLOAT;

		if (tmpRef->lastVal != newValue)
		{
			tmpRef->lastVal = newValue;
			tmpRef->timestamp = millis();
			tmpRef->instrument->setValue(newValue);
		}
	}

}

int CANaero::_findInList(uint8_t toFind) {

	int curRecord = -1;
	_CanDataLinks* tmpRef;

	DPRINT("find in list:");
	DPRINTLN(toFind);
	DPRINT("items in list:");
	DPRINTLN(_listRefs.size());


	for (int i = 0; i < _listRefs.size(); i++) {
		DPRINT("check item:"); DPRINTLN(i);
		tmpRef = _listRefs[i];
		DPRINT("Compaire:");
		DPRINT(toFind);
		DPRINT(":with:");
		DPRINTLN(tmpRef->canAreoId);
		if (toFind - tmpRef->canAreoId) {
			curRecord = i;
			break;
		}
	}

	DPRINT("find in list done:");
	DPRINTLN(curRecord);

	return curRecord;
}

//-------------------------------------------------------------------------------------------------------------------
// internal call if new value from master then update n internal array and call to update physical instrument.
//-------------------------------------------------------------------------------------------------------------------

int CANaero::_updateValue(uint8_t type, float value)
{
	int curRecord = -1;
	_CanDataLinks* newRef;
	DPRINTLN("CANAero:_updateValue START");

	curRecord = _findInList(type);
	if (curRecord > -1)
	{
		newRef = _listRefs[curRecord];
		DPRINT("CANAero:Current Value:");
		DPRINT(newRef->lastVal);
		DPRINT(":new value:");
		DPRINTLN(value);
		if (newRef->lastVal != value)
		{
			DPRINTLN("value changed");
			newRef->lastVal = value;
			DPRINTLN("Check for instrument");
			if (newRef->instrument != NULL)
				DPRINTLN("Call update function");
			newRef->instrument->setValue(value);
		}
	}

	DPRINTLN("CANareo:_updateValue END");
	return 0;
}
