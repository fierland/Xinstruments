//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================
/// <summary>
/// CAN Aerospace class
///
/// A light implementation of the CAN Aerospace protocol to manage simulated instruments.
///
/// Thanks to mjs513/CANaerospace (Pavel Kirienko, 2013 (pavel.kirienko@gmail.com))
///
/// VERSION HISTORY:
///
///</summary>
///<remark>
/// Current limitations:
///	- only one sender per data item allowed no support for redundant devices yet
///	- only one canbus allowed no support for multiples redundant busses.
///</remark>
//==================================================================================================
#pragma once

#ifndef _CANAEROINF_H_
#define _CANAEROINF_H_

#include <stdlib.h>
#include "CANaeroBase.h"
#include "CANAS_service.h"
#include "GenericIndicator.h"
#include <QList.h>

//---------------------------------------------------------------------------------------------------------------------
// CANaero Class definition
//---------------------------------------------------------------------------------------------------------------------
  // structure for containing the subscribed data elements to capture
typedef struct _CanDataLinks {
	int linkId;
	uint8_t canAreoId = 0;
	float lastVal;
	uint8_t last_message_code = 0;
	GenericIndicator* indicator = NULL;
	unsigned long timestamp = 0;  // last read time
};

class CANareoInterface : public CANaeroBase {
public:

	/// <summary>
	/// Initialize class instance.
	/// </summary>
	/// <param name="nodeId">        nodeId to use</param>
	/// <param name="serviceChannel">service channel to use</param>
	/// <param name="hdwId">         Hardware revision</param>
	/// <param name="swId">          software revision</param>

	CANareoInterface(uint8_t nodeId = 255, uint8_t serviceChannel = 0, uint8_t hdwId = 0, uint8_t swId = 0, void(*myCallBack)(CAN_FRAME*) = NULL);

	~CANareoInterface();

	/**
	 * Update instance state.
	 * Must be called for every new incoming frame or by timeout.
	 * @note It is recommended to call this function at least every 10 ms
	 * @return            @ref CanasErrorCode
	 */
	int Update();

	/// <summary>
	/// Parameter subscriptions. Each parameter must be subscribed before you can read it from the
	/// bus. Each subscription can track an arbitrary number of redundant units transmitting this
	/// message, from 1 to 255.
	/// </summary>
	/// <param name="msg_id">canas message id , see canas manual</param>
	/// <param name="instrument">pointer to instruement class to display parameter</param>
	/// <returns>CanasErrorCode</returns>
	int ParamSubscribe(GenericIndicator* instrument);
	int ParamUnsubscribe(uint16_t msg_id);
	//int ParamRead( uint16_t msg_id, CanasParamCallbackArgs* pargs);
	int ParamRead(uint16_t msg_id, float* newValue);

	/**
	 * @}
	 */

	 /**
	  * @}
	  */

	  /**
	   * Node Service Protocol API.
	   * Normally you will not use it if you are not going to implement your own service.
	   * Functions of this group return @ref CanasErrorCode.
	   * @{
	   */
	int ServiceRegister(CANAS_service* newService);
	int ServiceRegister_requestData(int(*newService)(uint8_t canasID), int(*removeService)(uint8_t canasID));
	int ServiceUnregister(uint8_t service_code);

	/*
	int canasServiceSetState(CanasInstance* pi, uint8_t service_code, void* pstate);
	int canasServiceGetState(CanasInstance* pi, uint8_t service_code, void** ppstate);
	*/

	//void* Malloc(CanasInstance* pi, int size);
	//void Free(CanasInstance* pi, void* ptr);
	/**
	 * @}
	 */
	void CallBack(CAN_FRAME* frame);

private:

	CANAS_service_identification* 	_identService;
	CANAS_service_requestdata* 		_requestService;
	CANAS_service_timestamp*		_timeStampService;

	//	CanasMallocFn fn_malloc;        ///< Required; read the notes @ref CanasMallocFn
	//	CanasFreeFn fn_free;            ///< Optional, may be NULL. Read the notes @ref /CanasFreeFn
	//	CanasHookCallbackFn fn_hook;    ///< Should be null if not used

	uint8_t _filters_per_iface = 0;      ///< Number of filters per interface. May be 0 if no filters available.

	//uint32_t _service_request_timeout_usec = CANAS_DEFAULT_SERVICE_REQUEST_TIMEOUT_USEC; ///< Time to wait for response from remote node. Default is okay.
	//uint16_t _service_poll_interval_usec = CANAS_DEFAULT_SERVICE_POLL_INTERVAL_USEC;  ///< Do not change
	//uint8_t  _service_frame_hist_len = CANAS_DEFAULT_SERVICE_HIST_LEN;  ///< Do not change

	uint32_t _repeat_timeout_usec = CANAS_DEFAULT_REPEAT_TIMEOUT_USEC;///< Largest interval of repeated messages (default should be good enough)

	//uint8_t  _redund_channel_id				= 0; ///< Local Node Redundancy Channel ID

protected:

	void(*_callBack)(CAN_FRAME*);

	// structure for containing all services we can reply to
	struct _CanServiceLinks {
		int linkId;
		uint8_t canServiceCode = 0;
		float currentVal = 0;
		CANAS_service* service;
		unsigned long timestamp = 0;	// last read time
	};

	static const int CANAS_DEFAULT_REPEAT_TIMEOUT_USEC = 30 * 1000 * 1000;
	QList<_CanDataLinks*> _dataLinkRefs;
	QList<_CanServiceLinks*> _serviceRefs;

	int _findDataLinkInList(uint8_t toFind);
	_CanDataLinks* _GetDataLinkInList(int i);
	int _findServiceInList(uint8_t toFind);

	int _updateValue(uint8_t type, float value);
	int _handleReceivedParam(CanasMessage* pframe, int subId, long timestamp);
	int _handleReceivedService(CanasMessage* pframe, int curRecord, long timestamp);

	int(*_newXservicecall)(uint8_t);
	int(*_removeXservicecall)(uint8_t);
};

#endif
