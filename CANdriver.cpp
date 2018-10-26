//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================
// VERSION HISTORY:
//
//==================================================================================================
// A light implementation of the CAN Aerospace protocol to manage simulated instruments.
//	This code is in the public domain.
//
// Thanks to mjs513/CANaerospace (Pavel Kirienko, 2013 (pavel.kirienko@gmail.com))
//-------------------------------------------------------------------------------------------------------------------
#include <QList.h>
#include <stdlib.h>
#include <sys/config.h>
#include "CANdriver.h"
#include <sys/time.h>
#include "mydebug.h"
/*
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
  CANAS_ERR_NOT_RUNNING,
  CANAS_INF_NO_DATA
 */
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
// marchall stuff
//-------------------------------------------------------------------------------------------------------------------
#define IS_UDEF(type) ((type) >= CANAS_DATATYPE_UDEF_BEGIN_ && (type) <= CANAS_DATATYPE_UDEF_END_)

#ifdef __IEEE_BIG_ENDIAN
//#if __BYTE_ORDER__ == __BIG_ENDIAN
#  define SWITCH(x)
#else

static inline void _switchByteOrder(void* ptr, int len)
{
	uint8_t tmp[4], *pbyte = (uint8_t*)ptr;
	if (len == 2)
	{
		memcpy(tmp, ptr, 2);
		pbyte[0] = tmp[1];
		pbyte[1] = tmp[0];
	}
	else if (len == 4)
	{
		memcpy(tmp, ptr, 4);
		pbyte[0] = tmp[3];
		pbyte[1] = tmp[2];
		pbyte[2] = tmp[1];
		pbyte[3] = tmp[0];
	}
}

#  define SWITCH(x) _switchByteOrder(&(x), sizeof(x))
#endif

#define MARSHAL_RESULT_ERROR -1
#define MARSHAL_RESULT_UDEF  -2
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

