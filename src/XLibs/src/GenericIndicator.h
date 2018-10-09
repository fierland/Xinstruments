//=================================================================================================
//
// GenericIndicator.h
//
// author: Frank van Ierland
// version: 0.1
//
// Genric base class for anny instrument 2 main derived classes are :
//	- GaugeStepper
//	- GaugeDisplay
//
//=================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef GenericIndicator_h_
#define GenericIndicator_h_

#include "mydebug.h"
#include <stdlib.h>
#include "CANaero.h"
//#include "CANbus.h"


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class GenericIndicator
{
public:

//-------------------------------------------------------------------------------------------------
/**
 * Class instance
 * Must be called for every new incoming frame or by timeout.
 * @note It is recommended to call this function at least every 10 ms
 * @param [in] pi     Instance pointer
 * @param [in] iface  Interface index from which the frame was received; ignored when no frame provided
 * @param [in] pframe Pointer to the received frame, NULL when called by timeout
 * @return            @ref CanasErrorCode
 */
//-------------------------------------------------------------------------------------------------

	/**
	 * create new instrument
	 * @param [in]	CANid the CANAreo id for the instument to use
	 */
	GenericIndicator(CanasNodDefaultID CANid);
	~GenericIndicator();
 
	int startCommunication(CANaero * canObject);

	/**
	 * Update instrument with new value to display
	 * @param [in] value  new value for the instrument
	 * @return            true if succes, false if failed
	 */
	int setCanAeroId(CanasNodDefaultID newCanId);
	int getCanAeroId(CanasNodDefaultID newCanId);
	int type();


	/**
	* Calibrate the Instrument move to default start position implemented by subclass
	*/
	virtual void calibrate() = 0;
	/**
	 * Update instrument with new value to display , implemented by subclass
	 * @param [in] value  new value for the instrument
	 * @return            true if succes, false if failed
	 */
	virtual int setValue(float value) = 0;
	/**
	 * Move insturment to Off state;
	 */
	virtual void powerOff();
	/**
	 * Move insturment to On state;
	 */
	virtual void powerOn();
	virtual bool powerState(bool setOn);

	typedef enum 
	{	
		INDICATOR_UNKNOWN = 0, 
		INDICATOR_STEPPER = 1, 
		INDICATOR_DISPLAY = 2

	} IndicatorType;

protected:
	uint16_t	_CanAreoId = 0;
	float 		_currentValue = 0;
	float		_rangeMin = 0;
	float		_rangeMax = 10;
	bool		_powerOn = true;
	int			_type = INDICATOR_UNKNOWN;
	CANaero*	_myCanAreo = NULL;

};

#endif

