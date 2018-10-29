//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================
//
//	 CAN Aerospace class
//
//  Franks Flightsim Intruments project
//	by Frank van Ierland
//
// A light implementation of the CAN Aerospace protocol to manage simulated instruments.
//	This code is in the public domain.
//
//
//	VERSION HISTORY:
//
//
//==================================================================================================

#include <stdlib.h>
#include "CANaero.h"
#include "CAN_ids.h"
#include "mydebug.h"
//#include "CANdriver.h"

//#define RANGEINCLUSIVE(x, min, max) ((x) >= (min) && (x) <= (max))

/**
 * Maximum number of nodes per one network
 */
static const int CANAS_MAX_NODES = 255;

//-------------------------------------------------------------------------------------------------------------------
// constructor
//-------------------------------------------------------------------------------------------------------------------
CANareoInterface::CANareoInterface(uint8_t nodeId, uint8_t serviceChannel, uint8_t hdwId, uint8_t swId, void(*myCallBack)(CAN_FRAME*))
{
	CanasSrvIdsPayload canasCfg;

	DPRINTINFO("START");

	_node_id = nodeId;
	_service_channel = serviceChannel; // for now set service channel to node id

	canasCfg.hardware_revision = hdwId;
	canasCfg.software_revision = swId;
	canasCfg.header_type = CANAS_SRV_IDS_ID_DISTRIBUTION_STD;
	canasCfg.id_distribution = CANAS_SRV_IDS_HEADER_TYPE_STD;

	_callBack = myCallBack;

	_identService = new CANAS_service_identification((CANaeroBase*)this, &canasCfg);
	_requestService = new CANAS_service_requestdata((CANaeroBase*)this);
	_timeStampService = new CANAS_service_timestamp((CANaeroBase*)this);

	ServiceRegister(_identService);		// register service ...
	ServiceRegister(_timeStampService);

	DPRINTINFO("STOP");
}

//-------------------------------------------------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------------------------------------------------

CANareoInterface::~CANareoInterface()
{
	DPRINTINFO("START");

	delete _requestService;
	delete _identService;
	delete _timeStampService;

	DPRINTINFO("STOP");
}