static int _marshal(CanasMessageData* pmsg)
{
	switch (pmsg->type)
	{
	case CANAS_DATATYPE_NODATA:
		return 0;

	case CANAS_DATATYPE_FLOAT:
		/*
		 * Target platform must support IEEE754 floats. The good news that almost every platform does that.
		 * But if some platform doesn't, proper converting from native float
		 * representation to IEEE754 (and vice versa) must be implemented somewhere here.
		 */
		 // FALLTHROUGH
	case CANAS_DATATYPE_ERROR:
	case CANAS_DATATYPE_LONG:
	case CANAS_DATATYPE_ULONG:
	case CANAS_DATATYPE_BLONG:
		SWITCH(pmsg->container.ULONG);
		return 4;

	case CANAS_DATATYPE_SHORT:
	case CANAS_DATATYPE_USHORT:
	case CANAS_DATATYPE_BSHORT:
		SWITCH(pmsg->container.USHORT);
		return 2;

	case CANAS_DATATYPE_CHAR:
	case CANAS_DATATYPE_UCHAR:
	case CANAS_DATATYPE_BCHAR:
		return 1;

	case CANAS_DATATYPE_SHORT2:
	case CANAS_DATATYPE_USHORT2:
	case CANAS_DATATYPE_BSHORT2:
		SWITCH(pmsg->container.USHORT2[0]);
		SWITCH(pmsg->container.USHORT2[1]);
		return 4;

	case CANAS_DATATYPE_CHAR4:
	case CANAS_DATATYPE_UCHAR4:
	case CANAS_DATATYPE_BCHAR4:
		return 4;

	case CANAS_DATATYPE_CHAR2:
	case CANAS_DATATYPE_UCHAR2:
	case CANAS_DATATYPE_BCHAR2:
		return 2;

	case CANAS_DATATYPE_MEMID:
	case CANAS_DATATYPE_CHKSUM:
		SWITCH(pmsg->container.MEMID);
		return 4;

	case CANAS_DATATYPE_ACHAR:
		return 1;

	case CANAS_DATATYPE_ACHAR2:
		return 2;

	case CANAS_DATATYPE_ACHAR4:
		return 4;

	case CANAS_DATATYPE_CHAR3:
	case CANAS_DATATYPE_UCHAR3:
	case CANAS_DATATYPE_BCHAR3:
	case CANAS_DATATYPE_ACHAR3:
		return 3;

	case CANAS_DATATYPE_DOUBLEH:
	case CANAS_DATATYPE_DOUBLEL:
		// See note about IEEE754 compatibility above.
		SWITCH(pmsg->container.DOUBLEL);
		return 4;

	default:
		if (IS_UDEF(pmsg->type) && pmsg->length <= 4)
			return MARSHAL_RESULT_UDEF;
		DPRINT("marshal: unknown data type ");
		DPRINT(pmsg->type);
		DPRINT(", udf_len = ");
		DPRINTLN(pmsg->length);
		return MARSHAL_RESULT_ERROR;
	}
}
void printFrame(CAN_FRAME *message)
{
  Serial.print(message->id, HEX);
  if (message->extended) Serial.print(" X ");
  else Serial.print(" S ");   
  Serial.print(message->length, DEC);
  for (int i = 0; i < message->length; i++) {
    Serial.print(message->data.byte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
//-------------------------------------------------------------------------------------------------------------------
// initiator
//-------------------------------------------------------------------------------------------------------------------
CANdriver::CANdriver()
{
	CAN0.begin(XI_CANBUS_SPEED);
	CAN0.watchFor();
}

//-------------------------------------------------------------------------------------------------------------------
// destructor and close CAN bus
//-------------------------------------------------------------------------------------------------------------------
CANdriver::~CANdriver()
{
	CAN0.disable();
}
//-------------------------------------------------------------------------------------------------------------------
// write message to canbus
//-------------------------------------------------------------------------------------------------------------------

int CANdriver::writeMsg(CanasCanFrame* frame)
{
	CAN_FRAME message;

	if (frame == NULL)
		return -CANAS_ERR_ARGUMENT;

	for (int i = 0; i < 8; i++)
		message.data.bytes[i] = frame->data[i];

	message.length = frame->dlc;
	if (frame->id & CANAS_CAN_FLAG_EFF)
	{
		message.extended = true;
	}
	else
	{
		message.extended = false;
	}
	message.id = frame->id;
	message.rtr = (frame->id & CANAS_CAN_FLAG_RTR) ? 1 : 0;
	CAN0.sendFrame(message);

	return CANAS_ERR_OK;
}

//-------------------------------------------------------------------------------------------------------------------
// set filter on canbus
//-------------------------------------------------------------------------------------------------------------------

int CANdriver::setFilter(int msgID, void(*cbFunction)(CAN_FRAME *))
{
	int newMbx;

	newMbx = CAN0.watchFor(msgID);
	// TODO attach callback function
	if (newMbx != -1)
		CAN0.setCallback(newMbx, cbFunction);

  return CANAS_ERR_OK;
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
int64_t CANdriver::timeStamp()
{
	timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
int _CanTryRead(CanasCanFrame * pframe)
{
	CAN_FRAME message;
DPRINTINFO("START");

  memset(&message,0,sizeof(CAN_FRAME));
	if (CAN0.read(message))
	{
    DPRINTLN("read success");
#ifdef MACRO_DEBUG
    printFrame(&message);
#endif
		CANdriver::can2areo(pframe, &message);
		return CANAS_ERR_OK;
	}
DPRINTINFO("STOP");
	return -CANAS_INF_NO_DATA;
}
//-------------------------------------------------------------------------------------------------------------------
// need to cleanup with neat timer function
//-------------------------------------------------------------------------------------------------------------------

int CANdriver::receive(CanasCanFrame * pframe, unsigned int timeout_usec)
{
	int iTimer;
DPRINTINFO("START");

	iTimer = millis() + timeout_usec;

	if (pframe == NULL)
		return -CANAS_ERR_ARGUMENT;

	while (iTimer < millis())
	{
		if (_CanTryRead(pframe)==0){
      DPRINTLN("frame found");
      return CANAS_ERR_OK;
		}
			
		delayMicroseconds(10);
	};
  
DPRINTINFO("STOP");
	return -CANAS_INF_NO_DATA;
}
//-------------------------------------------------------------------------------------------------------------------
// recieve message from CAN bus
//-------------------------------------------------------------------------------------------------------------------
int CANdriver::receive(CanasCanFrame * pframe)
{
  DPRINTINFO("START");
 
 if (pframe == NULL)
		return -CANAS_ERR_ARGUMENT;
    
  DPRINTINFO("STOP");
	return _CanTryRead(pframe);
}

//-------------------------------------------------------------------------------------------------------------------
// recieve message from CAN bus
//-------------------------------------------------------------------------------------------------------------------
int CANdriver::receive(CanasMessage* pframe)
{
	CanasCanFrame mframe;
 DPRINTINFO("START");
 
	if (pframe == NULL)
		return -CANAS_ERR_ARGUMENT;

	if (_CanTryRead(&mframe) == 0)
	{
		frame2msg(pframe, &mframe);
    return CANAS_ERR_OK;
	}
 
 DPRINTINFO("STOP");
 return -CANAS_INF_NO_DATA;
}

//-------------------------------------------------------------------------------------------------------------------
// convert Canbus message to can areo frame
//-------------------------------------------------------------------------------------------------------------------
int CANdriver::can2areo(CanasCanFrame * pframe, CAN_FRAME * message)
{
  DPRINTINFO("START");
  
	if (message == NULL || pframe == NULL)
		return -CANAS_ERR_ARGUMENT;

	for (int i = 0; i < 8; i++)
		message->data.bytes[i] = 0;

	for (int i = 0; i < message->length; i++)
		pframe->data[i] = message->data.bytes[i];

	pframe->dlc = message->length;

	pframe->id = message->id;

	if (message->extended)
	{
		pframe->id = message->id & CANAS_CAN_MASK_EXTID;
		pframe->id |= CANAS_CAN_FLAG_EFF;
	}
	else
		pframe->id = message->id & CANAS_CAN_MASK_STDID;

	if (message->rtr == 1)
		pframe->id |= CANAS_CAN_FLAG_RTR;
   
  DPRINTINFO("STOP");
	return CANAS_ERR_OK;
}

//-------------------------------------------------------------------------------------------------------------------
// convert Canbus message to can areo message
//-------------------------------------------------------------------------------------------------------------------
int CANdriver::frame2msg(CanasMessage* pmsg, CanasCanFrame * pframe)
{
	if (pmsg == NULL || pframe == NULL)
		return -CANAS_ERR_ARGUMENT;

	for (int i = 0; i < 4; i++)
		pmsg->data.container.CHAR4[i] = 0;

	for (int i = 4; i < pframe->dlc; i++)
		pmsg->data.container.CHAR4[i] = pframe->data[i + 4];

	pmsg->node_id = pframe->data[0];
	pmsg->data.type = pframe->data[1];
	pmsg->service_code = pframe->data[2];
	pmsg->message_code = pframe->data[3];
	pmsg->data.length = pframe->dlc;
	pmsg->can_id = pframe->id;

	return CANAS_ERR_OK;
}
int CANdriver::msg2frame(CanasCanFrame * pframe, uint16_t msg_id, const CanasMessage * pmsg)
{
	memset(pframe, 0, sizeof(*pframe));
	pframe->id = msg_id & CANAS_CAN_MASK_STDID;
	pframe->dlc = sizeof(CanasMessage);
	pframe->data[0] = pmsg->node_id;
	pframe->data[1] = pmsg->data.type;
	pframe->data[2] = pmsg->service_code;
	pframe->data[3] = pmsg->message_code;
	int datalen = canasHostToNetwork(pframe->data + 4, &pmsg->data);
	if (datalen < 0)
	{
		DPRINT("framemaker: bad data type=");
		DPRINT(pmsg->data.type);
		DPRINT(" error = ");
		DPRINTLN(datalen);
		return datalen;
	};

	pframe->dlc = datalen + 4;

	return CANAS_ERR_OK;
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

int CANdriver::canasHostToNetwork(uint8_t * pdata, const CanasMessageData * phost)
{
	if (pdata == NULL || phost == NULL)
		return -CANAS_ERR_ARGUMENT;

	CanasMessageData nwk = *phost;
	const int ret = _marshal(&nwk);
	if (ret >= 0)
	{
		memcpy(pdata, &nwk.container, ret);
		return ret;
	}
	if (ret == MARSHAL_RESULT_UDEF)
	{
		memcpy(pdata, &nwk.container, nwk.length);
		return nwk.length;
	}
	return -CANAS_ERR_BAD_DATA_TYPE;
	
}
//-------------------------------------------------------------------------------------------------------------------
// convert CanAero message to canbus message
//-------------------------------------------------------------------------------------------------------------------
int CANdriver::areo2can(CanasCanFrame * pframe, CAN_FRAME * message)
{
	if (pframe == NULL || message == NULL)
		return -CANAS_ERR_ARGUMENT;

	for (int i = 0; i < 8; i++)
		message->data.bytes[i] = pframe->data[i];

	message->length = pframe->dlc;
	if (pframe->id & CANAS_CAN_FLAG_EFF)
	{
		message->extended = true;
	}
	else
	{
		message->extended = false;
	}
	message->id = pframe->id;
	message->rtr = (pframe->id & CANAS_CAN_FLAG_RTR) ? 1 : 0;

	return CANAS_ERR_OK;
}
