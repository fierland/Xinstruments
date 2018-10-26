//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================

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
#pragma once

#ifndef _INSTRUMENTMSG_H_
#define _INSTRUMENTMSG_H_


typedef enum {
	INSTR_ERR_OK = 0,

	INSTR_ERR_ARGUMENT,
	INSTR_ERR_NOT_ENOUGH_MEMORY,

	INSTR_ERR_NO_SUCH_ENTRY,
	INSTR_ERR_ENTRY_EXISTS,

	INSTR_ERR_BAD_DATA_TYPE,
	
	INSTR_ERR_QUOTA_EXCEEDED,
	INSTR_ERR_LOGIC,                ///< May be returned by a service if it goes wrong
	INSTR_ERR_NOT_RUNNING,
	INSTR_INF_NO_DATA
	
} InstrumentErrorCode;

#endif