//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
static int _diffU8(uint8_t a, uint8_t b)
{
	const int d = a - b;
	if (d <= -128)
		return 256 + d;
	else if (d >= 127)
		return d - 256;
	return d;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANareoInterface::_handleReceivedParam(CanasMessage* pframe, int subId, long timestamp)
{
	_CanDataLinks* curParm;
	DPRINTINFO("START");

	curParm = _GetDataLinkInList(subId);

	// check if new message
	/// positive if a > b
	int msg_diff = _diffU8(pframe->message_code, curParm->last_message_code);

	if (msg_diff <= 0)
	{
		DPRINT("No new message code Last=");
		DPRINT(curParm->last_message_code);
		DPRINT(" new=");
		DPRINTLN(pframe->message_code);

		return 0;
	}
	curParm->last_message_code = pframe->message_code;
	curParm->timestamp = timestamp;
	if (curParm->lastVal != pframe->data.container.FLOAT)
	{
		/// TODO: check type of parameter and use corect cast
		curParm->lastVal = pframe->data.container.FLOAT;
		curParm->indicator->setValue(curParm->lastVal);
	}

	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANareoInterface::_handleReceivedService(CanasMessage* pframe, int subId, long timestamp)
{
	_CanServiceLinks* curParm;
	DPRINTINFO("START");

	curParm = _serviceRefs.at(subId);

	// check if new message

	DPRINTINFO("STOP");
	return (curParm->service->ProcessFrame(pframe));
}
//-------------------------------------------------------------------------------------------------------------------
// update canbus check for new input
//-------------------------------------------------------------------------------------------------------------------
int CANareoInterface::Update()
{
	const uint64_t timestamp = _canBus.timeStamp();
	CanasCanFrame frame;
	CanasMessage msg;
	MessageGroup msggroup = MSGGROUP_WTF;
	int ret = 0;
	int curRecord = -1;

	//DPRINTINFO("START");

	if (!_running)
		return -CANAS_ERR_NOT_RUNNING;

	if (_canBus.receive(&frame) != 0)
	{
//		DPRINTINFO("STOP");
		return 0;
	};

	DPRINT("received:");
	DPRINTLN(CANAS_DUMPFRAME(&frame));

	if (frame.dlc < 4 || frame.dlc>8)
	{
		DPRINTLN("update: data size incorrect");
		ret = -CANAS_ERR_BAD_CAN_FRAME;
	}

	if (frame.id & CANAS_CAN_FLAG_RTR)
	{
		DPRINTLN("frameparser: RTR flag is not allowed");
		return -CANAS_ERR_BAD_CAN_FRAME;
	}

	CANdriver::frame2msg(&msg, &frame);

	msggroup = _detectMessageGroup(msg.can_id);
	if (msggroup == MSGGROUP_WTF)
	{
		DPRINTLN("update: failed to detect the message group");
		ret = -CANAS_ERR_BAD_MESSAGE_ID;
	}

	if (msggroup == MSGGROUP_PARAMETER)
	{
		DPRINT("CANaero:update record=");
		DPRINTLN(msg.service_code);

		curRecord = _findDataLinkInList(msg.can_id);
		if (curRecord != -1)
		{
			_handleReceivedParam(&msg, curRecord, timestamp);
		}
		else
		{
			DPRINT("foreign param msgid=");
			DPRINT(msg.service_code);
			DPRINT(" datatype=");
			DPRINTLN(msg.data.type);
		}
	}
	else if (msggroup == MSGGROUP_SERVICE)
	{
		curRecord = _findServiceInList(msg.service_code);
		if (curRecord != -1)
		{
			_handleReceivedService(&msg, curRecord, timestamp);
		}
		else
		{
			DPRINT("foreign service msgid=");
			DPRINT(msg.service_code);
			DPRINT(" datatype=");
			DPRINTLN(msg.data.type);
		}
	}

	//DPRINTINFO("STOP");
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANareoInterface::ParamSubscribe(GenericIndicator* newIndicator)
{
	DPRINTINFO("START");
	if (newIndicator == NULL)
		return -CANAS_ERR_ARGUMENT;

	uint8_t msg_id = newIndicator->getCanAeroId();
	DPRINT("adding:");
	DPRINTLN(msg_id);

	int curRecord = -1;
	_CanDataLinks* newRef;

	DPRINT("RegisterDataRef:START:");

	if (_detectMessageGroup(msg_id) != MSGGROUP_PARAMETER)
		return -CANAS_ERR_BAD_MESSAGE_ID;

	curRecord = _findDataLinkInList(msg_id);

	if (curRecord == -1)
	{
		//not found so create new item
		DPRINTLN("New record");
		newRef = new _CanDataLinks;
		DPRINTLN("added object");
		_dataLinkRefs.push_back(newRef);
		DPRINTLN("in list");
		newRef->linkId = _dataLinkRefs.size();
		DPRINTLN("id adeded");
		newRef->canAreoId = msg_id;
		DPRINTLN("type added");
		newRef->indicator = newIndicator;

		// set filter in canbus to capture this element
		_canBus.setFilter(msg_id, _callBack);
		/// TODO: send notification to master to start sending data
		_requestService->Request(msg_id, XI_Base_NodeID);
	}
	else
		return -CANAS_ERR_ENTRY_EXISTS;

	DPRINTLN("record processed");

	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANareoInterface::ParamUnsubscribe(uint16_t msg_id)
{
	CanasCanFrame pframe;
	_CanDataLinks* tmpRef;
	int	listId;
	DPRINTINFO("START");

	DPRINT("removing:");
	DPRINTLN(msg_id);

	listId = _findDataLinkInList(msg_id);
	if (listId == -1)
		return -CANAS_ERR_NO_SUCH_ENTRY;

	/// TODO: send notification to master to stop sending data

	tmpRef = _GetDataLinkInList(listId);
	_dataLinkRefs.clear(listId);
	delete tmpRef;

	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANareoInterface::ParamRead(uint16_t msg_id, float* pargs)
{
	DPRINTINFO("START");

	// TODO: code
	DPRINTINFO("STOP");

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// register new service on bus
//-------------------------------------------------------------------------------------------------------------------
int CANareoInterface::ServiceRegister(CANAS_service* newService)
{
	DPRINTINFO("START");

	if (newService == NULL)
		return -CANAS_ERR_ARGUMENT;

	uint8_t service_code = newService->serviceId();

	DPRINT("adding:");
	DPRINTLN(service_code);

	_CanServiceLinks* newRef;

	int curRecord = _findServiceInList(service_code);

	if (curRecord != -1)
		return -CANAS_ERR_ENTRY_EXISTS;

	//not found so create new item
	DPRINTLN("New record");
	newRef = new _CanServiceLinks;
	DPRINTLN("added object");
	_serviceRefs.push_back(newRef);
	DPRINTLN("in list");
	newRef->linkId = _serviceRefs.size();
	DPRINTLN("id adeded");
	newRef->canServiceCode = service_code;
	DPRINTLN("type added");
	newRef->service = newService;

	DPRINTINFO("STOP");

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANareoInterface::ServiceRegister_requestData(int(*newService)(uint8_t canasID), int(*removeService)(uint8_t canasID))
{
	_newXservicecall = newService;
	_removeXservicecall = removeService;

	return ServiceRegister(_requestService);
}

//-------------------------------------------------------------------------------------------------------------------
// unregister a service
//-------------------------------------------------------------------------------------------------------------------
int CANareoInterface::ServiceUnregister(uint8_t service_code)
{
	DPRINTINFO("START");
	DPRINT("removing:");
	DPRINTLN(service_code);

	int curService = _findServiceInList(service_code);
	_CanServiceLinks* tmpRef;
	CanasCanFrame pframe;

	if (curService == -1)
		return -CANAS_ERR_NO_SUCH_ENTRY;

	/// TODO: send notification to master to stop sending data

	tmpRef = _serviceRefs[curService];
	_serviceRefs.clear(curService);

	delete tmpRef;

	DPRINTINFO("STOP");

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
void CANareoInterface::CallBack(CAN_FRAME * message)
{
	CanasCanFrame pframe;
	CanasDataContainer canData;

	DPRINTINFO("START");

	_CanDataLinks* tmpRef;
	int	listId;
	float newValue;

	CANdriver::can2areo(&pframe, message);
	listId = _findDataLinkInList(pframe.id);

	if (listId != -1)
	{
		tmpRef = _dataLinkRefs[listId];
		for (int i = 0; i < 4; i++)
			canData.ACHAR4[i] = pframe.data[i + 4];

		newValue = (float)canData.FLOAT;

		if (tmpRef->lastVal != newValue)
		{
			tmpRef->lastVal = newValue;
			tmpRef->timestamp = CANdriver::timeStamp();
			tmpRef->indicator->setValue(newValue);
		}
	}

	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------------------------
// helper function to find a item i the list of  subscribed items
//-------------------------------------------------------------------------------------------------------------------
int CANareoInterface::_findDataLinkInList(uint8_t toFind)
{
	int curRecord = -1;
	_CanDataLinks* tmpRef;

	DPRINTINFO("START");

	DPRINT("find in list:");
	DPRINTLN(toFind);
	DPRINT("items in list:");
	DPRINTLN(_dataLinkRefs.size());

	for (int i = 0; i < _dataLinkRefs.size(); i++)
	{
		DPRINT("check item:"); DPRINTLN(i);
		tmpRef = _GetDataLinkInList(i);
		DPRINT("Compaire:");
		DPRINT(toFind);
		DPRINT(":with:");
		DPRINTLN(tmpRef->canAreoId);
		if (toFind - tmpRef->canAreoId)
		{
			curRecord = i;
			break;
		}
	}

	DPRINTINFO("STOP");
	DPRINT("find in list done:");
	DPRINTLN(curRecord);

	return curRecord;
}

_CanDataLinks* CANareoInterface::_GetDataLinkInList(int i)
{
	return (_dataLinkRefs[i]);
}
//-------------------------------------------------------------------------------------------------------------------
// helper function to find a item i the list of  subscribed items
//-------------------------------------------------------------------------------------------------------------------
int CANareoInterface::_findServiceInList(uint8_t toFind)
{
	int curRecord = -1;
	_CanServiceLinks* tmpRef;

	DPRINTINFO("START");

	DPRINT("find in list:");
	DPRINTLN(toFind);
	DPRINT("items in list:");
	DPRINTLN(_serviceRefs.size());

	int rsize = _serviceRefs.size();
	for (int i = 0; i < rsize; i++)
	{
		DPRINT("check item:"); DPRINTLN(i);
		tmpRef = _serviceRefs[i];
		DPRINT("Compaire:");
		DPRINT(toFind);
		DPRINT(":with:");
		DPRINTLN(tmpRef->canServiceCode);
		if (toFind - tmpRef->canServiceCode)
		{
			curRecord = i;
			break;
		}
	}

	DPRINTINFO("STOP");

	DPRINT("find in list done:");
	DPRINTLN(curRecord);

	return curRecord;
}

//-------------------------------------------------------------------------------------------------------------------
// internal call if new value from master then update n internal array and call to update physical instrument.
//-------------------------------------------------------------------------------------------------------------------

int CANareoInterface::_updateValue(uint8_t type, float value)
{
	int curRecord = -1;
	_CanDataLinks* newRef;

	DPRINTINFO("START");

	curRecord = _findDataLinkInList(type);
	if (curRecord > -1)
	{
		newRef = _dataLinkRefs[curRecord];
		DPRINT("CANAero:Current Value:");
		DPRINT(newRef->lastVal);
		DPRINT(":new value:");
		DPRINTLN(value);
		if (newRef->lastVal != value)
		{
			DPRINTLN("value changed");
			newRef->lastVal = value;
			DPRINTLN("Check for instrument");
			if (newRef->indicator != NULL)
				DPRINTLN("Call update function");
			newRef->indicator->setValue(value);
		}
	}

	DPRINTINFO("STOP");

	return 0;
};
