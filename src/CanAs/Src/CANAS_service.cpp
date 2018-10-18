//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================

//=========================================================================
//
//
//=========================================================================
//-------------------------------------------------------------------------------------------------------------------
//	 CAN Aerospace class - service subclass
//
//  Franks Flightsim Intruments project
//	by Frank van Ierland
//
// A light implementation of the CAN Aerospace protocol to manage simulated instruments.
//	This code is in the public domain.
//
// Thanks to mjs513/CANaerospace (Pavel Kirienko, 2013 (pavel.kirienko@gmail.com))
//
// ERSION HISTORY:
//
//
//-------------------------------------------------------------------------------------------------------------------
#include "CANAS_service.h"
#include <time.h>
#include <sys/time.h>

#define RANGEINCLUSIVE(x, min, max) ((x) >= (min) && (x) <= (max))
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANAS_service::_serviceChannelFromMessageID(uint16_t msg_id, bool* pisrequest)
{
	DPRINTINFO("START");
	static const uint16_t REQ_MASK = ~(uint16_t)1;
	if (RANGEINCLUSIVE(msg_id, CANAS_MSGTYPE_NODE_SERVICE_HIGH_MIN, CANAS_MSGTYPE_NODE_SERVICE_HIGH_MAX))
	{
		int srvchan = ((msg_id & REQ_MASK) - 128) / 2;
		*pisrequest = !(msg_id & 1);
		return srvchan + CANAS_SERVICE_CHANNEL_HIGH_MIN;
	}
	if (RANGEINCLUSIVE(msg_id, CANAS_MSGTYPE_NODE_SERVICE_LOW_MIN, CANAS_MSGTYPE_NODE_SERVICE_LOW_MAX))
	{
		int srvchan = ((msg_id & REQ_MASK) - 2000) / 2;
		*pisrequest = !(msg_id & 1);
		return srvchan + CANAS_SERVICE_CHANNEL_LOW_MIN;
	}

	DPRINTINFO("STOP");

	return -CANAS_ERR_BAD_MESSAGE_ID;
}

//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANAS_service::serviceChannelToMessageID(uint8_t service_channel, bool isrequest)
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
CANAS_service::CANAS_service(CANaero * CanAsBus, uint8_t serviceID)
{
	DPRINTINFO("START");
	if (CanAsBus == NULL) {
		DPRINTLN("CANAS_service init: bad payload param");
		return;
	};

	_myServiceId = serviceID;
	_CanasBus = CanAsBus;

	DPRINTINFO("STOP");
}

