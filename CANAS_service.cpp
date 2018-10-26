//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================
//	 CAN Aerospace class - service subclass
//
// A light implementation of the CAN Aerospace protocol to manage simulated instruments.
//	This code is in the public domain.
//
// Thanks to mjs513/CANaerospace (Pavel Kirienko, 2013 (pavel.kirienko@gmail.com))
//
// VERSION HISTORY:
//
//
//==================================================================================================

#include "CANAS_service.h"
#include <time.h>
#include <sys/time.h>
#include "mydebug.h"

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
CANAS_service::CANAS_service(CANaeroBase* CanAsBus, uint8_t serviceID)
{
	DPRINTINFO("START");
	/*
	if (CanAsBus == NULL)
	{
		DPRINTLN("CANAS_service init: bad payload param");
		return;
	};
	*/
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
	//_CanasBus->ServiceUnregister(_myServiceId);
	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------------------
uint8_t CANAS_service::serviceId()
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

//===================================================================================================================
// identification service class
//-------------------------------------------------------------------------------------------------------------------

CANAS_service_identification::CANAS_service_identification(CANaeroBase * CanAsBus, CanasSrvIdsPayload * payload) : CANAS_service(CanAsBus, CANAS_SRV_IDENTIFICATION)
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
	if (ret != 0)
	{
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
	if (ret != 0)
	{
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

CANAS_service_timestamp::CANAS_service_timestamp(CANaeroBase * CanAsBus) : CANAS_service(CanAsBus, CANAS_SRV_TIMESTAMP)
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
	int ret = _CanasBus->ServiceSendRequest(&msg, CANAS_SRV_TIMESTAMP);

	if (ret != 0)
	{
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
// request to the main controler to stat providing a canareeospace data item on the bus
//
// Message	| datafield		|	service		| service
// DataByte	| description	|	request		|response
//--------------------------------------------------------------
//		0	| Node-ID		|	<node-ID>	| <node-ID>
//		1	| Data Type		|	UCHAR		|	NODATA
//		2	| Service code	|	100			|	100
//		3	| Message Code	|	0 = End		|	0 = success
//			|				|	1 = New		|	1 = error
//		4-7	| Message Data	|	Canas ID	|	n.a.
//-------------------------------------------------------------------------------------------------------------------

CANAS_service_requestdata::CANAS_service_requestdata(CANaeroBase * CanAsBus) : CANAS_service(CanAsBus, CANAS_SRV_USR_REUEST_CANDATA)
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
int CANAS_service_requestdata::Request(int dataId, int nodeId, bool newId)
{
	CanasMessage msg;
	DPRINTINFO("START");

	msg.node_id = nodeId;	// target node for request
	msg.data.type = CANAS_DATATYPE_UCHAR;
	msg.data.container.UCHAR = dataId; // data item to transmit on the bus

	msg.service_code = _myServiceId;
	msg.message_code = (newId ? 1 : 0);

	_CanasBus->ServiceSendRequest(&msg, CANAS_SRV_USR_REUEST_CANDATA);

	DPRINTINFO("STOP");

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// helper function to find a item i the list of  subscribed items
//-------------------------------------------------------------------------------------------------------------------
int CANAS_service_requestdata::_findInList(uint8_t toFind)
{
	int curRecord = -1;
	_dataRequest* tmpRef;

	DPRINTINFO("START");

	DPRINT("find in list:");
	DPRINTLN(toFind);
	DPRINT("items in list:");
	//DPRINTLN(_dataReqRefs.size());

	for (int i = 0; i < _dataReqRefs.size(); i++)
	{
		DPRINT("check item:"); DPRINTLN(i);
		tmpRef = _dataReqRefs[i];
		DPRINT("Compaire:");
		DPRINT(toFind);
		DPRINT(":with:");
		DPRINTLN(tmpRef->canId);
		if (toFind - tmpRef->canId)
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
int CANAS_service_requestdata::_findNode(_dataRequest* myStruct, uint8_t nodeId, bool doDelete)
{
	int i = 0;
	_subscribedNode* curNode;
	_subscribedNode* prevNode;
	int result = 0;

	if (myStruct->subscriptions == 0 || myStruct->firstNode == NULL)
		result = -CANAS_ERR_NO_SUCH_ENTRY;

	curNode = myStruct->firstNode;
	prevNode = NULL;

	while (curNode != NULL)
	{
		if (curNode->nodeId == nodeId)
			return 0;
		prevNode = curNode;
		curNode = curNode->next;
	}

	if (curNode != NULL)
	{
		if (doDelete)
		{
			prevNode->next = curNode->next;
			delete curNode;
		}
	}
	else
	{
		result = -CANAS_ERR_BAD_NODE_ID;
	}

	return result;
}
//-------------------------------------------------------------------------------------------------------------------
// TODO: support multiple request for same item
//-------------------------------------------------
int CANAS_service_requestdata::ProcessFrame(CanasMessage* msg)
{
	int result = 0;
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

	uint8_t newCanId = (uint8_t)msg->data.container.UCHAR;
	_dataRequest* tmpRef;
	_subscribedNode* newNode;

	//  check if we already provide this item ?
	int item = _findInList(newCanId);

	if (msg->message_code == 0)
	{
		// request to remove an Item
		if (item == 0)
		{
			// not found so can not remove
			result = -CANAS_ERR_NO_SUCH_ENTRY;
		}
		else
		{
			// found so remove item in list
			tmpRef = _dataReqRefs.at(item);
			if (tmpRef->subscriptions > 1)
			{
				if (_findNode(tmpRef, msg->node_id, true))
					tmpRef->subscriptions--;
				else
					result = -CANAS_ERR_BAD_NODE_ID;
			}
			else
			{
				_dataReqRefs.clear(item);
				result = _CanasBus->ParamUnadvertise(newCanId);
			}
		}
	}
	else
	{
		// request to add an item
		if (item != 0)
		{
			// found so update current item with this request
			tmpRef = _dataReqRefs.at(item);

			if (_findNode(tmpRef, msg->node_id))
			{
				result = -CANAS_ERR_ENTRY_EXISTS;
			}
			else
			{
				tmpRef->subscriptions++;
				newNode = new _subscribedNode;
				newNode->nodeId = msg->node_id;
				tmpRef->lastNode->next = newNode;
				tmpRef->lastNode = newNode;
			}
		}
		else
		{
			// not found so add item in list
			tmpRef = new _dataRequest;
			tmpRef->canId = newCanId;
			newNode = new _subscribedNode;
			newNode->nodeId = msg->node_id;
			tmpRef->firstNode = newNode;
			tmpRef->lastNode = newNode;
			tmpRef->subscriptions = 1;
			_dataReqRefs.push_back(tmpRef);

			result = _CanasBus->ParamAdvertise(newCanId);
		}
	}
	// return result of action
	msg->message_code = result;
	msg->data.type = CANAS_DATATYPE_NODATA;
	msg->can_id = _CanasBus->getNodeId();

	_CanasBus->ServiceSendResponse(msg, CANAS_SRV_USR_REUEST_CANDATA);

	DPRINTINFO("STOP");
	return result;
}