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

#ifndef CANAERO_H_
#define CANAERO_H_

#include <stdlib.h>
#include <QList.h>
#include "CanAeroMessage.h"
#include "CAN_ids.h"
#include "CANdriver.h"
#include "CANAS_service.h"
#include "src\Instruments\Src\GenericIndicator.h"
#include "src\XLibs\Src\mydebug.h"

/**
* Buffer size required for functions like canasDump*()
*/
#define CANAS_DUMP_BUF_LEN 50

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
	CANAS_ERR_LOGIC,                ///< May be returned by a service if it goes wrong
	CANAS_ERR_NOT_RUNNING
} CanasErrorCode;

// structure for containing the subscribed data elements to capture
struct _CanDataLinks {
	int linkId;
	uint8_t canAreoId = 0;
	float lastVal;
	uint8_t last_message_code = 0;
	GenericIndicator* indicator = NULL;
	unsigned long timestamp = 0;	// last read time
};

// structure for containing all services we can reply to
struct _CanServiceLinks {
	int linkId;
	uint8_t canServiceCode = 0;
	float currentVal = 0;
	CANAS_service* service;
	unsigned long timestamp = 0;	// last read time
};

// structure for containing all published dataelements we publish
struct _CanPublishLinks {
	int linkId;
	uint8_t canAreoId = 0;
	float currentVal = 0;
	uint8_t maxIntervalMs = 100;
	uint8_t last_message_code = 0;
	unsigned long timestamp = 0;	// last send time
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

	/// <summary>
	/// Initialize class instance.
	/// </summary>
	/// <param name="nodeId">        nodeId to use</param>
	/// <param name="serviceChannel">service channel to use</param>
	/// <param name="hdwId">         Hardware revision</param>
	/// <param name="swId">          software revision</param>

	CANaero(uint8_t nodeId = 255, uint8_t serviceChannel = 0, uint8_t hdwId = 0, uint8_t swId = 0);

	~CANaero();

	int setNodeId(uint8_t newID);
	int setServiceChannel(uint8_t newChannel);
	int start();
	int stop();

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
	int ParamSubscribe(uint16_t msg_id, GenericIndicator* instrument);
	int ParamUnsubscribe(uint16_t msg_id);
	//int ParamRead( uint16_t msg_id, CanasParamCallbackArgs* pargs);
	int ParamRead(uint16_t msg_id, float* newValue);

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
	int ParamAdvertise(uint16_t msg_id, uint16_t intervalMs = 100);
	int ParamUnadvertise(uint16_t msg_id);
	int ParamPublish(uint16_t msg_id, const CanasMessageData* pdata, uint8_t service_code);
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
	int ServiceUnregister(uint8_t service_code);
	int ServiceSendRequest(const CanasMessage* pmsg, uint8_t service_channel = _service_channel);
	int ServiceSendResponse(const CanasMessage* pmsg, uint8_t service_channel = _service_channel);
	/*
	int canasServiceSetState(CanasInstance* pi, uint8_t service_code, void* pstate);
	int canasServiceGetState(CanasInstance* pi, uint8_t service_code, void** ppstate);
	*/

	/**
	 * Dump a CAN frame for humans.
	 * @param [in]  pframe Pointer to frame to be dumped
	 * @param [out] pbuf   Pointer to output string buffer of size @ref CANAS_DUMP_BUF_LEN
	 * @return             pbuf
	 */
	static char* DumpCanFrame(const CanasCanFrame* pframe, char* pbuf);

	/**
	 * Dump a CANaerospace message for humans.
	 * @param [in]  pframe Pointer to message to be dumped
	 * @param [out] pbuf   Pointer to output string buffer of size @ref CANAS_DUMP_BUF_LEN
	 * @return             pbuf
	 */
	static char* DumpMessage(const CanasMessage* pmsg, char* pbuf);

	/**
	 * Convenience functions for Node Service Protocol services.
	 * They are just wrappers over corresponding function pointers in the configuration structure.
	 * @{
	 */
	uint64_t Timestamp();
	//void* Malloc(CanasInstance* pi, int size);
	//void Free(CanasInstance* pi, void* ptr);
	/**
	 * @}
	 */
	uint8_t  _service_channel = -1;              ///< Service channel of the Local Node. May be of high or low priority.
private:
	static CANdriver _canBus;

	CANAS_service_identification* 	_identService;
	CANAS_service_requestdata* 		_requestService;
	CANAS_service_timestamp*		_timeStampService;

	//	CanasMallocFn fn_malloc;        ///< Required; read the notes @ref CanasMallocFn
	//	CanasFreeFn fn_free;            ///< Optional, may be NULL. Read the notes @ref /CanasFreeFn
	//	CanasHookCallbackFn fn_hook;    ///< Should be null if not used

	uint8_t _filters_per_iface = 0;      ///< Number of filters per interface. May be 0 if no filters available.

	uint32_t _service_request_timeout_usec = CANAS_DEFAULT_SERVICE_REQUEST_TIMEOUT_USEC; ///< Time to wait for response from remote node. Default is okay.
	uint16_t _service_poll_interval_usec = CANAS_DEFAULT_SERVICE_POLL_INTERVAL_USEC;  ///< Do not change
	uint8_t  _service_frame_hist_len = CANAS_DEFAULT_SERVICE_HIST_LEN;  ///< Do not change

	uint32_t _repeat_timeout_usec = CANAS_DEFAULT_REPEAT_TIMEOUT_USEC;///< Largest interval of repeated messages (default should be good enough)

	uint8_t  _node_id = 255; ///< Local Node ID
	//uint8_t  _redund_channel_id				= 0; ///< Local Node Redundancy Channel ID
	static void _CallBack(CAN_FRAME* frame);
	static int _genericSend(uint16_t msg_id, uint8_t msggroup, const CanasMessage* pmsg);

protected:
	bool _running = false;
	static const int CANAS_DEFAULT_REPEAT_TIMEOUT_USEC = 30 * 1000 * 1000;
	QList<_CanDataLinks *> _listRefs;
	QList<_CanServiceLinks *> _serviceRefs;
	QList < _CanPublishLinks*> _publishRefs;

	int _findInList(uint8_t toFind);
	int _findServiceInList(uint8_t toFind);
	int _findPublishedInList(uint8_t toFind);

	static int _parseFrame(const CanasCanFrame* pframe, uint16_t* pmsg_id, CanasMessage* pmsg);

	int _updateValue(uint8_t type, float value);
	int _handleReceivedParam(CanasMessage* pframe, int subId, long timestamp);
	int _handleReceivedService(CanasMessage* pframe, int curRecord, long timestamp);
};
// =================================================================
//
// =================================================================

#ifdef MACRO_DEBUG
// Debugging can only work in single-threaded environment
static char _canas_debug_strbuf_dumpframe[CANAS_DUMP_BUF_LEN] __attribute__((unused));

#define CANAS_DUMPFRAME(pframe) CANareo::DumpCanFrame(pframe, _canas_debug_strbuf_dumpframe)
#define CANAS_MESSAGE(pmsg) CANareo::DumpMessage(pmsg, _canas_debug_strbuf_dumpframe)

#else
#define CANAS_DUMPFRAME(pframe)
#define CANAS_MESSAGE(pmsg)
#endif

#endif