//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
CANAS_service::~CANAS_service()
{
	DPRINTINFO("START");
	_CanasBus->ServiceUnregister(_myServiceId);
	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANAS_service::serviceId()
{
	DPRINTINFO("START");
	DPRINTINFO("STOP");

	return _myServiceId;
}

//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
int CANAS_service::requestID(uint8_t nodeId)
{
	DPRINTINFO("START");
	DPRINTINFO("STOP");

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
bool CANAS_service::canasIsValidServiceChannel(uint8_t service_channel)
{
	return
		RANGEINCLUSIVE(service_channel, CANAS_SERVICE_CHANNEL_HIGH_MIN, CANAS_SERVICE_CHANNEL_HIGH_MAX) ||
		RANGEINCLUSIVE(service_channel, CANAS_SERVICE_CHANNEL_LOW_MIN, CANAS_SERVICE_CHANNEL_LOW_MAX);
}

//===================================================================================================================
// identification service class
//-------------------------------------------------------------------------------------------------------------------

CANAS_service_identification::CANAS_service_identification(CANaero * CanAsBus, CanasSrvIdsPayload * payload) : CANAS_service(CanAsBus, CANAS_SRV_IDENTIFICATION)
{
	DPRINTINFO("START");
	if (payload == NULL)
	{
		DPRINTLN("srv ids init: bad payload param");
		return;
	}
	_myPayload = payload;
	DPRINTINFO("STOP");
}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
CANAS_service_identification::~CANAS_service_identification()
{
	DPRINTINFO("START");
	delete _myPayload;
	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------------------------
// send response to a identification service request`
//-------------------------------------------------------------------------------------------------------------------
int CANAS_service_identification::ProcessFrame(CanasMessage * msg)
{
	DPRINTINFO("START");
	if (msg == NULL)
	{
		DPRINTLN("srv ids response: bad msg param");
		return -CANAS_ERR_ARGUMENT;
	}

	msg->data.type = CANAS_DATATYPE_UCHAR4;
	msg->data.container.UCHAR4[0] = _myPayload->hardware_revision;
	msg->data.container.UCHAR4[1] = _myPayload->software_revision;
	msg->data.container.UCHAR4[2] = _myPayload->id_distribution;
	msg->data.container.UCHAR4[3] = _myPayload->header_type;

	msg->service_code = _myServiceId;

	int ret = _CanasBus->ServiceSendResponse(msg, 0);
	if (ret != 0) {
		DPRINT("srv ids: failed to respond:");
		DPRINTLN(ret);
	}
	DPRINTINFO("STOP");

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// request an identification service and chek if minimum hard/software versions are ok. Store all nodes that reply
//-------------------------------------------------------------------------------------------------------------------
int CANAS_service_identification::Request(CanasMessage * msg)
{
	DPRINTINFO("START");

	msg->message_code = CANAS_SRV_IDENTIFICATION;
	msg->data.type = CANAS_DATATYPE_NODATA;

	msg->service_code = _myServiceId;
	int ret = _CanasBus->ServiceSendRequest(msg, 0);
	if (ret != 0) {
		DPRINT("srv ids: failed to respond:");
		DPRINTLN(ret);
		return ret;
	}
	DPRINTINFO("STOP");

	return 0;
}
//===================================================================================================================
// timestamp service class
//-------------------------------------------------------------------------------------------------------------------

CANAS_service_timestamp::CANAS_service_timestamp(CANaero * CanAsBus) : CANAS_service(CanAsBus, CANAS_SRV_TIMESTAMP)
{
	DPRINTINFO("START");
	DPRINTINFO("STOP");
}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
CANAS_service_timestamp::~CANAS_service_timestamp()
{
	DPRINTINFO("START");
	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------------------------
// request an identification service and chek if minimum hard/software versions are ok. Store all nodes that reply
//-------------------------------------------------------------------------------------------------------------------
int CANAS_service_timestamp::Request(uint64_t timestamp)
{
	CanasMessage msg;
	DPRINTINFO("START");

	msg.data.type = CANAS_DATATYPE_ULONG;
	msg.data.container.ULONG = timestamp;
	msg.node_id = CANAS_BROADCAST_NODE_ID;

	msg.service_code = _myServiceId;
	int ret = _CanasBus->ServiceSendRequest(&msg);

	if (ret != 0) {
		DPRINT("srv ids: failed to request:");
		DPRINTLN(ret);
		return ret;
	}
	DPRINTINFO("STOP");

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// request a time sinc of all modules
//-------------------------------------------------------------------------------------------------------------------
int CANAS_service_timestamp::ProcessFrame(CanasMessage* msg)
{
	DPRINTINFO("START");

	if (msg == NULL)
	{
		DPRINTLN("srv nss req: invalid state pointer");
		return CANAS_ERR_ARGUMENT;
	}

	if (msg->data.type != CANAS_DATATYPE_ULONG)
	{
		DPRINT("srv nss req: wrong data type ");
		DPRINTLN(msg->data.type);
		return -CANAS_ERR_BAD_DATA_TYPE;
	}
	if (msg->message_code != 0)
	{
		DPRINT("srv nss req: wrong message code ");
		DPRINTLN(msg->message_code);
		return -CANAS_ERR_BAD_MESSAGE_ID;
	}
	/// TODO : setr time of device t0 timestamp

	timeval tv;

	tv.tv_sec = msg->data.container.ULONG;
	settimeofday(&tv, NULL);

	DPRINTINFO("STOP");

	return 0;
}
//===================================================================================================================
//
//-------------------------------------------------------------------------------------------------------------------

CANAS_service_requestdata::CANAS_service_requestdata(CANaero * CanAsBus) : CANAS_service(CanAsBus, CANAS_SRV_USR_REUEST_CANDATA)
{
	DPRINTINFO("START");

	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

CANAS_service_requestdata::~CANAS_service_requestdata()
{
	DPRINTINFO("START");

	DPRINTINFO("STOP");
}

//-------------------------------------------------------------------------------------------------------------------
int CANAS_service_requestdata::Request(int dataId, int nodeId)
{
	CanasMessage msg;
	DPRINTINFO("START");

	msg.node_id = nodeId;	// target node for request
	msg.data.type = CANAS_DATATYPE_UCHAR;
	msg.data.container.UCHAR = dataId; // data item to transmit on the bus

	msg.service_code = _myServiceId;

	_CanasBus->ServiceSendRequest(&msg);
	DPRINTINFO("STOP");

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
int CANAS_service_requestdata::ProcessFrame(CanasMessage* msg)
{
	DPRINTINFO("START");

	if (msg == NULL)
	{
		DPRINTLN("srv nss req: invalid state pointer");
		return -CANAS_ERR_ARGUMENT;
	}

	if (msg->data.type != CANAS_DATATYPE_UCHAR)
	{
		DPRINT("srv nss req: wrong data type ");
		DPRINTLN(msg->data.type);
		return -CANAS_ERR_BAD_DATA_TYPE;
	}
	if (msg->message_code != _myServiceId)
	{
		DPRINT("srv nss req: wrong message code ");
		DPRINTLN(msg->message_code);
		return -CANAS_ERR_BAD_MESSAGE_ID;
	}

	// start advertisement of specific canas data element.
	_CanasBus->ParamAdvertise(msg->data.container.UCHAR);

	/// TODO: add code
	DPRINTINFO("STOP");
	return (0);
}