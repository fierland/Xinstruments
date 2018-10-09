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

#pragma once

#include "CANaero.h"


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
	CANAS_service(CANaero* CanAsBus, uint8_t serviceID, bool cananswer=true, bool canrequest=true);

	~CANAS_service();

	/**
	* this callback is called when remote node responds to ids request
	* @param node_id  node id of responding node
	* @param ppayload will be null in case of timeout. that is important.
	*/
	int replyToIDCall();

	int serviceId();

	int SendResponse(CanasMessage* msg);
	int SendRequest(CanasMessage* msg);
	int ProcessFrame(CanasMessage* msg);

	/**
	* Sends IDS request to a remote node; response will be returned through callback.
	* @param node_id  Remote Node ID (node to be requested)
	* //@param callback Function to be called when request completed or timed out. Required.
	* @return         @ref CanasErrorCode
	*/
	int requestID(uint8_t nodeId);


protected:
	int			_myServiceId = -1;
	CANaero*	_CanasBus = NULL;	
	bool		_canAnswer;
	bool		_canrequest;

private:
	void CANAS_service::_request(unit8_t msgID);
};
//---------------------------------------------------------------------------------------------------------------------
// service identification class Class definition
//---------------------------------------------------------------------------------------------------------------------
class CANAS_service_identification : CANAS_service
{
public:
	CANAS_service_identification(CANaero* CanAsBus, CanasSrvIdsPayload* payload);
	~CANAS_service_identification();

	int SendResponse(CanasMessage* msg);

private:
	CanasSrvIdsPayload* _myPayload = NULL;
};
//---------------------------------------------------------------------------------------------------------------------
// service get data class Class definition
//---------------------------------------------------------------------------------------------------------------------
class CANAS_service_requestdata : CANAS_service
{
public:
	CANAS_service_requestdata(CANaero* CanAsBus);
	~CANAS_service_requestdata();

	int SendRequest(CanasMessage* msg);

private:
	CanasSrvReqDataPayload* _myPayload = NULL;
}

