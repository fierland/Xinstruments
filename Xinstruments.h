//==================================================================================================
//  Franks Flightsim Intruments project
//  by Frank van Ierland
//
// This code is in the public domain.
//
//==================================================================================================

//===================================================================================================================
// Xinstruments.h
//
// author: Frank van Ierland
// version: 0.1
//
// Main configuration file to build instruments configurations
//
/// <remark>
// TODO: store type of instrument in permanent memory and ude dynamic configuring in stead of hard cofiguring by #ifdef's: then we only need 1 exec for all
// TODO: implement update of code thru canas BUS OR WIFI.
///</remark>
//===================================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef _XINSTRUMENTS_H_
#define _XINSTRUMENTS_H_

#include <Instrument.h>
#include <InstrumentMsg.h>
#include <Arduino.h>
//#define DEBUG_CLI

//===================================================================================================================
// generic defines
//===================================================================================================================
constexpr uint8_t XI_Software_Revision = 0x01;

//#define XI_Base_NodeID  1

//===================================================================================================================
// INSTRUMENT Selection: select one of this list comment out the rest
//===================================================================================================================

// 2,5" instruments
#define XI_INSTRUMENT_FUEL_GAUGE
//#define XI_INSTRUMENT_OIL_GAUGE
//#define XI_INSTRUMENT_EXHFLOW_GAUGE
//#define XI_INSTRUMENT_VACAM_GAUGE

//TODO: Support gauges below

//#define XI_INSTRUMENT_DIG_CLOCK

// 3,25" instruments
//#define XI_INSTRUMENT_AIRSPEED_IND
//#define XI_INSTRUMENT_MAG_COMPASS
//#define XI_INSTRUMENT_ATTITUDE_IND
//#define XI_INSTRUMENT_ALTIMETER
//#define XI_INSTRUMENT_GLIDESCOPE
//#define XI_INSTRUMENT_TURN_IND
//#define XI_INSTRUMENT_DIRECTIONAL_GYRO
//#define XI_INSTRUMENT_VERTICAL_SPEED_IND
//#define XI_INSTRUMENT_CDI_VOR
//#define XI_INSTRUMENT_TACHOMETER
//#define XI_INSTRUMENT_ADF_BEARING_IND

//-------------------------------------------------------------------------------------------------------------------
#ifdef XI_INSTRUMENT_FUEL_GAUGE

XI_instrumentDescription myInstrumentInfo = {
	"FEULG",
	XI_CONTROLER_25,
	1,
	XI_DUAL_STEPPER_25,
	1,
	1,
	1,
	0,
	{
		{CANAS_NOD_DEF_FUEL_TANK_1_QUANTITY,XI_INDICATOR_STEPPER_PIE,1,120,0,26,0.3674371033,-10},
		{CANAS_NOD_DEF_FUEL_TANK_2_QUANTITY,XI_INDICATOR_STEPPER_PIE,2,120,0,26,0.3674371033,-10},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0}
	}
};
#endif

//-------------------------------------------------------------------------------------------------------------------
#ifdef  XI_INSTRUMENT_OIL_GAUGE

XI_instrumentDescription myInstrumentInfo = {
	"OILGA",
	XI_CONTROLER_25,
	1,
	XI_DUAL_STEPPER_25,
	1,
	1,
	8,
	0,
	{
		{CANAS_NOD_DEF_ENGINE_1_OIL_PRESSURE_ECS_CHANNEL_A,	XI_INDICATOR_STEPPER_360,1,120, 0,115,1,-10},
		{CANAS_NOD_DEF_ENGINE_1_OIL_TEMPERATURE_ECS_CHANNEL_A,	XI_INDICATOR_STEPPER_360,2,120,75,245,1,-10},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0}
	}
};

#endif

//-------------------------------------------------------------------------------------------------------------------
#ifdef  XI_INSTRUMENT_EXHFLOW_GAUGE

XI_instrumentDescription myInstrumentInfo = {
	"EXHFL",
	XI_CONTROLER_25,
	1,
	XI_DUAL_STEPPER_25,
	1,
	1,
	8,
	0,
	{
		{CANAS_NOD_DEF_FUEL_PUMP_1_FLOW_RATE,								XI_INDICATOR_STEPPER_360,1,120,0, 19,1,-10},
		{CANAS_NOD_DEF_ENGINE_1_TURBINE_OUTLET_TEMPERATURE_ECS_CHANNEL_A,	XI_INDICATOR_STEPPER_360,2,120,0,400,1,-10},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0}
	}
};

#endif

//-------------------------------------------------------------------------------------------------------------------
#ifdef  XI_INSTRUMENT_VACAM_GAUGE

XI_instrumentDescription myInstrumentInfo = {
	"VACAM",
	XI_CONTROLER_25,
	1,
	XI_DUAL_STEPPER_25,
	1,
	1,
	1,
	0,
	{
		{CANAS_NOD_DEF_AC_SYSTEM_1_CURRENT,						XI_INDICATOR_STEPPER,1,120,-60,60,1,-10},
		{CANAS_NOD_DEF_ENGINE_1_MANIFOLD_PRESSURE_ECS_CHANNEL_A,XI_INDICATOR_STEPPER,2,120,  3, 7,1,-10},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0}
}
};

