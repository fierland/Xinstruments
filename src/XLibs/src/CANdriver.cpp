
//-------------------------------------------------------------------------------------------------------------------
//	 CAN aerospace driver class
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

//-------------------------------------------------------------------------------------------------------------------
// initiator
//-------------------------------------------------------------------------------------------------------------------
CANdriver::CANdriver() {
	CAN0.begin(XI_CANBUS_SPEED);
	CAN0.watchFor();
}

//-------------------------------------------------------------------------------------------------------------------
// destructor and close CAN bus
//-------------------------------------------------------------------------------------------------------------------

CANdriver::~CANdriver(){
	CAN0.disable();
}
//-------------------------------------------------------------------------------------------------------------------
// write message to canbus
//-------------------------------------------------------------------------------------------------------------------

int CANdriver::writeMsg(CanasCanFrame* frame){
	CAN_FRAME message;

	if (frame == NULL)
		return -1;
	
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
// set filter on canbus
//-------------------------------------------------------------------------------------------------------------------

int CANdriver::setFilter(int msgID, void(*cbFunction)(CAN_FRAME *)){
	int newMbx;

		newMbx = CAN0.watchFor(msgID);
		// TODO attach callback function
		if (newMbx != -1)
			CAN0.setCallback(newMbx, cbFunction);
		
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
int CANdriver::timeStamp()
{
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
int _CanTryRead(CanasCanFrame * pframe)
{
	CAN_FRAME message;
	
	if (CAN0.read(message))
	{
		CANdriver::can2areo(pframe, &message);			
		return 0;
	}
	
	return -1;
}
//-------------------------------------------------------------------------------------------------------------------
// need to cleanup with neat timer function
//-------------------------------------------------------------------------------------------------------------------

int CANdriver::receive(CanasCanFrame * pframe, unsigned int timeout_usec)
{
	int iTimer;
	
	iTimer = millis() + timeout_usec;

	if (pframe == NULL)
		return -1;

	while (iTimer < millis())
	{
		if (_CanTryRead(pframe))
			return 0;
		delayMicroseconds(10);
	}

	return -1;
}
//-------------------------------------------------------------------------------------------------------------------
// recieve message from CAN bus
//-------------------------------------------------------------------------------------------------------------------
int CANdriver::receive(CanasCanFrame * pframe)
{	
	if (pframe == NULL)
		return -1;

	return _CanTryRead(pframe);
}

//-------------------------------------------------------------------------------------------------------------------
// recieve message from CAN bus
//-------------------------------------------------------------------------------------------------------------------
int CANdriver::receive(CanasMessage* pframe)
{
	CanasCanFrame mframe;

	if (pframe == NULL)
		return -1;

	if (_CanTryRead(&mframe) == 0)
	{
		frame2msg(pframe, &mframe);
	}
}

//-------------------------------------------------------------------------------------------------------------------
// convert Canbus message to can areo frame
//-------------------------------------------------------------------------------------------------------------------
int CANdriver::can2areo(CanasCanFrame * pframe, CAN_FRAME * message)
{
	if (message == NULL || pframe == NULL)
		return -1;
	
	for (int i = 0; i < 8; i++)
		message->data.bytes[i] = 0;

	for (int i = 0; i < message->length; i++)
		pframe->data[i] = message->data.bytes[i];

	pframe->dlc = message->length;

	pframe->id = message->id;

	if (message->extended) {
		pframe->id = message->id & CANAS_CAN_MASK_EXTID;
		pframe->id |= CANAS_CAN_FLAG_EFF;
	}
	else
		pframe->id = message->id & CANAS_CAN_MASK_STDID;

	if (message->rtr == 1)
		pframe->id |= CANAS_CAN_FLAG_RTR;


	return 0;

}

//-------------------------------------------------------------------------------------------------------------------
// convert Canbus message to can areo message
//-------------------------------------------------------------------------------------------------------------------
int CANdriver::frame2msg(CanasMessage* pmsg, CanasCanFrame * pframe)
{
	if (pmsg == NULL || pframe == NULL)
		return -1;

	for (int i = 0; i < 4; i++)
		pmsg->data.container.CHAR4[i] = 0;

	for (int i = 4; i < message->length; i++)
		pmsg->data.container.CHAR4[i] = pframe->data[i + 4];

	pmsg->node_id		= pframe->data[0]; 
	pmsg->data.type		= pframe->data[1];	
	pmsg->service_code	= pframe->data[2];
	pmsg->message_code	= pframe->data[3];
	pmsg->data.length	= pframe->dlc;
	pmsg->can_id		= pframe->id;

	return 0;

}
//-------------------------------------------------------------------------------------------------------------------
// convert CanAero message to canbus message
//-------------------------------------------------------------------------------------------------------------------
int CANdriver::areo2can(CanasCanFrame * pframe, CAN_FRAME * message)
{

	if (pframe == NULL || message == NULL)
		return -1;

	for (int i = 0; i < 8; i++)
		message->data.bytes[i] = pframe->data[i];

	message->length = pframe->dlc;
	if (pframe->id & CANAS_CAN_FLAG_EFF) {
		message->extended = true;
	}
	else {
		message->extended = false;
	}
	message->id = pframe->id;
	message->rtr = (pframe->id & CANAS_CAN_FLAG_RTR) ? 1 : 0;


	return 0;
}

