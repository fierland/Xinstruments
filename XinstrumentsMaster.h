//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================
// Xinstruments.h
//
// version: 0.1
//
// Main configuration file to build main instruiment server
//
/// <remark>
//
///</remark>
//===================================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef _XINSTRUMENTS_MASTER_H_
#define _XINSTRUMENTS_MASTER_H_

//===================================================================================================================
// generic defines
//===================================================================================================================
constexpr auto XI_Software_Revision = 0x01;
constexpr auto XI_Hardware_Revision = 0x01;

constexpr auto XI_Base_NodeID = 1;

#define XI_INSTRUMENT_MAIN_CONTROLER

constexpr char XI_Instrument_Code[] = "MAIN_";
constexpr auto XI_Instrument_NodeID = 1;
constexpr auto XI_Instrument_Service_Chan = 100;
constexpr auto XI_Instrument_Redun_Chan = 0;

#endif

