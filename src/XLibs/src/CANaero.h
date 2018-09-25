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
#include "CANdriver.h"

/**
 * Broadcast Node ID can be used to perform network-wide service requests
 */
static const int CANAS_BROADCAST_NODE_ID = 0;

/**
 * Maximum number of nodes per one network
 */
static const int CANAS_MAX_NODES = 255;

/**
 * This library can work with the number of redundant interfaces not higher than this.
 */
static const int CANAS_IFACE_COUNT_MAX = 8;

/**
 * Buffer size required for functions like canasDump*()
 */
#define CANAS_DUMP_BUF_LEN 50

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

typedef struct CanasInstanceStruct CanasInstance;

/**
 * Send a message to the bus.
 * @param [in] pi    Instance pointer
 * @param [in] iface Interface index
 * @param [in] pmsg  Pointer to the message to be sent
 * @return           Number of messages sent (0 or 1), negative on failure
 */
typedef int(*CanasCanSendFn)(int, const CanasCanFrame*);

/**
 * Configure acceptance filters.
 * @param [in] pi       Instance pointer
 * @param [in] iface    Interface index
 * @param [in] pfilters Pointer to an array of filter configs
 * @param [in] len      Length of the config array
 * @return              0 if ok, negative on failure
 */
typedef int(*CanasCanFilterFn)( int, const CanasCanFilterConfig*, int);

/**
 * Allocates a chunk of memory.
 * If the application does not require de-initialization features like unsubscription of unadvertisement, then dynamic
 * memory is not needed at all. If this is the case, you can use a static pool allocator.
 * This feature is useful for embedded systems where dynamic memory is not always available.
 * @note Allocated memory must be aligned properly.
 * @param [in] pi   Instance pointer
 * @param [in] size Size of the memory block required
 * @return          Pointer or NULL if no memory available
 */
typedef void* (*CanasMallocFn)(CanasInstance*, int);

/**
 * Deallocates memory. This function is not required in most cases, see @ref CanasMallocFn for details.
 * If the deallocation is not needed (e.g. in case of static allocator), then pointer to this function should be NULL.
 * @param [in] pi  Instance pointer
 * @param [in] ptr Pointer to the memory to be deallocated
 */
typedef void(*CanasFreeFn)(CanasInstance*, void*);

/**
 * Returns current timestamp in microseconds.
 * Any base value (uptime or UNIX time) will do.
 * @param [in] pi Instance pointer
 * @return        Current timestamp in microseconds
 */
typedef uint64_t(*CanasTimestampFn)(CanasInstance*);

typedef struct
{
	uint64_t timestamp_usec;
	CanasMessage message;
	uint16_t message_id;
	uint8_t redund_channel_id;
	uint8_t iface;
} CanasHookCallbackArgs;
typedef void(*CanasHookCallbackFn)(CanasInstance*, CanasHookCallbackArgs*);

typedef struct
{
	uint64_t timestamp_usec;
	void* parg;
	CanasMessage message;
	uint16_t message_id;
	uint8_t redund_channel_id;
} CanasParamCallbackArgs;
typedef void(*CanasParamCallbackFn)(CanasInstance*, CanasParamCallbackArgs*);

typedef struct
{
	uint64_t timestamp_usec;
	void* pstate;
} CanasServicePollCallbackArgs;
typedef void(*CanasServicePollCallbackFn)(CanasInstance*, CanasServicePollCallbackArgs*);

typedef struct
{
	uint64_t timestamp_usec;
	void* pstate;
	CanasMessage message;
	uint8_t service_channel;
} CanasServiceRequestCallbackArgs;
typedef void(*CanasServiceRequestCallbackFn)(CanasInstance*, CanasServiceRequestCallbackArgs*);

typedef struct
{
	uint64_t timestamp_usec;
	void* pstate;
	CanasMessage message;
} CanasServiceResponseCallbackArgs;
typedef void(*CanasServiceResponseCallbackFn)(CanasInstance*, CanasServiceResponseCallbackArgs*);

typedef struct
{
	uint64_t timestamp_usec;        ///< Empty entry contains zero timestamp
	uint32_t header;                ///< First 4 bytes of frame
	uint8_t ifaces_mask;
} CanasServiceFrameHistoryEntry;

