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
#include "CANdriver.h"

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
CANaero::CANaero(uint8_t nodeId, uint8_t serviceChannel, uint8_t hdwId, uint8_t swId)
{
	CanasSrvIdsPayload canasCfg;

	DPRINTINFO("START");

	_node_id = nodeId;
	_service_channel = serviceChannel; // for now set service channel to node id

	canasCfg.hardware_revision = hdwId;
	canasCfg.software_revision = swId;
	canasCfg.header_type = CANAS_SRV_IDS_ID_DISTRIBUTION_STD;
	canasCfg.id_distribution = CANAS_SRV_IDS_HEADER_TYPE_STD;

	_identService = new CANAS_service_identification(this, &canasCfg);
	_requestService = new CANAS_service_requestdata(this);
	_timeStampService = new CANAS_service_timestamp(this);

	ServiceRegister(_identService);		// register service ...
	ServiceRegister(_timeStampService);

	DPRINTINFO("STOP");
}

//-------------------------------------------------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------------------------------------------------

CANaero::~CANaero()
{
	DPRINTINFO("START");

	delete _requestService;
	delete _identService;
	delete _timeStampService;

	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
int CANaero::setServiceChannel(uint8_t newChannel)
{
	DPRINTINFO("START");

	if (CANAS_service::canasIsValidServiceChannel(newChannel) != 0)
		return -CANAS_ERR_BAD_SERVICE_CHAN;

	_service_channel = newChannel;

	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
int CANaero::start()
{
	DPRINTINFO("START");
	_running = true;
	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
int CANaero::stop()
{
	DPRINTINFO("START");
	_running = false;
	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// set the id for this node to use
//-------------------------------------------------------------------------------------------------------------------
int CANaero::setNodeId(uint8_t newID)
{
	DPRINTINFO("START");
	_node_id = newID;

	DPRINTINFO("STOP");
	return 0;
}
uint8_t CANaero::getNodeId()
{
	return _node_id;
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
int CANaero::_handleReceivedParam(CanasMessage* pframe, int subId, long timestamp)
{
	_CanDataLinks* curParm;
	DPRINTINFO("START");

	curParm = _listRefs.get[subId];

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
int CANaero::_handleReceivedService(CanasMessage* pframe, int subId, long timestamp)
{
	_CanServiceLinks* curParm;
	DPRINTINFO("START");

	curParm = _serviceRefs.get[subId];

	// check if new message

	DPRINTINFO("STOP");
	return (curParm->service->ProcessFrame(pframe));
}
//-------------------------------------------------------------------------------------------------------------------
// update canbus check for new input
//-------------------------------------------------------------------------------------------------------------------
int CANaero::Update()
{
	const uint64_t timestamp = _canBus.timeStamp();
	CanasCanFrame frame;
	CanasMessage msg;
	MessageGroup msggroup = MSGGROUP_WTF;
	int ret = 0;
	int curRecord = -1;

	DPRINTINFO("START");

	if (!_running)
		return -CANAS_ERR_NOT_RUNNING;

	if (!_canBus.receive(&frame))
	{
		DPRINTINFO("STOP");
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

		curRecord = _findInList(msg.can_id);
		if (curRecord != -1)
		{
			_handleReceivedParam(&msg, curRecord);
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

	DPRINTINFO("STOP");
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamSubscribe(uint16_t msg_id, GenericIndicator* newIndicator)
{
	DPRINTINFO("START");
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
		newRef->canAreoId = msg_id;
		DPRINTLN("type added");
		newRef->indicator = newIndicator;

		// set filter in canbus to capture this element
		_canBus.setFilter(msg_id, _CallBack);
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
int CANaero::ParamUnsubscribe(uint16_t msg_id)
{
	CanasCanFrame pframe;
	_CanDataLinks* tmpRef;
	int	listId;
	DPRINTINFO("START");

	DPRINT("removing:");
	DPRINTLN(msg_id);

	listId = _findInList(msg_id);
	if (listId == -1)
		return -CANAS_ERR_NO_SUCH_ENTRY;

	/// TODO: send notification to master to stop sending data

	tmpRef = _listRefs[listId];
	_listRefs.clear(listId);
	delete tmpRef;

	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamRead(uint16_t msg_id, float* pargs)
{
	DPRINTINFO("START");

	// TODO: code
	DPRINTINFO("STOP");

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamAdvertise(uint16_t msg_id, uint16_t intervalMs)
{
	int curRecord = -1;
	_CanPublishLinks* newRef;
	MessageGroup msggroup = MSGGROUP_WTF;

	DPRINTINFO("START");
	/// TODO: find in array
	DPRINT("adding:");
	DPRINTLN(msg_id);

	// check if valid record
	if (_detectMessageGroup(msg_id) != MSGGROUP_PARAMETER)
	{
		DPRINTLN("update: failed to detect the message group");
		return -CANAS_ERR_BAD_MESSAGE_ID;
	}

	// check if we know this param id
	curRecord = _findPublishedInList(msg_id);

	if (curRecord == -1) {
		//not found so create new item
		DPRINTLN("New record");
		newRef = new _CanPublishLinks;
		DPRINTLN("added object");
		_publishRefs.push_back(newRef);
		DPRINTLN("in list");
		newRef->linkId = _publishRefs.size();
		DPRINTLN("id adeded");
		newRef->canAreoId = msg_id;
		DPRINTLN("type added");
		newRef->maxIntervalMs = intervalMs;
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
int CANaero::ParamUnadvertise(uint16_t msg_id)
{
	DPRINTINFO("START");

	// TODO: code ParamUnadvertise
	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamPublish(uint16_t msg_id, const CanasMessageData * pdata, uint8_t service_code)
{
	DPRINTINFO("START");

	// TODO: code ParamPublish
	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// register new service on bus
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ServiceRegister(CANAS_service* newService)
{
	DPRINTINFO("START");

	if (newService == NULL)
		return -CANAS_ERR_ARGUMENT;

	int service_code = newService->serviceId;

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
int CANaero::ServiceRegister_requestData(void(*newService)(uint8_t canasID), void(*removeService)(uint8_t canasID))
{
	_newXservicecall = newService;
	_removeXservicecall = removeService;

	return ServiceRegister(_requestService);
}

//-------------------------------------------------------------------------------------------------------------------
// unregister a service
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ServiceUnregister(uint8_t service_code)
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
int CANaero::ServiceSendRequest(const CanasMessage* pmsg, uint8_t service_channel) {
	DPRINTINFO("START");
	if (pmsg == NULL)
		return -CANAS_ERR_ARGUMENT;

	if (pmsg->node_id == _node_id)  // Self-addressed requests are ridiculous
		return -CANAS_ERR_BAD_NODE_ID;

	int msg_id = CANAS_service::serviceChannelToMessageID(service_channel, true);
	if (msg_id < 0)
		return msg_id;
	return _genericSend((uint16_t)msg_id, MSGGROUP_SERVICE, pmsg);

	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ServiceSendResponse(const CanasMessage* pmsg, uint8_t service_channel) {
	DPRINTINFO("START");
	if (pmsg == NULL)
		return -CANAS_ERR_ARGUMENT;

	CanasMessage msg = *pmsg;
	if (msg.node_id == CANAS_BROADCAST_NODE_ID)
	{
		DPRINTLN("srv response to broadcast request");
		msg.node_id = _node_id;   // Silently correct the Node ID for responses to global requests
	}

	if (msg.node_id != _node_id)  // Usage of foreign Node ID in response is against specification.
		return -CANAS_ERR_BAD_NODE_ID;

	int msg_id = CANAS_service::serviceChannelToMessageID(service_channel, false); // Also will check validity of service_channel
	if (msg_id < 0)
		return msg_id;
	return _genericSend((uint16_t)msg_id, MSGGROUP_SERVICE, &msg);

	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
char * CANaero::DumpCanFrame(const CanasCanFrame * pframe, char * pbuf)
{
	static const int ASCII_COLUMN_OFFSET = 34;

	char* wpos = pbuf, *epos = pbuf + CANAS_DUMP_BUF_LEN;
	memset(pbuf, 0, CANAS_DUMP_BUF_LEN);

	if (pframe->id & CANAS_CAN_FLAG_EFF)
		wpos += snprintf(wpos, epos - wpos, "%08x  ", (unsigned int)(pframe->id & CANAS_CAN_MASK_EXTID));
	else
		wpos += snprintf(wpos, epos - wpos, "     %03x  ", (unsigned int)(pframe->id & CANAS_CAN_MASK_STDID));

	if (pframe->id & CANAS_CAN_FLAG_RTR)
	{
		wpos += snprintf(wpos, epos - wpos, " RTR");
	}
	else
	{
		for (int dlen = 0; dlen < pframe->dlc; dlen++)                         // hex bytes
			wpos += snprintf(wpos, epos - wpos, " %02x", pframe->data[dlen]);

		while (wpos < pbuf + ASCII_COLUMN_OFFSET)                              // alignment
			*wpos++ = ' ';

		wpos += snprintf(wpos, epos - wpos, "  \'");                           // ascii
		for (int dlen = 0; dlen < pframe->dlc; dlen++)
		{
			uint8_t ch = pframe->data[dlen];
			if (ch < 0x20 || ch > 0x7E)
				ch = '.';
			wpos += snprintf(wpos, epos - wpos, "%c", ch);
		}
		wpos += snprintf(wpos, epos - wpos, "\'");
	}
	return pbuf;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------

char * CANaero::DumpMessage(const CanasMessage * pmsg, char * pbuf)
{
	if (pbuf != NULL)
	{
		snprintf(pbuf, CANAS_DUMP_BUF_LEN, "N%02x D%02x S%02x M%02x   [%02x %02x %02x %02x]",
			// header
			(unsigned int)pmsg->node_id,
			(unsigned int)pmsg->data.type,
			(unsigned int)pmsg->service_code,
			(unsigned int)pmsg->message_code,
			// payload
			(unsigned int)pmsg->data.container.UCHAR4[0],
			(unsigned int)pmsg->data.container.UCHAR4[1],
			(unsigned int)pmsg->data.container.UCHAR4[2],
			(unsigned int)pmsg->data.container.UCHAR4[3]);
	}
	return pbuf;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------

uint64_t CANaero::Timestamp()
{
	return _canBus.timeStamp();
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
void CANaero::_CallBack(CAN_FRAME * message)
{
	CanasCanFrame pframe;
	CanasDataContainer canData;

	DPRINTINFO("START");

	_CanDataLinks* tmpRef;
	int	listId;
	float newValue;

	_canBus.can2areo(&pframe, message);
	listId = _findInList(pframe.id);

	if (listId != -1)
	{
		tmpRef = _listRefs[listId];
		for (int i = 0; i < 4; i++)
			canData.ACHAR4[i] = pframe.data[i + 4];

		newValue = (float)canData.FLOAT;

		if (tmpRef->lastVal != newValue)
		{
			tmpRef->lastVal = newValue;
			tmpRef->timestamp = _canBus.timeStamp();
			tmpRef->indicator->setValue(newValue);
		}
	}

	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------------------------
// helper function to find a item i the list of  subscribed items
//-------------------------------------------------------------------------------------------------------------------
int CANaero::_findInList(uint8_t toFind) {
	int curRecord = -1;
	_CanDataLinks* tmpRef;

	DPRINTINFO("START");

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

	DPRINTINFO("STOP");
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

	DPRINTINFO("START");

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

	DPRINTINFO("STOP");

	DPRINT("find in list done:");
	DPRINTLN(curRecord);

	return curRecord;
}
//-------------------------------------------------------------------------------------------------------------------
// helper function to find a item i the list of  Published items
//-------------------------------------------------------------------------------------------------------------------
int CANaero::_findPublishedInList(uint8_t toFind)
{
	int curRecord = -1;
	_CanPublishLinks* tmpRef;

	DPRINTINFO("START");

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
		DPRINTLN(tmpRef->canAreoId);
		if (toFind - tmpRef->canAreoId) {
			curRecord = i;
			break;
		}
	}

	DPRINTINFO("STOP");

	DPRINT("find in list done:");
	DPRINTLN(curRecord);

	return curRecord;
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// internal call if new value from master then update n internal array and call to update physical instrument.
//-------------------------------------------------------------------------------------------------------------------

int CANaero::_updateValue(uint8_t type, float value)
{
	int curRecord = -1;
	_CanDataLinks* newRef;

	DPRINTINFO("START");

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

	DPRINTINFO("STOP");

	return 0;
};
//-------------------------------------------------------------------------------------------------------------------
// parse fram and convcer to message and id
//-------------------------------------------------------------------------------------------------------------------
int CANaero::_parseFrame(const CanasCanFrame* pframe, uint16_t* pmsg_id, CanasMessage* pmsg)
{
	DPRINTINFO("START");

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

	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// generic function to send a message on canbus.
//-------------------------------------------------------------------------------------------------------------------
int CANaero::_genericSend(uint16_t msg_id, uint8_t msggroup, const CanasMessage* pmsg)
{
	CanasCanFrame frame;
	int mkframe_result = -1;

	DPRINTINFO("START");

	mkframe_result = _makeFrame(&frame, msg_id, pmsg);

	if (mkframe_result != 0)
		return mkframe_result;

	DPRINT("sending:");
	DPRINTLN(CANAS_DUMPFRAME(&frame));

	bool sent_successfully = false;

	const int send_result = _canBus.writeMsg(&frame);
	if (send_result == 1)
		sent_successfully = true;            // At least one successful sending is enough to return success.
	else {
		DPRINTLN("send failed: result=");
		DPRINTLN(send_result);
	}

	DPRINTINFO("STOP");
	return sent_successfully ? 0 : -CANAS_ERR_DRIVER;
}