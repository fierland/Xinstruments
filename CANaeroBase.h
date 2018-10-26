//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================
/// <summary>
/// CAN Aerospace class base elements
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
#ifndef _CANAEROINF_BASE_H_
#define _CANAEROINF_BASE_H_

#include "CanAeroMessage.h"
#include "CANdriver.h"
#include <QList.h>

typedef int(*CanasParamCallbackFn)(float);

class CANaeroBase {
public:
	int serviceChannelToMessageID(uint8_t service_channel, bool isrequest);
	CANaeroBase();
	~CANaeroBase();

	virtual int start();
	virtual int stop();

	virtual int setNodeId(uint8_t newID);
	virtual uint8_t getNodeId();
	virtual int setServiceChannel(uint8_t newChannel);

	/**
	 * Convenience functions for Node Service Protocol services.
	 * They are just wrappers over corresponding function pointers in the configuration structure.
	 * @{
	 */
	virtual uint64_t Timestamp();

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
	static bool canasIsValidServiceChannel(uint8_t service_channel);

	/**
	 * Parameter publications.
	 * Each parameter must be advertised before you can publish it to the bus.
	 * 'interlaced' stands for traffic sharing between all available interfaces.
	 * Functions of this group return @ref CanasErrorCode.
	 * @{
	 */
	virtual int ParamAdvertise(uint16_t msg_id, uint16_t intervalMs = 100);
	virtual int ParamUnadvertise(uint16_t msg_id);
	virtual int ParamPublish(uint16_t msg_id, const CanasMessageData* pdata);

	virtual int ServiceSendRequest(const CanasMessage* pmsg, uint8_t service_channel = 0);
	virtual int ServiceSendResponse(const CanasMessage* pmsg, uint8_t service_channel = 0);
	uint8_t  _service_channel;              ///< Service channel of the Local Node. May be of high or low priority.

	//void CallBack(CAN_FRAME* frame);

protected:
	static MessageGroup _detectMessageGroup(uint16_t id);
	// structure for containing all published dataelements we publish
	struct _CanPublishLinks {
		int linkId;
		uint8_t canAreoId = 0;
		float currentVal = 0;
		uint8_t maxIntervalMs = 100;
		uint8_t last_message_code = 0;
		unsigned long timestamp = 0;	// last send time
	};

	QList<_CanPublishLinks*> _publishRefs;

	int _findPublishedInList(uint8_t toFind);

	CANdriver _canBus;

	uint8_t  _node_id = 255; ///< Local Node ID

	int _genericSend(uint16_t msg_id, uint8_t msggroup, const CanasMessage* pmsg);
	static int _canasDataCopy(CanasMessageData * pdataTo, const CanasMessageData * pdataFrom);
	bool _running = false;

	static int _parseFrame(const CanasCanFrame* pframe, uint16_t* pmsg_id, CanasMessage* pmsg);
};
// =================================================================
//
// =================================================================
#ifdef MACRO_DEBUG
// Debugging can only work in single-threaded environment
static char _canas_debug_strbuf_dumpframe[CANAS_DUMP_BUF_LEN] __attribute__((unused));

#define CANAS_DUMPFRAME(pframe) CANareoBase::DumpCanFrame(pframe, _canas_debug_strbuf_dumpframe)
#define CANAS_MESSAGE(pmsg) CANareoBase::DumpMessage(pmsg, _canas_debug_strbuf_dumpframe)

#else
#define CANAS_DUMPFRAME(pframe)
#define CANAS_MESSAGE(pmsg)
#endif
#endif
