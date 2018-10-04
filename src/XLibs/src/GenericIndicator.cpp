//=================================================================================================
//=================================================================================================

#include "GenericIndicator.h"


void GenericIndicator::_initIndicator(uint16_t CANid) {
	// add listener to can bus
	CANareoBus.ParamSubscribe(CANid, this);
};

GenericIndicator::GenericIndicator()
{
	
}


GenericIndicator::GenericIndicator(uint16_t CANid)
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

int GenericIndicator::setCanAeroId(uint16_t newCanId)
{
	int oldVal = _CanAreoId;

	if (_CanAreoId != 0)
		CANareoBus.ParamUnsubscribe(_CanAreoId);

	_CanAreoId = newCanId;
	_initIndicator(newCanId);

	return oldVal;
}