#endif
//-------------------------------------------------------------------------------------------------------------------
#ifdef  XI_INSTRUMENT_DIG_CLOCK

// TODO : make definition
XI_instrumentDescription myInstrumentInfo = {
	"DICLK",
	XI_CONTROLER_25,
	1,
	XI_DUAL_STEPPER_25,
	1,
	1,
	1,
	0,
	{
		{CANAS_NOD_DEF_DATE,XI_INDICATOR_DISPLAY,1,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0},
		{CANAS_NOD_NOVALUE,XI_INDICATOR_NONE,0,0,0,0,0,0}
	 }
};

#endif

//-------------------------------------------------------------------------------------------------------------------
// codes not used in instruments yet :-)
//-------------------------------------------------------------------------------------------------------------------

// set these flags if the instrument has a Power and ready indication flag controled by a servo
//#define USE_PWR_FLAG_SERVO

//#define PWR_FLAG_SERVO_PIN		XI_SERVO1_PIN
//#define PWR_FLAG_SERVO_MAXPOS	50

//#define XI_STEP3_360
//#define XI_STEP3_PIE
//#define XI_STEP3_MIN_PIE 0
//#define XI_STEP3_MAX_PIE 180
//#define XI_STEP3_MIN_RANGE 0
//#define XI_STEP3_MAX_RANGE 360
//#define XI_STEP3_ITEM "GYRO_HEADING"

//#define XI_STEP4_360
//#define XI_STEP4_PIE
//#define XI_STEP4_MIN_PIE 0
//#define XI_STEP4_MAX_PIE 180
//#define XI_STEP4_MIN_RANGE 0
//#define XI_STEP4_MAX_RANGE 360
//#define XI_STEP4_ITEM "GYRO_HEADING"

//define XI_RE1_ITEM "GYRO_HEADING"
//define XI_RE2_ITEM "GYRO_HEADING"

//===================================================================================================================
// HARDWARE CONFIGURATION VALIDATION SECTION
//===================================================================================================================
/*
#if (defined(XI_CONTROLER_35) && defined(XI_CONTROLER_25)) ||\
	(defined(XI_CONTROLER_35) && defined(XI_CONTROLER_MAIN)) ||\
	(defined(XI_CONTROLER_MAIN) && defined(XI_CONTROLER_25))
#error Only one controler board type can be selected
#endif

#if (defined(XI_QUAD_STEPPER_35) && defined(XI_DUAL_STEPPER_35)) || \
	(defined(XI_QUAD_STEPPER_35) && defined(XI_DUAL_STEPPER_25)) || \
	(defined(XI_DUAL_STEPPER_25) && defined(XI_DUAL_STEPPER_35))
#error Only one stepper board type can be selected
#endif

#if	(defined(XI_CONTROLER_25) && defined(XI_DUAL_STEPPER_35)) || \
	(defined(XI_CONTROLER_25) && defined(XI_QUAD_STEPPER_35)) || \
	(defined(XI_CONTROLER_35) && defined(XI_DUAL_STEPPER_25))
#error wrong mix of stepper board & controler
#endif

#endif
*/

//-------------------------------------------------------------------------------------------------------------------
//
//	Settings for the 3,25" controler board
//
//-------------------------------------------------------------------------------------------------------------------

#ifdef __XI_CONTROLER_35
constexpr uint8_t XI_Hardware_Revision = 0x01;  //revision always first byte 0;

#define XI_CANBUS_RX 16
#define XI_CANBUS_TX 17

//set the max numbers of stepper motors and servos in the instrument
#define XI_MAX_STEPPERS		4
#define XI_MAX_SERVO		2
#define XI_MAX_HAL			2
#define XI_MAX_RE		    2
#define XI_LED_PIN			GPIO_NUM_12

#define XI_SERVO1_PIN 18
#define XI_SERVO2_PIN 19
#define XI_SERVO3_PIN 20

#define XI_HAL1_PIN	21
#define XI_HAL2_PIN 22

#define XI_RE1_A_PIN 11
#define XI_RE1_B_PIN 11
#define XI_RE1_SW_PIN 11

#define XI_RE2_A_PIN 11
#define XI_RE2_B_PIN 11
#define XI_RE2_SW_PIN 11

#define XI_PWR_MON_PIN 39

#endif

//-------------------------------------------------------------------------------------------------------------------
//	Settings for the 2,5" controler board
//-------------------------------------------------------------------------------------------------------------------

#ifdef __XI_CONTROLER_25
constexpr uint8_t XI_Hardware_Revision = 0x11;  //revision always first byte 1;

#define XI_CANBUS_TX 16
#define XI_CANBUS_RX 17

//set the max numbers of stepper motors and servos in the instrument
#define XI_MAX_STEPPERS		2
#define XI_MAX_SERVO		2
#define XI_MAX_HAL			0
#define XI_MAX_RE		    0

