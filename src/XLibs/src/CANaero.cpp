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

#define RANGEINCLUSIVE(x, min, max) ((x) >= (min) && (x) <= (max))

static MessageGroup _detectMessageGroup(uint16_t id)
{
	if (RANGEINCLUSIVE(id, CANAS_MSGTYPE_EMERGENCY_EVENT_MIN, CANAS_MSGTYPE_EMERGENCY_EVENT_MAX))
		return MSGGROUP_PARAMETER;

	if (RANGEINCLUSIVE(id, CANAS_MSGTYPE_NODE_SERVICE_HIGH_MIN, CANAS_MSGTYPE_NODE_SERVICE_HIGH_MAX))
		return MSGGROUP_SERVICE;

	if (RANGEINCLUSIVE(id, CANAS_MSGTYPE_USER_DEFINED_HIGH_MIN, CANAS_MSGTYPE_USER_DEFINED_HIGH_MAX))
		return MSGGROUP_PARAMETER;

	if (RANGEINCLUSIVE(id, CANAS_MSGTYPE_NORMAL_OPERATION_MIN, CANAS_MSGTYPE_NORMAL_OPERATION_MAX))
		return MSGGROUP_PARAMETER;

	if (RANGEINCLUSIVE(id, CANAS_MSGTYPE_USER_DEFINED_LOW_MIN, CANAS_MSGTYPE_USER_DEFINED_LOW_MAX))
		return MSGGROUP_PARAMETER;

	if (RANGEINCLUSIVE(id, CANAS_MSGTYPE_DEBUG_SERVICE_MIN, CANAS_MSGTYPE_DEBUG_SERVICE_MAX))
		return MSGGROUP_PARAMETER;

	if (RANGEINCLUSIVE(id, CANAS_MSGTYPE_NODE_SERVICE_LOW_MIN, CANAS_MSGTYPE_NODE_SERVICE_LOW_MAX))
		return MSGGROUP_SERVICE;

	DPRINT("msggroup: failed to detect, msgid=");
	DPRINTLN(id);
	
	return MSGGROUP_WTF;
}




//-------------------------------------------------------------------------------------------------------------------
// constructor
//-------------------------------------------------------------------------------------------------------------------
CANaero::CANaero(uint8_t nodeId, uint8_t hdwId, uint8_t swId )
{
	CanasSrvIdsPayload canasCfg;

	_node_id = nodeId;

	canasCfg.hardware_revision	= hdwId;
	canasCfg.software_revision	= swId;
	canasCfg.header_type		= CANAS_SRV_IDS_ID_DISTRIBUTION_STD;
	canasCfg.id_distribution	= CANAS_SRV_IDS_HEADER_TYPE_STD;
	
	_identService = new CANAS_service_identification(this, &canasCfg);	   
	_requestService = new CANAS_service_requestdata(this);
};

//-------------------------------------------------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------------------------------------------------

CANaero::~CANaero()
{
	delete _requestService;
	delete _identService;
}

