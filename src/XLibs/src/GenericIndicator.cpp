//=================================================================================================
// Generic indicator class
//
// author: Frank van Ierland
// version: 0.1
//
//=================================================================================================

#include "GenericIndicator.h"


void GenericIndicator::_initIndicator(CanasNodDefaultID CANid) {
	// add listener to can bus
	CANareoBus.ParamSubscribe(CANid, this);
};

GenericIndicator::GenericIndicator()
{
	
}


GenericIndicator::GenericIndicator(CanasNodDefaultID CANid)
{
	_CanAreoId = CANid;
	_initIndicator(CANid)
}

GenericIndicator::~GenericIndicator()
{
	CANareoBus.ParamUnsubscribe(_CanAreoId);
}


void GenericIndicator::powerOff()
{
	DPRINTLN("Start GenericIndicator powerOff");

	_powerOn = false;

	DPRINTLN("End GenericIndicator powerOff");

}

void GenericIndicator::powerOn()
{
	DPRINTLN("Start GenericIndicator powerOn");
	_powerOn = true;
	DPRINTLN("End GenericIndicator powerOn");
}

bool GenericIndicator::powerState(bool setOn)
{
		bool oldValue = _powerOn;			

		if (setOn)
			powerOn();
		else
			powerOff();

		return oldValue;;
}

int GenericIndicator::setCanAeroId(CanasNodDefaultID newCanId)
{
	int oldVal = _CanAreoId;

	if (_CanAreoId != 0)
		CANareoBus.ParamUnsubscribe(_CanAreoId);

	_CanAreoId = newCanId;
	_initIndicator(newCanId);

	return oldVal;
}

int GenericIndicator::getCanAeroId(CanasNodDefaultID newCanId)
{
	return _CanAreoId;
}

int GenericIndicator::type()
{
	return _type;
}

