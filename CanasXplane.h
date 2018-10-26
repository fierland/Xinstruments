//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================
// canasXplane.h
/// <summary>
/// Tranlation object from Canas codes to Xplane strings
/// </summary>
//==================================================================================================

#ifndef _CANAS_XPLANE_H
#define _CANAS_XPLANE_H
#include "Src\CanAs\Src\CAN_ids.h"

// conversions
// fuel 6 pounds / gallon
constexpr auto XP_Conv_Fuel_poundsGallon = 6;
// 1 kilogram = 2.20462262 lb
constexpr auto XP_conv_kg_lb = 2.20462262;

struct CanasXplaneTrans {
	uint8_t canasId;
	char* xplaneId;
	uint8_t	canasDataType;
	bool arrayType = false;
	uint8_t arrayPointer = 1;
};

CanasXplaneTrans const canasXplaneTable[] = {
	{CANAS_NOD_DEF_FUEL_TANK_1_QUANTITY,"sim/cockpit2/fuel/fuel_quantity",CANAS_DATATYPE_FLOAT,true,1},  // float in Kgs
	{CANAS_NOD_DEF_FUEL_TANK_2_QUANTITY,"sim/cockpit2/fuel/fuel_quantity",CANAS_DATATYPE_FLOAT,true,2},  // float in Kgs
	{CANAS_NOD_DEF_ENGINE_1_OIL_PRESSURE_ECS_CHANNEL_A,"sim/flightmodel/engine/ENGN_oil_press_psi",CANAS_DATATYPE_FLOAT,true,1}, //psi
	{CANAS_NOD_DEF_ENGINE_1_OIL_TEMPERATURE_ECS_CHANNEL_A,"sim/flightmodel/engine/ENGN_oil_temp_c",CANAS_DATATYPE_FLOAT,true,1}, //degree F
	{CANAS_NOD_DEF_FUEL_PUMP_1_FLOW_RATE,"im/cockpit2/engine/indicators/fuel_flow_kg_sec",CANAS_DATATYPE_FLOAT,true,1}, // kg/sec
	{CANAS_NOD_DEF_ENGINE_1_TURBINE_OUTLET_TEMPERATURE_ECS_CHANNEL_A,"sim/flightmodel/engine/ENGN_EGT_c",CANAS_DATATYPE_FLOAT,true,1}, //degrees F
	{CANAS_NOD_DEF_AC_SYSTEM_1_CURRENT,"Sim/cockpit2/electrical/battery_amps",CANAS_DATATYPE_FLOAT,true,1}, //amps
	{CANAS_NOD_DEF_ENGINE_1_MANIFOLD_PRESSURE_ECS_CHANNEL_A,"sim/cockpit/misc/vacuum",CANAS_DATATYPE_FLOAT,false},   //psi ?
	{0,"",0}	//zero terminator to define end of array.
};
// sim/cockpit/electrical/instrument_brightness

#endif
