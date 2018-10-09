//=================================================================================================
//
// IndicatorDisplay.cpp
//
// author: Frank van Ierland
// version: 0.1
//
// Class to control the whole instrument
//
//=================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland
#include "IndicatorDisplay.h"


//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------IndicatorDisplay::IndicatorDisplay(CanasNodDefaultID canID) : GenericIndicator(canID)
{
	DPRINTLN("Start Display constructor");

	_type = INDICATOR_DISPLAY;

	DPRINTLN("END Display constructor");
}
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
IndicatorDisplay::~IndicatorDisplay()
{
}
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
void IndicatorDisplay::powerOn()
{
	_powerOn = true;
}
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
void IndicatorDisplay::powerOff()
{
	_powerOn = false;
}


