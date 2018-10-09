//-------------------------------------------------------------------------------------------------------------------
//	 CAN Aerospace class
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

#ifndef CANAERO_H_
#define CANAERO_H_

#include <stdlib.h>
#include <QList.h>
#include "CanAeroMessage.h"
#include "CAN_ids.h"
#include "CANdriver.h"
#include "CANAS_service.h"
#include "GenericInstrument.h"
#include "mydebug.h"

// generic canas stuff
/**
 * Broadcast Node ID can be used to perform network-wide service requests
 */
static const int CANAS_BROADCAST_NODE_ID = 0;

/**
 * Maximum number of nodes per one network
 */
static const int CANAS_MAX_NODES = 255;

typedef enum
{
	MSGGROUP_WTF,
	MSGGROUP_PARAMETER,
	MSGGROUP_SERVICE
} MessageGroup;

/**
 * Nearly all API calls return an error code.
 * @note API calls return the negative error codes. You have to invert the sign to get the actual error code.
 */
typedef enum
{
	CANAS_ERR_OK = 0,

	CANAS_ERR_ARGUMENT,
	CANAS_ERR_NOT_ENOUGH_MEMORY,

	CANAS_ERR_DRIVER,

	CANAS_ERR_NO_SUCH_ENTRY,
	CANAS_ERR_ENTRY_EXISTS,

	CANAS_ERR_BAD_DATA_TYPE,
	CANAS_ERR_BAD_MESSAGE_ID,
	CANAS_ERR_BAD_NODE_ID,
	CANAS_ERR_BAD_REDUND_CHAN,
	CANAS_ERR_BAD_SERVICE_CHAN,
	CANAS_ERR_BAD_CAN_FRAME,

	CANAS_ERR_QUOTA_EXCEEDED,
	CANAS_ERR_LOGIC                 ///< May be returned by a service if it goes wrong
} CanasErrorCode;

struct _CanDataLinks {
	int linkId;
	uint8_t canAreoId = 0;
	float lastVal;	
	uint8_t last_message_code = 0;
	GenericIndicator* indicator = NULL;
	unsigned long timestamp = 0;	// last read time	
};

struct _CanServiceLinks {
	int linkId;
	uint8_t canServiceCode = 0;
	float currentVal = 0;
	CANAS_service* service;
	unsigned long timestamp = 0;	// last read time	
};

/*
typedef struct
{
	uint64_t		timestamp_usec;
	void*			parg;
	CanasMessage	message;
	uint16_t		message_id;
	uint8_t			redund_channel_id;
} CanasParamCallbackArgs;
*/

typedef void(*CanasParamCallbackFn)(float);
//---------------------------------------------------------------------------------------------------------------------
// CANaero Class definition
//---------------------------------------------------------------------------------------------------------------------

class CANaero
{
public:
	/**
	 * Initialize class instance.
	 * @param [in]  canDriver the driver objecct wiht send recivve and filter functions for the can bus;
	 */
	 
	CANaero(uint8_t nodeId = 255, uint8_t hdwId=0, uint8_t swId =0);

	~CANaero();

	int setNodeId(int newID);


/**
 * Update instance state.
 * Must be called for every new incoming frame or by timeout.
 * @note It is recommended to call this function at least every 10 ms
 * @param [in] pi     Instance pointer
 * @param [in] iface  Interface index from which the frame was received; ignored when no frame provided
 * @param [in] pframe Pointer to the received frame, NULL when called by timeout
 * @return            @ref CanasErrorCode
 */
int Update( );

/**
 * Parameter subscriptions.
 * Each parameter must be subscribed before you can read it from the bus.
 * Each subscription can track an arbitrary number of redundant units transmitting this message, from 1 to 255.
 * Functions of this group return @ref CanasErrorCode.
 * @{
 */
//int ParamSubscribe( uint16_t msg_id,  CanasParamCallbackFn callback, void* callback_arg);
int ParamSubscribe( uint16_t msg_id, GenericIndicator* instrument);
int ParamUnsubscribe( uint16_t msg_id);
//int ParamRead( uint16_t msg_id, CanasParamCallbackArgs* pargs);
int ParamRead( uint16_t msg_id, float* newValue);


/**
 * @}
 */

