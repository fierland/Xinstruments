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

//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
CANAS_service::CANAS_service(CANaero * CanAsBus, uint8_t serviceID, bool cananswer, bool canrequest)
{	
	if (CanAsBus == NULL )
		return;

	_myServiceId = serviceID;
	_CanasBus	= CanAsBus;
	_canAnswer	= cananswer;
	_canrequest = canrequest;

	if (cananswer)
	{
		_CanasBus->ServiceRegister(self);
		// register service ...
	}
}

//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
CANAS_service::~CANAS_service()
{
	if (_canAnswer)
	{
		_CanasBus->ServiceUnregister(_myServiceId);
	}
	/*
	    int ret = canasServiceRegister(pi, THIS_SERVICE_CODE, _poll, _request, _response, pd);
	*/
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANAS_service::replyToIDCall()
{

}
int CANAS_service::serviceId()
{
	return _myServiceId;
}
int CANAS_service::SendResponse(CanasMessage * msg)
{
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANAS_service::requestID(uint8_t nodeId)
{
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
static void CANAS_service::_request(unit8_t msgID)
{
	CanasMessage msg;

	if (_myPayload == NULL)
	{
		DPRINTLN("srv ids req: invalid state pointer\n");
		return;
	}
		
	msg.data.type = CANAS_DATATYPE_UCHAR4;
	msg.data.container.UCHAR4[0] = _myPayload.hardware_revision;
	msg.data.container.UCHAR4[1] = _myPayload.software_revision;
	msg.data.container.UCHAR4[2] = _myPayload.id_distribution;
	msg.data.container.UCHAR4[3] = _myPayload.header_type;

	int ret = canasServiceSendResponse(&msg);
	if (ret != 0) {
		DPRINT("srv ids: failed to respond:");
		DPRINTLN(ret);
	}
}
//===================================================================================================================
// 
//-------------------------------------------------------------------------------------------------------------------

CANAS_service_identification::CANAS_service_identification(CANaero * CanAsBus, CanasSrvIdsPayload * payload) : CANAS_service(CanAsBus, CANAS_SRV_IDENTIFICATION,true,false)
{
	_myPayload = payload;
}

CANAS_service_identification::~CANAS_service_identification()
{
}
int CANAS_service_identification::SendResponse(CanasMessage * msg)
{
	return 0;
}
//===================================================================================================================
// 
//-------------------------------------------------------------------------------------------------------------------

CANAS_service_requestdata::CANAS_service_requestdata(CANaero * CanAsBus) : CANAS_service(CanAsBus, CANAS_SRV_USR_REUEST_CANDATA, false,true)
{
	//

}

CANAS_service_requestdata::~CANAS_service_requestdata()
{
}

int CANAS_service_requestdata::SendRequest(CanasMessage * msg)
{
	return 0;
}
