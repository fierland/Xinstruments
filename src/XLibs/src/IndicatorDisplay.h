//===================================================================================================================
// IndicatorDisplay.h
//
// author: Frank van Ierland
// version: 0.1
//
// master display indicator class 
//
//===================================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#pragma once
#include "GenericIndicator.h"

class IndicatorDisplay : public GenericIndicator
{
public:
	IndicatorDisplay(CanasNodDefaultID canID);
	~IndicatorDisplay();

	virtual void powerOn();
	virtual void powerOff();

};

