//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================

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
#ifndef _CANAS_SERVICE_H_
#define _CANAS_SERVICE_H_

#include "CANaero.h"
#include <QList.h>

typedef enum
{
	CANAS_SRV_IDS_ID_DISTRIBUTION_STD = 0
} CanasSrvIdsIdDistribution;

typedef enum
{
	CANAS_SRV_IDS_HEADER_TYPE_STD = 0
} CanasSrvIdsHeaderType;

typedef enum
{
	CANAS_SRV_IDENTIFICATION = 0,
	CANAS_SRV_TIMESTAMP = 1,
	CANAS_SRV_USR_REUEST_CANDATA = 100
} CanasSrvCodes;

/**
 * Data to be transferred by the identification service
 */
typedef struct
{
	uint8_t hardware_revision;
	uint8_t software_revision;
	uint8_t id_distribution;
	uint8_t header_type;
} CanasSrvIdsPayload;

/**
 * Data to be transferred by the request data service
 */
typedef struct
{
	int dataId;
} CanasSrvReqDataPayload;
//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
class CANAS_service
{
public:

	/**
	* Initializes standard Identification Service.
	* Note that this service must be available on every CANaerospace node (as required by specification),
	* so you should not left it uninitialized.
	* Also keep in mind that outgoing broadcast request creates CANAS_MAX_NODES pending requests, so you must
	* provide adequate number of pending request slots; see the corresponding parameter @ref max_pending_requests.
	* @param CanAsBus             pointer to canasbus object
	* @param newPayload			  Description of the local node
	*/
	CANAS_service(CANaero* CanAsBus, uint8_t serviceID);

	~CANAS_service();

	/**
	* this callback is called when remote node responds to ids request
	* @param node_id  node id of responding node
	* @param ppayload will be null in case of timeout. that is important.
	*/
	//int replyToIDCall();

	int serviceId();

	//virtual int Response(CanasMessage* msg) = 0;
//	virtual int Request(CanasMessage* msg) = 0; // is implemented in child classes
	virtual int ProcessFrame(CanasMessage* msg) = 0;

	/**
	* Sends IDS request to a remote node; response will be returned through callback.
	* @param node_id  Remote Node ID (node to be requested)
	* //@param callback Function to be called when request completed or timed out. Required.
	* @return         @ref CanasErrorCode
	*/
	int requestID(uint8_t nodeId);

	static bool canasIsValidServiceChannel(uint8_t service_channel);

	static int serviceChannelToMessageID(uint8_t service_channel, bool isrequest);

protected:
	int			_myServiceId = -1;
	CANaero*	_CanasBus = NULL;
	bool		_canAnswer;
	bool		_canrequest;

private:
	static int _serviceChannelFromMessageID(uint16_t msg_id, bool* pisrequest);
};
//---------------------------------------------------------------------------------------------------------------------
// service identification class Class definition
//---------------------------------------------------------------------------------------------------------------------
class CANAS_service_identification : public CANAS_service
{
public:
	CANAS_service_identification(CANaero* CanAsBus, CanasSrvIdsPayload* payload);
	~CANAS_service_identification();

	int ProcessFrame(CanasMessage* msg);
	int Request(CanasMessage* msg);

private:
	CanasSrvIdsPayload* _myPayload = NULL;
};
//---------------------------------------------------------------------------------------------------------------------
// service timestamp class Class definition
//---------------------------------------------------------------------------------------------------------------------
class CANAS_service_timestamp : public CANAS_service
{
public:
	CANAS_service_timestamp(CANaero* CanAsBus);
	~CANAS_service_timestamp();

	int ProcessFrame(CanasMessage* msg);
	int Request(uint64_t timestamp);
};
//---------------------------------------------------------------------------------------------------------------------
// service get data class Class definition
//---------------------------------------------------------------------------------------------------------------------
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
class CANAS_service_requestdata : public CANAS_service
{
public:
	CANAS_service_requestdata(CANaero* CanAsBus);
	~CANAS_service_requestdata();

	int ProcessFrame(CanasMessage* msg);
	int Request(int dataId, int nodeId, bool newId = true);
private:

	struct _subscribedNode
	{
		uint8_t		nodeId;
		_subscribedNode* next = NULL;
	};
	struct _dataRequest
	{
		uint8_t		canId = 0;
		int			subscriptions = 0;
		_subscribedNode* firstNode = NULL;
		_subscribedNode* lastNode = NULL;
	};
	QList < _dataRequest*> _dataReqRefs;

	int _findInList(uint8_t toFind);
	int _findNode(_dataRequest* tmpRef, uint8_t nodeId, bool doDelete = false);
};

#endif
