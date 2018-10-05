#include "IndicatorDisplay.h"



IndicatorDisplay::IndicatorDisplay(CanasNodDefaultID canID) : GenericIndicator(canID)
{
	DPRINTLN("Start Display constructor");

	_type = INDICATOR_DISPLAY;

	DPRINTLN("END Display constructor");
}

IndicatorDisplay::~IndicatorDisplay()
{
}

void IndicatorDisplay::powerOn()
{
	_powerOn = true;
}

void IndicatorDisplay::powerOff()
{
	_powerOn = false;
}


