/*
 * CANaerospace driver interface
 * Pavel Kirienko, 2013 (pavel.kirienko@gmail.com)
 */

#ifndef CANAEROSPACE_DRIVER_H_
#define CANAEROSPACE_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include <esp32_can.h>
//#include "CANaero.h"

#define XI_CANBUS_SPEED CAN_SPEED_500KBPS

/**
 * CAN ID masks
 * @{
 */
static const uint32_t CANAS_CAN_MASK_STDID = ((uint32_t)0x000007FFu);
static const uint32_t CANAS_CAN_MASK_EXTID = ((uint32_t)0x1FFFFFFFu);
/**
 * @}
 */

/**
 * CAN flags, to be set on CAN ID
 * @{
 */
static const uint32_t CANAS_CAN_FLAG_EFF = (1 << 31);  ///< Extended frame format
static const uint32_t CANAS_CAN_FLAG_RTR = (1 << 30);  ///< Remote transmission request
/**
 * @}
 */

/**
 * CAN frame
 */
typedef struct
{
    uint8_t data[8];
    uint32_t id;      ///< Full ID (Standard + Extended) and flags (CANAS_CAN_FLAG_*)
    uint8_t dlc;      ///< Data length code
} CanasCanFrame;

/**
 * Acceptance filter configuration.
 * Use flags to filter messages by type. ref CANAS_CAN_FLAG_EFF ref CANAS_CAN_FLAG_RTR.
 */
typedef struct
{
    uint32_t id;
    uint32_t mask;
} CanasCanFilterConfig;

class CANdriver {
public:
	CANdriver();
	~CANdriver();
	
	int writeMsg(CanasCanFrame* frame);

	int setFilter(int msgID, void(*cbFunction)(CAN_FRAME *));
	int timeStamp();
	int receive(CanasCanFrame* pframe, unsigned int timeout_usec);
	int receive(CanasCanFrame* pframe);

private:
protected:
};
#endif

static void _frameSpl2Canas(const CAN_FRAME * pspl, CanasCanFrame * pcanas)
{

	memset(pcanas, 0, sizeof(*pcanas));
	memcpy(pcanas->data, pspl->Data, pspl->DLC);
	pcanas->dlc = pspl->DLC;
	if (pspl->IDE == CAN_Id_Standard)
	{
		pcanas->id = pspl->StdId & CANAS_CAN_MASK_STDID;
	}
	else
	{
		pcanas->id = pspl->ExtId & CANAS_CAN_MASK_EXTID;
		pcanas->id |= CANAS_CAN_FLAG_EFF;
	}
	if (pspl->RTR == CAN_RTR_Remote)
		pcanas->id |= CANAS_CAN_FLAG_RTR;
}