typedef struct
{
	void* pnext;                    ///< Must be the first entry
	CanasServicePollCallbackFn callback_poll;
	CanasServiceRequestCallbackFn callback_request;
	CanasServiceResponseCallbackFn callback_response;
	void* pstate;
	uint8_t service_code;
	uint8_t history_len;
	CanasServiceFrameHistoryEntry history[1]; // flexible
} CanasServiceSubscription;

typedef struct
{
	uint64_t timestamp_usec;        ///< Empty entry contains zero timestamp
	CanasMessage message;
} CanasParamCacheEntry;

typedef struct
{
	void* pnext;                    ///< Must be the first entry
	CanasParamCallbackFn callback;
	void* callback_arg;
	uint16_t message_id;
	uint8_t redund_count;
	CanasParamCacheEntry redund_cache[1]; // flexible
} CanasParamSubscription;

typedef struct
{
	void* pnext;                    ///< Must be the first entry
	uint16_t message_id;
	uint8_t message_code;
	int8_t interlacing_next_iface;
} CanasParamAdvertisement;



struct CanasInstanceStruct
{
	CanasConfig config;
	void* pthis;                    ///< To be used by application

	uint64_t last_service_ts;

	CanasServiceSubscription* pservice_subs;
	CanasParamSubscription* pparam_subs;
	CanasParamAdvertisement* pparam_advs;
};

typedef enum
{
	MSGGROUP_WTF,
	MSGGROUP_PARAMETER,
	MSGGROUP_SERVICE
} MessageGroup;

#define ALL_IFACES  -1
#define REDUND_CHAN_MULT 65536ul
#define RANGEINCLUSIVE(x, min, max) ((x) >= (min) && (x) <= (max))

//---------
/**
 * At least 10ms is required by some services.
 * Thus, increasing is NOT recommended.
 */
static const int CANAS_DEFAULT_SERVICE_POLL_INTERVAL_USEC = 10 * 1000;

/**
 * History length for repetition detection.
 * Expressed in number of frames to track.
 */
static const int CANAS_DEFAULT_SERVICE_HIST_LEN = 32;

/**
 * Time to wait for response from remote node
 */
static const int CANAS_DEFAULT_SERVICE_REQUEST_TIMEOUT_USEC = 100 * 1000;

//---------------------------------------------------------------------------------------------------------------------
// CANaero Class definition
//---------------------------------------------------------------------------------------------------------------------

class CANaero
{
public:
	/**
	 * Initialize class instance.
	 * @param [in]  canSend pointer to canSend Function;
	 */
	 
	CANaero(CANdriver* canDriver);

	~CANaero();


/**
 * Update instance state.
 * Must be called for every new incoming frame or by timeout.
 * @note It is recommended to call this function at least every 10 ms
 * @param [in] pi     Instance pointer
 * @param [in] iface  Interface index from which the frame was received; ignored when no frame provided
 * @param [in] pframe Pointer to the received frame, NULL when called by timeout
 * @return            @ref CanasErrorCode
 */
int Update( const CanasCanFrame* pframe);

/**
 * Parameter subscriptions.
 * Each parameter must be subscribed before you can read it from the bus.
 * Each subscription can track an arbitrary number of redundant units transmitting this message, from 1 to 255.
 * Functions of this group return @ref CanasErrorCode.
 * @{
 */
int ParamSubscribe( uint16_t msg_id,  CanasParamCallbackFn callback, void* callback_arg);
int ParamUnsubscribe( uint16_t msg_id);
int ParamRead( uint16_t msg_id, CanasParamCallbackArgs* pargs);
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
/*
int canasServiceSendRequest(CanasInstance* pi, const CanasMessage* pmsg);
int canasServiceSendResponse(CanasInstance* pi, const CanasMessage* pmsg, uint8_t service_channel);
int canasServiceRegister(CanasInstance* pi, uint8_t service_code, CanasServicePollCallbackFn callback_poll,
	CanasServiceRequestCallbackFn callback_request,
	CanasServiceResponseCallbackFn callback_response, void* pstate);
int canasServiceUnregister(CanasInstance* pi, uint8_t service_code);
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
	CANdriver* 			_canBus = NULL;

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

protected:
	static const int CANAS_DEFAULT_REPEAT_TIMEOUT_USEC = 30 * 1000 * 1000;


};

#endif