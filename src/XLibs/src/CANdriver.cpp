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
#include <QList.h>
#include <stdlib.h>
#include "CANdriver.h"

//
// These configuration parameters should be overriden by preprocessor definitions. 
//
#ifndef CAN_RX_QUEUE_LEN
#   define CAN_RX_QUEUE_LEN 20
#endif
#ifndef CAN_TX_QUEUE_LEN
#   define CAN_TX_QUEUE_LEN 20
#endif


typedef struct
{
	CanasCanFrame frame;
	uint8_t iface;
} FifoEntry;

typedef struct
{
	FifoEntry* const pbuf;
	const int bufsize;
	int in;
	int out;
	int len;
} Fifo;
/* Software FIFO */

static FifoEntry _buff_rx[CAN_RX_QUEUE_LEN];
static Fifo _fifo_rx = { _buff_rx, CAN_RX_QUEUE_LEN, 0, 0, 0 };


static inline void _fifoPush(Fifo* pfifo, const FifoEntry* pframe, bool* overflow)
{
	pfifo->pbuf[pfifo->in++] = *pframe;
	if (pfifo->in >= pfifo->bufsize)
		pfifo->in = 0;
	if (pfifo->len >= pfifo->bufsize)
	{
		*overflow = true;
		pfifo->out++;
		if (pfifo->out >= pfifo->bufsize)
			pfifo->out = 0;
	}
	else
	{
		*overflow = false;
		pfifo->len++;
	}
}

static inline int _fifoPop(Fifo* pfifo, FifoEntry* pframe)
{
	if (pfifo->len <= 0)
		return 0;
	pfifo->len--;
	if (pframe)
		*pframe = pfifo->pbuf[pfifo->out++];
	if (pfifo->out >= pfifo->bufsize)
		pfifo->out = 0;
	return 1;
}


//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
CANdriver::CANdriver() {
	CAN0.begin(XI_CANBUS_SPEED);
	CAN0.watchFor();
}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

CANdriver::~CANdriver(){
	
}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

int CANdriver::writeMsg(CanasCanFrame* frame){
	CAN_FRAME message;

	for (int i = 0; i < 8; i++)
		message.data.bytes[i] = frame->data[i];

	message.length = frame->dlc;
	if (frame->id & CANAS_CAN_FLAG_EFF){		
		message.extended = true;
	}
	else {		
		message.extended = false;
	}
	message.id = frame->id;
	message.rtr = (frame->id & CANAS_CAN_FLAG_RTR) ? 1 : 0;
	CAN0.sendFrame(message);
	
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------


int CANdriver::setFilter(int msgID, void(*cbFunction)(CAN_FRAME *)){
	int newMbx;

		newMbd = CAN0.watchFor(msgID);
		// TODO attach callback function
		if (newMbx != -1)
			CAN0.setCallback(newMbx, cbFunction);
			
}

int CANdriver::receive(CanasCanFrame * pframe, unsigned int timeout_usec)
{

	return 0;
}