//-------------------------------------------------------------------------------------------------------------------
// set the id for this node to use
//-------------------------------------------------------------------------------------------------------------------
int CANaero::setNodeId(int newID)
{
		   
	_node_id = newID;

	//canasCfg.hardware_revision = 
	
	/*
    CanasConfig cfg = canasMakeConfig();
    cfg.filters_per_iface = CAN_FILTERS_PER_IFACE;
    cfg.iface_count = CAN_IFACE_COUNT;
    cfg.fn_send     = _drvSend;
    cfg.fn_filter   = _drvFilter;
    cfg.fn_malloc   = _staticMalloc;        // No dynamic memory needed. Thus, free() is not necessary too.
    cfg.fn_timestamp = _getTimestamp;
    // Each function pointer is initialized to NULL, so no need to assign them by hand.
    // These settings are application-defined:
    cfg.node_id           = MY_NODE_ID;
    cfg.redund_channel_id = MY_REDUND_CHAN;
    cfg.service_channel   = MY_SERVICE_CHAN;

	memset(&_canas_platform, 0, sizeof(_canas_platform));
	_canas_platform.rmutex = xSemaphoreCreateRecursiveMutex();
	ASSERT_ALWAYS(_canas_platform.rmutex != NULL);

	ASSERT_ALWAYS(0 == canasInit(&_canas_instance, &cfg, &_canas_platform));

	// IDS service (it is mandatory for every node):
	CanasSrvIdsPayload ids_selfdescr;
	ids_selfdescr.hardware_revision = MY_HARDWARE_REVISION;
	ids_selfdescr.software_revision = MY_SOFTWARE_REVISION;
	ids_selfdescr.id_distribution   = CANAS_SRV_IDS_ID_DISTRIBUTION_STD;  // These two are standard
	ids_selfdescr.header_type       = CANAS_SRV_IDS_HEADER_TYPE_STD;
	ASSERT_ALWAYS(0 == canasSrvIdsInit(&_canas_instance, &ids_selfdescr, 0));
	*/

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::_handleReceivedParam(CanasMessage* pframe,int subId, long timestamp)
{
	_CanDataLinks* curParm;

	curParm = _listRefs.get[subId];

	// check if new message

	if (pframe->message_code > curParm->last_message_code)
	{
		curParm->last_message_code = pframe->message_code;
		curParm->timestamp = timestamp;
		if (curParm->lastVal != pframe->data.container.FLOAT)
		{
			/// TODO: check type of parameter and use corect cast
			curParm->lastVal = pframe->data.container.FLOAT;
			curParm->indicator->setValue(curParm->lastVal);
		}
	}
	
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::_handleReceivedService(CanasMessage* pframe, int subId, long timestamp)
{
	_CanServiceLinks* curParm;

	curParm = _serviceRefs.get[subId];

	// check if new message

	return (curParm->service->ProcessFrame(pframe));
	
}
//-------------------------------------------------------------------------------------------------------------------
// update canbus check for new input
//-------------------------------------------------------------------------------------------------------------------
int CANaero::Update()
{
	const uint64_t timestamp = _canBus.timeStamp();
	CanasMessage pframe;
	MessageGroup msggroup = MSGGROUP_WTF;
	int ret = 0;
	int curRecord = -1;

	DPRINTLN("START CANaero:update");
	

	if (_canBus.receive(&pframe))
	{
		//CANAS_TRACE("recv %s\n", CANAS_DUMPFRAME(pframe));	
		msggroup = _detectMessageGroup(pframe.id);
		if (msggroup == MSGGROUP_WTF)
		{
			DPRINTLN("update: failed to detect the message group");
			ret = -CANAS_ERR_BAD_MESSAGE_ID;
		}

	}
	if (msggroup == MSGGROUP_PARAMETER)
	{

		DPRINT("CANaero:update record=");
		DPRINTLN(pframe.service_code);

		curRecord = _findInList(pframe.can_id);
		if (curRecord != -1)
		{
			_handleReceivedParam(&pframe, curRecord);			
		}
		else
			DPRINT("foreign param msgid=");
			DPRINT(pframe.service_code );
			DPRINT(" datatype=");
			DPRINTLN(pframe.data.type);
	}
	else if (msggroup == MSGGROUP_SERVICE)
	{
		curRecord = _findServiceInList(pframe.service_code);
		if (curRecord != -1)
		{
			_handleReceivedService(&pframe, curRecord, timestamp);
		}
		
		else
		{
			DPRINT("foreign service msgid=");
			DPRINT(pframe.service_code);
			DPRINT(" datatype=");
			DPRINTLN(pframe.data.type);
		}
	}


	DPRINTLN("STOP CANaero:update");
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamSubscribe(uint16_t msg_id, GenericIndicator* newIndicator)
{
	DPRINTLN("START CANaero:addElement");
	DPRINT("adding:");
	DPRINTLN(msg_id);

	int curRecord = -1;
	_CanDataLinks* newRef;

	DPRINT("RegisterDataRef:START:");

	if (newIndicator == NULL)
		return -CANAS_ERR_ARGUMENT;

	if (_detectMessageGroup(msg_id) != MSGGROUP_PARAMETER)
		return -CANAS_ERR_BAD_MESSAGE_ID;
	
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
		newRef->indicator = newIndicator;
		
		// set filter in canbus to captture this element
		_canBus.setFilter(msg_id, _CallBack);
		/// TODO: send notification to master to start sending data
		
	}
	else
		return -CANAS_ERR_ENTRY_EXISTS;

	DPRINTLN("record processed");

	DPRINTLN("DONE CANaero:ParamSubscribe");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamUnsubscribe(uint16_t msg_id)
{
	CanasCanFrame pframe;
	_CanDataLinks* tmpRef;
	int	listId;

	DPRINTLN("START CANaero:ParamUnsubscribe");
	DPRINT("removing:");
	DPRINTLN(msg_id);

	listId = _findInList(msg_id);
	if (listId == -1)
		return -CANAS_ERR_NO_SUCH_ENTRY;

	/// TODO: send notification to master to stop sending data

	tmpRef = _listRefs[listId];
	_listRefs.clear(listId);
	delete tmpRef;

	DPRINTLN("DONE CANaero:ParamUnsubscribe");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamRead(uint16_t msg_id, float* pargs)
{

	// TODO: code
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// register new service on bus 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ServiceRegister(CANAS_service* newService)
{
	if (newService == NULL)
		return -CANAS_ERR_ARGUMENT;

	int service_code = newService->serviceId;
	
	DPRINTLN("START CANaero:ServiceRegister");
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
		
	DPRINTLN("DONE CANaero:ServiceRegister");
	return 0;
	
}

//-------------------------------------------------------------------------------------------------------------------
// unregister a service
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ServiceUnregister(uint8_t service_code)
{
	DPRINTLN("CANaero:ServiceUnRegister");
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
		DPRINTLN("CANaero:ServiceUnRegister done");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// generic callback for CANbus interupt
//-------------------------------------------------------------------------------------------------------------------
void CANaero::_CallBack(CAN_FRAME * message)
{
	CanasCanFrame pframe;
	CanasDataContainer canData;

	_CanDataLinks* tmpRef;
	int	listId;
	float newValue;

	_canBus.can2areo(&pframe, message);
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
			tmpRef->indicator->setValue(newValue);
		}
	}

}
//-------------------------------------------------------------------------------------------------------------------
// helper function to find a item i the list of  subscribed items
//-------------------------------------------------------------------------------------------------------------------
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
// helper function to find a item i the list of  subscribed items
//-------------------------------------------------------------------------------------------------------------------
int CANaero::_findServiceInList(uint8_t toFind) {

	int curRecord = -1;
	_CanServiceLinks* tmpRef;

	DPRINT("find in list:");
	DPRINTLN(toFind);
	DPRINT("items in list:");
	DPRINTLN(_serviceRefs.size());


	for (int i = 0; i < _serviceRefs.size(); i++) {
		DPRINT("check item:"); DPRINTLN(i);
		tmpRef = _listRefs[i];
		DPRINT("Compaire:");
		DPRINT(toFind);
		DPRINT(":with:");
		DPRINTLN(tmpRef->canServiceCode);
		if (toFind - tmpRef->canServiceCode) {
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
			if (newRef->indicator != NULL)
				DPRINTLN("Call update function");
			newRef->indicator->setValue(value);
		}
	}

	DPRINTLN("CANareo:_updateValue END");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// parse fram and convcer to message and id
//-------------------------------------------------------------------------------------------------------------------
static int _parseFrame(const CanasCanFrame* pframe, uint16_t* pmsg_id, CanasMessage* pmsg)
{
	if (pframe->dlc < 4 || pframe->dlc > 8)
	{
		DPRINT("frameparser: bad dlc=");
		DPRINTLN(pframe->dlc);		
		return -CANAS_ERR_BAD_CAN_FRAME;
	}
	if (pframe->id & CANAS_CAN_FLAG_RTR)
	{
		DPRINTLN("frameparser: RTR flag is not allowed");
		return -CANAS_ERR_BAD_CAN_FRAME;
	}

	*pmsg_id = pframe->id & CANAS_CAN_MASK_STDID;

	memset(pmsg, 0, sizeof(*pmsg));

	pmsg->node_id = pframe->data[0];
	pmsg->data.type = pframe->data[1];
	pmsg->service_code = pframe->data[2];
	pmsg->message_code = pframe->data[3];
	pmsg->data.length = pframe->dlc;
	
	for (int i = 0; i < 4; i++)
		pmsg->data.container.CHAR4[i] = 0;

	for (int i = 0; i < pmsg->data.length; i++)
		pmsg->data.container.CHAR4[i] = pframe->data[i + 4];

	/*
		int ret = canasNetworkToHost(&pmsg->data, pframe->data + 4, pframe->dlc - 4, pframe->data[1]);

	if (ret < 0)
	{
		DPRINT("frameparser: bad data type=");
		DPRINT(pframe->data[1]);
		DPRINT(" error=");
		DPRINTLN(ret);
		return ret;
	}
	*/
	return 0;
}
