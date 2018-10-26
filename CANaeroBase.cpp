//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================
//
//	 CAN Aerospace base class
//
//  Franks Flightsim Intruments project
//	by Frank van Ierland
//
// A light implementation of the CAN Aerospace protocol to manage simulated instruments.
//	This code is in the public domain.
//
//	VERSION HISTORY:
//
//==================================================================================================
#include "CANaeroBase.h"
#include "mydebug.h"

#define RANGEINCLUSIVE(x, min, max) ((x) >= (min) && (x) <= (max))
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
bool CANaeroBase::canasIsValidServiceChannel(uint8_t service_channel)
{
	return
		RANGEINCLUSIVE(service_channel, CANAS_SERVICE_CHANNEL_HIGH_MIN, CANAS_SERVICE_CHANNEL_HIGH_MAX) ||
		RANGEINCLUSIVE(service_channel, CANAS_SERVICE_CHANNEL_LOW_MIN, CANAS_SERVICE_CHANNEL_LOW_MAX);
}
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
MessageGroup CANaeroBase::_detectMessageGroup(uint16_t id)
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
//
//-------------------------------------------------------------------------------------------------------------------
int CANaeroBase::serviceChannelToMessageID(uint8_t service_channel, bool isrequest)
{
	DPRINTINFO("START");
	if (RANGEINCLUSIVE(service_channel, CANAS_SERVICE_CHANNEL_HIGH_MIN, CANAS_SERVICE_CHANNEL_HIGH_MAX))
	{
		int ret = 128 + service_channel * 2;
		if (!isrequest)
			ret++;
		return ret;
	}
	if (RANGEINCLUSIVE(service_channel, CANAS_SERVICE_CHANNEL_LOW_MIN, CANAS_SERVICE_CHANNEL_LOW_MAX))
	{
		service_channel -= CANAS_SERVICE_CHANNEL_LOW_MIN;
		int ret = 2000 + service_channel * 2;
		if (!isrequest)
			ret++;
		return ret;
	}

	DPRINTINFO("STOP");

	return -CANAS_ERR_BAD_SERVICE_CHAN;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
CANaeroBase::CANaeroBase()
{}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
CANaeroBase::~CANaeroBase()
{}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANaeroBase::ServiceSendRequest(const CanasMessage* pmsg, uint8_t service_channel)
{
	DPRINTINFO("START");
	if (pmsg == NULL)
		return -CANAS_ERR_ARGUMENT;

	if (pmsg->node_id == _node_id)  // Self-addressed requests are ridiculous
		return -CANAS_ERR_BAD_NODE_ID;

	int msg_id = serviceChannelToMessageID(service_channel, true);
	if (msg_id < 0)
		return msg_id;
	return _genericSend((uint16_t)msg_id, MSGGROUP_SERVICE, pmsg);

	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANaeroBase::ServiceSendResponse(const CanasMessage* pmsg, uint8_t service_channel)
{
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

	int msg_id = serviceChannelToMessageID(service_channel, false); // Also will check validity of service_channel
	if (msg_id < 0)
		return msg_id;
	return _genericSend((uint16_t)msg_id, MSGGROUP_SERVICE, &msg);

	DPRINTINFO("STOP");
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
int CANaeroBase::setServiceChannel(uint8_t newChannel)
{
	DPRINTINFO("START");

	if (canasIsValidServiceChannel(newChannel) != 0)
		return -CANAS_ERR_BAD_SERVICE_CHAN;

	_service_channel = newChannel;

	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
int CANaeroBase::start()
{
	DPRINTINFO("START");
	_running = true;
	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
int CANaeroBase::stop()
{
	DPRINTINFO("START");
	_running = false;
	DPRINTINFO("STOP");
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// set the id for this node to use
//-------------------------------------------------------------------------------------------------------------------
int CANaeroBase::setNodeId(uint8_t newID)
{
	DPRINTINFO("START");
	_node_id = newID;

	DPRINTINFO("STOP");
	return 0;
}
uint8_t CANaeroBase::getNodeId()
{
	return _node_id;
}

//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANaeroBase::_canasDataCopy(CanasMessageData* pdataTo, const CanasMessageData* pdataFrom)
{
	// TODO: check for null pointers
	pdataTo->length = pdataFrom->length;
	pdataTo->type = pdataFrom->type;
	for (int i = 0; i < 4; i++)
	{
		pdataTo->container.CHAR4[i] = pdataFrom->container.CHAR4[i];
	};
};

//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
char * CANaeroBase::DumpCanFrame(const CanasCanFrame * pframe, char * pbuf)
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

char * CANaeroBase::DumpMessage(const CanasMessage * pmsg, char * pbuf)
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

uint64_t CANaeroBase::Timestamp()
{
	return _canBus.timeStamp();
}

//-------------------------------------------------------------------------------------------------------------------
// parse fram and convcer to message and id
//-------------------------------------------------------------------------------------------------------------------
int CANaeroBase::_parseFrame(const CanasCanFrame* pframe, uint16_t* pmsg_id, CanasMessage* pmsg)
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
int CANaeroBase::_genericSend(uint16_t msg_id, uint8_t msggroup, const CanasMessage* pmsg)
{
	CanasCanFrame frame;
	int mkframe_result = -1;

	DPRINTINFO("START");

	mkframe_result = CANdriver::msg2frame(&frame, msg_id, pmsg);

	if (mkframe_result != 0)
		return mkframe_result;

	DPRINT("sending:");
	DPRINTLN(CANAS_DUMPFRAME(&frame));

	bool sent_successfully = false;

	const int send_result = _canBus.writeMsg(&frame);
	if (send_result == 1)
		sent_successfully = true;            // At least one successful sending is enough to return success.
	else
	{
		DPRINTLN("send failed: result=");
		DPRINTLN(send_result);
	}

	DPRINTINFO("STOP");
	return sent_successfully ? 0 : -CANAS_ERR_DRIVER;
}
//-------------------------------------------------------------------------------------------------------------------
// helper function to find a item i the list of  Published items
//-------------------------------------------------------------------------------------------------------------------
int CANaeroBase::_findPublishedInList(uint8_t toFind)
{
	int curRecord = -1;
	_CanPublishLinks* tmpRef;

	DPRINTINFO("START");

	DPRINT("find in list:");
	DPRINTLN(toFind);
	DPRINT("items in list:");
	DPRINTLN(_publishRefs.size());

	int rsize = _publishRefs.size();
	for (int i = 0; i < rsize; i++)
	{
		DPRINT("check item:"); DPRINTLN(i);
		tmpRef = _publishRefs.at(i);
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
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANaeroBase::ParamAdvertise(uint16_t msg_id, uint16_t intervalMs)
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

	if (curRecord == -1)
	{
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
int CANaeroBase::ParamUnadvertise(uint16_t msg_id)
{
	int curRecord = -1;
	DPRINTINFO("START");

	// check if we know this param id
	curRecord = _findPublishedInList(msg_id);
	if (curRecord == -1)
	{
		return -CANAS_ERR_NO_SUCH_ENTRY;
	}
	//not found so create new item
	// TODO: code ParamUnadvertise
	DPRINTINFO("STOP");
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANaeroBase::ParamPublish(uint16_t msg_id, const CanasMessageData * pdata)
{
	int curRecord = -1;
	CanasMessage msg;

	DPRINTINFO("START");

	// check we have advertised this one
	curRecord = _findPublishedInList(msg_id);
	if (curRecord == -1)
	{
		return -CANAS_ERR_NO_SUCH_ENTRY;
	}

	// send message on bus
	_canasDataCopy(&(msg.data), pdata);

	msg.message_code = _publishRefs[curRecord]->last_message_code++;
	msg.node_id = _node_id;
	msg.can_id = msg_id;
	msg.service_code = 0;
	_publishRefs[curRecord]->timestamp = Timestamp();

	return _genericSend((uint16_t)msg_id, MSGGROUP_PARAMETER, &msg);

	// TODO: code ParamPublish
	DPRINTINFO("STOP");
	return 0;
}