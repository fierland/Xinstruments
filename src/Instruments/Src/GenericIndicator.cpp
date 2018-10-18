//=================================================================================================
// Generic indicator class
//
// author: Frank van Ierland
// version: 0.1
//
//=================================================================================================

#include "GenericIndicator.h"

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
GenericIndicator::GenericIndicator(CanasNodDefaultID CANid)
{
	DPRINTINFO("START");
	_CanAreoId = CANid;
	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
GenericIndicator::~GenericIndicator()
{
	DPRINTINFO("START");
	_myCanAreo->ParamUnsubscribe(_CanAreoId);
	DPRINTINFO("STOP");
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
int GenericIndicator::startCommunication(CANaero * canObject)
{
	DPRINTINFO("START");
	_myCanAreo = canObject;
	_myCanAreo->ParamSubscribe(_CanAreoId, this);

	DPRINTINFO("STOP");

	return 0;
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GenericIndicator::powerOff()
{
	DPRINTINFO("START");

	_powerOn = false;

	DPRINTINFO("STOP");
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GenericIndicator::powerOn()
{
	DPRINTINFO("START");
	_powerOn = true;

	DPRINTINFO("STOP");
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
bool GenericIndicator::powerState(bool setOn)
{
	DPRINTINFO("START");
	bool oldValue = _powerOn;

	if (setOn)
		powerOn();
	else
		powerOff();

	DPRINTINFO("STOP");

	return oldValue;;
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
int GenericIndicator::setCanAeroId(CanasNodDefaultID newCanId)
{
	DPRINTINFO("START");
	int oldVal = _CanAreoId;

	if (_CanAreoId != 0)
		_myCanAreo->ParamUnsubscribe(_CanAreoId);

	_CanAreoId = newCanId;
	_initIndicator(newCanId);

	DPRINTINFO("STOP");

	return oldVal;
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
int GenericIndicator::getCanAeroId(CanasNodDefaultID newCanId)
{
	DPRINTINFO("START");

	DPRINTINFO("STOP");

	return _CanAreoId;
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
int GenericIndicator::type()
{
	DPRINTINFO("START");
	DPRINTINFO("STOP");

	return _type;
}