#define XI_LED_PIN      GPIO_NUM_12
// set the pin mappings for the stepper motors
// step A - 7 - 23
// dir A - 8 - 19
//step b - 9 - 25
//dir b - 10 - 33
/*connector pin 8*/
#define XI_STEP1_DIR GPIO_NUM_19
//connector pin 7
#define XI_STEP1_STP GPIO_NUM_23
//connector pin 10
#define XI_STEP2_DIR GPIO_NUM_33
//connector pin 9
#define XI_STEP2_STP GPIO_NUM_25
#define XI_STEP3_DIR -1
#define XI_STEP3_STP -1
#define XI_STEP4_DIR -1
#define XI_STEP4_STP -1

#define XI_SERVO1_PIN GPIO_NUM_25
#define XI_SERVO2_PIN GPIO_NUM_26
#define XI_SERVO3_PIN -1

#define XI_HAL1_PIN	-1
#define XI_HAL2_PIN -1

#define XI_RE1_A_PIN -1
#define XI_RE1_B_PIN -1
#define XI_RE1_SW_PIN -1

#define XI_RE2_A_PIN -1
#define XI_RE2_B_PIN -1
#define XI_RE2_SW_PIN -1

#define XI_PWR_MON_PIN -1

#endif
//-------------------------------------------------------------------------------------------------------------------
//Stepper board settings
//-------------------------------------------------------------------------------------------------------------------

// generic hardware settings

#ifdef __XI_QUAD_STEPPER_35
// specs voor quad stepper board
#define	XI_STEP1_MOTORTYPE IndicatorStepper::TYPE_BKA30
#define	XI_STEP2_MOTORTYPE IndicatorStepper::TYPE_BKA30
#define	XI_STEP3_MOTORTYPE IndicatorStepper::TYPE_BKA30
#define	XI_STEP4_MOTORTYPE IndicatorStepper::TYPE_BKA30
#define XI_STEP1_REVERSED false
#define XI_STEP2_REVERSED false
#define XI_STEP3_REVERSED false
#define XI_STEP4_REVERSED false
#endif

#ifdef __XI_DUAL_STEPPER_35
// specs voor quad stepper board
#define	XI_STEP1_MOTORTYPE IndicatorStepper::TYPE_BKA30
#define	XI_STEP2_MOTORTYPE IndicatorStepper::TYPE_BKA30
#define XI_STEP1_REVERSED false
#define XI_STEP2_REVERSED false

#endif

#ifdef __XI_DUAL_STEPPER_25
// specs voor 2'5" dual stepper board
#define	XI_STEP1_MOTORTYPE IndicatorStepper::TYPE_BKA30
#define	XI_STEP2_MOTORTYPE IndicatorStepper::TYPE_BKA30

#define XI_STEP1_REVERSED false
#define XI_STEP2_REVERSED true
#endif
//===================================================================================================================
// INSTRUMENT SPECIFIC SETTINGS - HARDWARE
//===================================================================================================================
#ifdef __XI_INSTRUMENT_FUEL_GAUGE
#define XI_CONTROLER_25
#define XI_DUAL_STEPPER_25
#endif

#ifdef  __XI_INSTRUMENT_OIL_GAUGE
#define XI_CONTROLER_25
#define XI_DUAL_STEPPER_25
#endif

#ifdef  __XI_INSTRUMENT_EXHFLOW_GAUGE
#define XI_CONTROLER_25
#define XI_DUAL_STEPPER_25
#endif

#ifdef  __XI_INSTRUMENT_VACAM_GAUGE
#define XI_CONTROLER_25
#define XI_DUAL_STEPPER_25
#endif

#ifdef  __XI_INSTRUMENT_MAIN_CONTROLER
#define XI_CONTROLER_MAIN
#endif
//-------------------------------------------------------------------------------------------------------------------
// define all hardware interfaces for the pins
//	Curently i have designed 2 controler boards one for 3,5" instruments and one for 2,5" instruments
//-------------------------------------------------------------------------------------------------------------------

// controler type used
//#define XI_CONTROLER_35
//#define XI_CONTROLER_25

//-------------------------------------------------------------------------------------------------------------------
// define which stepper board is used
//-------------------------------------------------------------------------------------------------------------------

//#define XI_QUAD_STEPPER_35
//#define XI_DUAL_STEPPER_35
//#define XI_DUAL_STEPPER_25

//===================================================================================================================
// BOARD SPECIFIC SETTINGS - HARDWARE - should need no modification
//===================================================================================================================
//-------------------------------------------------------------------------------------------------------------------
//
//  Settings for the OLIMEX ESP32-EVB controler
//
//-------------------------------------------------------------------------------------------------------------------

#ifdef __XI_CONTROLER_MAIN
constexpr uint8_t XI_Hardware_Revision = 0XF1;

#define XI_CANBUS_TX  5
#define XI_CANBUS_RX 35

#define XI_MAX_STEPPERS 0
#define XI_MAX_SERVO    0
#define XI_MAX_HAL      0
#define XI_MAX_RE       0

#endif

#endif