 /**
  * Parameter publications.
  * Each parameter must be advertised before you can publish it to the bus.
  * 'interlaced' stands for traffic sharing between all available interfaces.
  * Functions of this group return @ref CanasErrorCode.
  * @{
  */
int ParamAdvertise( uint16_t msg_id, bool interlaced);
int ParamUnadvertise( uint16_t msg_id);
int ParamPublish( uint16_t msg_id, const CanasMessageData* pdata, uint8_t service_code);
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

//int ServiceRegister(CanasInstance* pi, uint8_t service_code, CanasServicePollCallbackFn callback_poll,
//	CanasServiceRequestCallbackFn callback_request,
//	CanasServiceResponseCallbackFn callback_response, void* pstate);
int ServiceUnregister(uint8_t service_code);
/*
int canasServiceSendRequest(CanasInstance* pi, const CanasMessage* pmsg);
int canasServiceSendResponse(CanasInstance* pi, const CanasMessage* pmsg, uint8_t service_channel);
int canasServiceSetState(CanasInstance* pi, uint8_t service_code, void* pstate);
int canasServiceGetState(CanasInstance* pi, uint8_t service_code, void** ppstate);
*/
/**
 * @}
 */


 /**
  * Dump a CAN frame for humans.
  * @param [in]  pframe Pointer to frame to be dumped
  * @param [out] pbuf   Pointer to output string buffer of size @ref CANAS_DUMP_BUF_LEN
  * @return             pbuf
  */
char* DumpCanFrame(const CanasCanFrame* pframe, char* pbuf);

/**
 * Dump a CANaerospace message for humans.
 * @param [in]  pframe Pointer to message to be dumped
 * @param [out] pbuf   Pointer to output string buffer of size @ref CANAS_DUMP_BUF_LEN
 * @return             pbuf
 */
char* DumpMessage(const CanasMessage* pmsg, char* pbuf);

/**
 * Convenience functions for Node Service Protocol services.
 * They are just wrappers over corresponding function pointers in the configuration structure.
 * @{
 */
uint64_t Timestamp(CanasInstance* pi);
//void* Malloc(CanasInstance* pi, int size);
//void Free(CanasInstance* pi, void* ptr);
/**
 * @}
 */
/// ---- service.c
/*
bool IsInterestingServiceMessage(CanasInstance* pi, uint16_t msg_id, CanasMessage *pmsg);

void HandleReceivedService(CanasInstance* pi, CanasServiceSubscription* psrv, uint8_t iface, uint16_t msg_id,
	CanasMessage *pmsg, const CanasCanFrame* pframe, uint64_t timestamp_usec);

void PollServices(CanasInstance* pi, uint64_t timestamp_usec);

int ServiceChannelToMessageID(uint8_t service_channel, bool isrequest);

bool IsValidServiceChannel(uint8_t service_channel);
*/
private:
	static CANdriver _canBus;
	
	CANAS_service_identification* _identService;
	CANAS_service_requestdata* _requestService

//	CanasMallocFn fn_malloc;        ///< Required; read the notes @ref CanasMallocFn
//	CanasFreeFn fn_free;            ///< Optional, may be NULL. Read the notes @ref /CanasFreeFn

//	CanasHookCallbackFn fn_hook;    ///< Should be null if not used

	uint8_t _filters_per_iface				= 0;      ///< Number of filters per interface. May be 0 if no filters available.

	uint32_t _service_request_timeout_usec	= CANAS_DEFAULT_SERVICE_REQUEST_TIMEOUT_USEC ; ///< Time to wait for response from remote node. Default is okay.
	uint16_t _service_poll_interval_usec	= CANAS_DEFAULT_SERVICE_POLL_INTERVAL_USEC;  ///< Do not change
	uint8_t  _service_frame_hist_len		= CANAS_DEFAULT_SERVICE_HIST_LEN;  ///< Do not change
	uint8_t  _service_channel				= -1;              ///< Service channel of the Local Node. May be of high or low priority.
	uint32_t _repeat_timeout_usec			= CANAS_DEFAULT_REPEAT_TIMEOUT_USEC;///< Largest interval of repeated messages (default should be good enough)

	uint8_t  _node_id						= 255; ///< Local Node ID
	uint8_t  _redund_channel_id				= 0; ///< Local Node Redundancy Channel ID
	static void _CallBack(CAN_FRAME* frame);

protected:
	static const int CANAS_DEFAULT_REPEAT_TIMEOUT_USEC = 30 * 1000 * 1000;
	static QList<_CanDataLinks *> _listRefs;
	static QList<_CanServiceLinks *> _serviceRefs;

	static int _findInList(uint8_t toFind);
	static int _findServiceInList(uint8_t toFind);
	int _updateValue(uint8_t type, float value);
	int _handleReceivedParam(CanasMessage* pframe, int subId, long timestamp);
	int _handleReceivedService(CanasMessage* pframe, int curRecord,long timestamp);
	
};
// =================================================================

// =================================================================


#endif