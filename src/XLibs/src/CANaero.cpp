//-------------------------------------------------------------------------------------------------------------------
//	 CAN Aerospace class
//
//  Franks Flightsim Intruments project
//	by Frank van Ierland
//
// A light implementation of the CAN Aerospace protocol to manage simulated instruments.
//	This code is in the public domain.
//
//	VERSION HISTORY:
//	
//
//-------------------------------------------------------------------------------------------------------------------

#include "CANaero.h"
//-------------------------------------------------------------------------------------------------------------------
// helper functions
//-------------------------------------------------------------------------------------------------------------------

static MessageGroup _detectMessageGroup(uint16_t id)
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

	CANAS_TRACE("msggroup: failed to detect, msgid=%03x\n", (unsigned int)id);
	return MSGGROUP_WTF;
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
/// positive if a > b
static int _diffU8(uint8_t a, uint8_t b)
{
	const int d = a - b;
	if (d <= -128)
		return 256 + d;
	else if (d >= 127)
		return d - 256;
	return d;
}

//-------------------------------------------------------------------------------------------------------------------
// CLASS functions
//-------------------------------------------------------------------------------------------------------------------

CANaero::CANaero(CanasCanSendFn canSend, CanasCanFilterFn canFilter)
{		
	_fn_send	= canSend;
	_fn_filter	= canFilter;

	// set generic filter
	if (_fn_filter != NULL && _filters_per_iface) // Ask filter to accept all messages
	{
		CanasCanFilterConfig filt;
		filt.id = 0;                                        // Dear filter,
		filt.mask = CANAS_CAN_FLAG_RTR;                     // Would you be so kind to...
		for (int i = 0; i < _iface_count; i++)
			if (( _fn_filter(i, &filt, 1)) < 0)
				return -CANAS_ERR_DRIVER;
	}
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------

CANaero::~CANaero()
{
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::Update( int iface, const CanasCanFrame* pframe)
{
	const uint64_t timestamp = canasTimestamp(pi);

	if (pi == NULL)
		return -CANAS_ERR_ARGUMENT;
	if (pframe != NULL && (iface >= pi->config.iface_count || iface < 0))
		return -CANAS_ERR_ARGUMENT;

	uint16_t msg_id = 0xFFFF;
	CanasMessage msg;
	MessageGroup msggroup = MSGGROUP_WTF;
	uint8_t redund_ch = 0;
	int ret = 0;

	if (pframe != NULL)
	{
		//CANAS_TRACE("recv %s\n", CANAS_DUMPFRAME(pframe));
		ret = _parseFrame(pframe, &msg_id, &msg, &redund_ch);
		if (ret == 0)
		{
			msggroup = _detectMessageGroup(msg_id);
			if (msggroup == MSGGROUP_WTF)
			{
				CANAS_TRACE("update: failed to detect the message group\n");
				ret = -CANAS_ERR_BAD_MESSAGE_ID;
			}
		}
	}

	if (msggroup != MSGGROUP_WTF && pi->config.fn_hook != NULL)
		_issueMessageHookCallback(pi, iface, msg_id, &msg, redund_ch, timestamp);

	if (msggroup == MSGGROUP_PARAMETER)
	{
		CanasParamSubscription* ppar = _findParamSubscription(pi, msg_id);
		if (ppar != NULL)
			_handleReceivedParam(pi, ppar, msg_id, &msg, redund_ch, timestamp);
		else
			CANAS_TRACE("foreign param msgid=%03x datatype=%i\n", (unsigned  int)msg_id, (int)msg.data.type);
	}
	else if (msggroup == MSGGROUP_SERVICE)
	{
		if (canasIsInterestingServiceMessage(pi, msg_id, &msg))
		{
			CanasServiceSubscription* psrv = _findServiceSubscription(pi, msg.service_code);
			// Redundancy channel ID is not used with services
			if (psrv != NULL)
				canasHandleReceivedService(pi, psrv, iface, msg_id, &msg, pframe, timestamp);
			else
				CANAS_TRACE("foreign serv msgid=%03x srvcode=%i\n", (unsigned  int)msg_id, (int)msg.service_code);
		}
	}
	canasPollServices(pi, timestamp);
	return ret;
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamSubscribe(uint16_t msg_id, uint8_t redund_chan_count,
	CanasParamCallbackFn callback, void* callback_arg)
{
	if (pi == NULL)
		return -CANAS_ERR_ARGUMENT;

	if (_detectMessageGroup(msg_id) != MSGGROUP_PARAMETER)
		return -CANAS_ERR_BAD_MESSAGE_ID;
	if (redund_chan_count < 1)
		return -CANAS_ERR_BAD_REDUND_CHAN;
	if (_findParamSubscription(pi, msg_id) != NULL)
		return -CANAS_ERR_ENTRY_EXISTS;

	// this size magic is necessary because C++ does not allow flexible and zero-length arrays
	int size = sizeof(CanasParamSubscription) +
		sizeof(CanasParamCacheEntry) * redund_chan_count - sizeof(CanasParamCacheEntry);
	CanasParamSubscription* psub = canasMalloc(pi, size);
	if (psub == NULL)
		return -CANAS_ERR_NOT_ENOUGH_MEMORY;

	memset(psub, 0, size);
	psub->callback = callback;
	psub->callback_arg = callback_arg;
	psub->message_id = msg_id;
	psub->redund_count = redund_chan_count;

	canasListInsert((CanasListEntry**)&pi->pparam_subs, psub);
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamUnsubscribe(uint16_t msg_id)
{
	if (pi == NULL)
		return -CANAS_ERR_ARGUMENT;

	CanasParamSubscription* psub = _findParamSubscription(pi, msg_id);
	if (psub != NULL)
	{
		canasListRemove((CanasListEntry**)&pi->pparam_subs, psub);
		canasFree(pi, psub);
		return 0;
	}
	return -CANAS_ERR_NO_SUCH_ENTRY;
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamRead( uint16_t msg_id, uint8_t redund_chan, CanasParamCallbackArgs* pargs)
{
	if (pi == NULL || pargs == NULL)
		return -CANAS_ERR_ARGUMENT;

	CanasParamSubscription* psub = _findParamSubscription(pi, msg_id);
	if (psub != NULL)
	{
		if (redund_chan >= psub->redund_count)
			return -CANAS_ERR_BAD_REDUND_CHAN;
		pargs->message = psub->redund_cache[redund_chan].message;
		pargs->timestamp_usec = psub->redund_cache[redund_chan].timestamp_usec;
		pargs->message_id = msg_id;
		pargs->parg = psub->callback_arg;
		pargs->redund_channel_id = redund_chan;
		return 0;
	}
	return -CANAS_ERR_NO_SUCH_ENTRY;
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamAdvertise( uint16_t msg_id, bool interlaced)
{
	if (pi == NULL)
		return -CANAS_ERR_ARGUMENT;
	if (_detectMessageGroup(msg_id) != MSGGROUP_PARAMETER)
		return -CANAS_ERR_BAD_MESSAGE_ID;
	if (_findParamAdvertisement(pi, msg_id) != NULL)
		return -CANAS_ERR_ENTRY_EXISTS;

	CanasParamAdvertisement* padv = canasMalloc(pi, sizeof(CanasParamAdvertisement));
	if (padv == NULL)
		return -CANAS_ERR_NOT_ENOUGH_MEMORY;
	memset(padv, 0, sizeof(*padv));
	padv->message_id = msg_id;

	// Interlacing is only enabled if we have more than one interface (obviously)
	if (pi->config.iface_count < 2)
		interlaced = false;
	padv->interlacing_next_iface = interlaced ? 0 : ALL_IFACES;

	canasListInsert((CanasListEntry**)&pi->pparam_advs, padv);
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamUnadvertise( uint16_t msg_id)
{
	if (pi == NULL)
		return -CANAS_ERR_ARGUMENT;

	CanasParamAdvertisement* padv = _findParamAdvertisement(pi, msg_id);
	if (padv != NULL)
	{
		canasListRemove((CanasListEntry**)&pi->pparam_advs, padv);
		canasFree(pi, padv);
		return 0;
	}
	return -CANAS_ERR_NO_SUCH_ENTRY;
}
//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------
int CANaero::ParamPublish( uint16_t msg_id, const CanasMessageData* pdata, uint8_t service_code)
{
	if (pi == NULL || pdata == NULL)
		return -CANAS_ERR_ARGUMENT;

	const uint8_t msggroup = _detectMessageGroup(msg_id);
	if (msggroup != MSGGROUP_PARAMETER)
		return -CANAS_ERR_BAD_MESSAGE_ID;

	CanasParamAdvertisement* padv = _findParamAdvertisement(pi, msg_id);
	if (padv == NULL)
		return -CANAS_ERR_NO_SUCH_ENTRY;

	int iface = ALL_IFACES;
	if (padv->interlacing_next_iface >= 0)
	{
		iface = padv->interlacing_next_iface++;
		if (padv->interlacing_next_iface >= pi->config.iface_count)
			padv->interlacing_next_iface = 0;
	}

	CanasMessage msg;
	memset(&msg, 0, sizeof(msg));
	msg.node_id = pi->config.node_id;
	msg.service_code = service_code;
	msg.message_code = padv->message_code++;    // Keeping the correct value of message code
	msg.data = *pdata;
	return _genericSend(pi, iface, msg_id, msggroup, &msg);
}


//-------------------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------------------