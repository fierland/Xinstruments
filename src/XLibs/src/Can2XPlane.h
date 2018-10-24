//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================
// canas2Xplane.h
/// <summary>
/// Tranlation object from Canas codes to Xplane strings
/// </summary>
//==================================================================================================

#ifndef _CANAS_2XPLANE_H
#define _CANAS_2XPLANE_H
#include "Src\CanAs\Src\CAN_ids.h"
#include "src\XLibs\src\mydebug.h"

// conversions
// fuel 6 pounds / gallon
constexpr auto XP_Conv_Fuel_poundsGallon = 6;
// 1 kilogram = 2.20462262 lbDE
constexpr auto XP_conv_kg_lb = 2.20462262;

struct CanasXplaneTrans {
	uint8_t canasId;
	char* xplaneId;
	uint8_t	canasDataType;
};

//==================================================================================================
//==================================================================================================
class Can2XPlane
{
public:

	static CanasXplaneTrans const canasXplaneTable[] = {
		{CANAS_NOD_DEF_FUEL_TANK_1_QUANTITY,"sim/cockpit2/fuel/fuel_quantity[0]",CANAS_DATATYPE_FLOAT},  // float in Kgs
		{CANAS_NOD_DEF_FUEL_TANK_2_QUANTITY,"sim/cockpit2/fuel/fuel_quantity[1]",CANAS_DATATYPE_FLOAT},  // float in Kgs
		{CANAS_NOD_DEF_ENGINE_1_OIL_PRESSURE_ECS_CHANNEL_A,"sim/flightmodel/engine/ENGN_oil_press_psi[0]",CANAS_DATATYPE_FLOAT}, //psi
		{CANAS_NOD_DEF_ENGINE_1_OIL_TEMPERATURE_ECS_CHANNEL_A,"sim/flightmodel/engine/ENGN_oil_temp_c[0]",CANAS_DATATYPE_FLOAT}, //degree F
		{CANAS_NOD_DEF_FUEL_PUMP_1_FLOW_RATE,"sim/cockpit2/engine/indicators/fuel_flow_kg_sec[0]",CANAS_DATATYPE_FLOAT}, // kg/sec
		{CANAS_NOD_DEF_ENGINE_1_TURBINE_OUTLET_TEMPERATURE_ECS_CHANNEL_A,"sim/flightmodel/engine/ENGN_EGT_c[0]",CANAS_DATATYPE_FLOAT}, //degrees F
		{CANAS_NOD_DEF_AC_SYSTEM_1_CURRENT,"sim/cockpit2/electrical/battery_amps[0]",CANAS_DATATYPE_FLOAT}, //amps
		{CANAS_NOD_DEF_ENGINE_1_MANIFOLD_PRESSURE_ECS_CHANNEL_A,"sim/cockpit/misc/vacuum",CANAS_DATATYPE_FLOAT},   //psi ?
		{0,"",0}	//zero terminator to define end of array.
	};
	// sim/cockpit/electrical/instrument_brightness

	Can2XPlane();
	~Can2XPlane();

	static char* fromCan2Xplane(uint8_t canID);
};

#endif
