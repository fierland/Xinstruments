//===================================================================================================================
// Xinstruments.h
//
// author: Frank van Ierland
// version: 0.1
//
// Main configuration file to build instruments configurations 
//
//===================================================================================================================
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef XINSTRUMENTS_H_
#define XINSTRUMENTS_H_

#include "src\XLibs\src\mydebug.h"
#include "src\XLibs\src\XServo.h"
#include "src\XLibs\src\StepperPie.h"
#include "src\XLibs\src\Stepper360.h"
#include "src\XLibs\src\XPUtils.h"
#include "src\XLibs\src\Xcomm.h"

#define DEBUG_CLI

#ifdef DEBUG_CLI

#endif


//===================================================================================================================
// INSTRUMENT Selection: select one of this list comment out the rest
//===================================================================================================================

#define XI_INSTRUMENT_FUEL_GAUGE
//#define XI_INSTRUMENT_OIL_GAUGE
//#define XI_INSTRUMENT_EXHFLOW_GAUGE
//#define XI_INSTRUMENT_VACAM_GAUGE

//#define XI_INSTRUMENT_DIG_CLOCK

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

//===================================================================================================================
// data element definitions for communication
//===================================================================================================================
#define XP_WET_COMPASS		"CMPWET"
// 0/360
#define XP_GYRO_DIRECTION	"GYRDIR"
// 0/360
#define XP_GYRO_HEADING		"GIRHEA"
// 0/360
#define XP_ALTITUTE			"ALTITU"
// 0/??
#define XP_ALT_BARO			"ALTBAR"
// ??/??
#define XP_FUEL_RIGHT		"FUELRI"
//	0/32
#define XP_FUEL_LEFT		"FUELLE"
//	0/32
#define XP_OIL_PRESSURE		"OILPRE"
//	0/115
#define XP_OIL_TEMP			"OILTMP"
//	75/245
#define XP_EGT_TEMP			"EGTTMP"
//	0/400
#define XP_FUEL_FLOW		"FUELFL"
//	0/19
#define XP_VACUEM			"VACUM"
//	3/7
#define XP_AMPERES			"PWRAMP"
//	-60/60


//===================================================================================================================
// INSTRUMENT SPECIFIC SETTINGS - HARDWARE
//===================================================================================================================
#ifdef XI_INSTRUMENT_FUEL_GAUGE
#define XI_CONTROLER_25
#define XI_DUAL_STEPPER_25
#endif

#ifdef  XI_INSTRUMENT_OIL_GAUGE
#define XI_CONTROLER_25
#define XI_DUAL_STEPPER_25
#endif

#ifdef  XI_INSTRUMENT_EXHFLOW_GAUGE
#define XI_CONTROLER_25
#define XI_DUAL_STEPPER_25
#endif

#ifdef  XI_INSTRUMENT_VACAM_GAUGE
#define XI_CONTROLER_25
#define XI_DUAL_STEPPER_25
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
//	Settings for the 3,5" controler board
//
//-------------------------------------------------------------------------------------------------------------------

#ifdef XI_CONTROLER_35

//set the max numbers of stepper motors and servos in the instrument
#define XI_MAX_STEPPERS 4
#define XI_MAX_SERVO	3
#define XI_MAX_HAL		2
#define XI_MAX_RE		2

// set the pin mappings for the stepper motors
	#define XI_STEP1_DIR 1
	#define XI_STEP1_STP 2
	#define XI_STEP2_DIR 3
	#define XI_STEP2_STP 4
	#define XI_STEP3_DIR 5
	#define XI_STEP3_STP 6
	#define XI_STEP4_DIR 7
	#define XI_STEP4_STP 8

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
//	Settings for the 3,5" controler board
//-------------------------------------------------------------------------------------------------------------------

#ifdef XI_CONTROLER_25

//set the max numbers of stepper motors and servos in the instrument
#define XI_MAX_STEPPERS 2
#define XI_MAX_SERVO	3
#define XI_MAX_HAL		0
#define XI_MAX_RE		0

// set the pin mappings for the stepper motors
// step A - 7 - 23
// dir A - 8 - 19
//step b - 9 - 25
//dir b - 10 - 33
/*connector pin 8*/
#define XI_STEP1_DIR 19
//connector pin 7
#define XI_STEP1_STP 23
//connector pin 10
#define XI_STEP2_DIR 33
//connector pin 9
#define XI_STEP2_STP 25
#define XI_STEP3_DIR -1
#define XI_STEP3_STP -1
#define XI_STEP4_DIR -1
#define XI_STEP4_STP -1

#define XI_SERVO1_PIN 12
#define XI_SERVO2_PIN 25
#define XI_SERVO3_PIN 26

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

// for VID29 and BKA30 steppers using a vid66 driver
// theorecticlay 360*12
<<<<<<< HEAD
#define XI_BKA30_STEPS_CIRCLE 4320
#define XI_BKA30_STEPS_MAXSPEED 5000
#define XI_BKA30_STEPS_ACCELERATION 300
#define XI_BKA30_STEPS_MINPULSEWIDTH 200
=======
#define XI_BKA30_STEPS_CIRCLE 900
#define XI_BKA30_STEPS_MAXSPEED 1000
#define XI_BKA30_STEPS_ACCELERATION 100
#define XI_BKA30_STEPS_MINPULSEWIDTH 4
>>>>>>> 9ae1e597c7d566f0290c51dd73e4281a1b8a47df

// for 28BYJ steppers
#define XI_28BYJ_STEPS_CIRCLE 2400
#define XI_28BYJ_STEPS_MAXSPEED 5000
#define XI_28BYJ_STEPS_ACCELERATION 300
<<<<<<< HEAD
#define XI_28BYJ_STEPS_MINPULSEWIDTH 200
=======
#define XI_28BYJ_STEPS_MINPULSEWIDTH 4
>>>>>>> 9ae1e597c7d566f0290c51dd73e4281a1b8a47df




#ifdef XI_QUAD_STEPPER_35
// specs voor quad stepper board
#define XI_STEP1_STEPS_CIRCLE XI_BKA30_STEPS_CIRCLE
#define XI_STEP2_STEPS_CIRCLE XI_BKA30_STEPS_CIRCLE
#define XI_STEP3_STEPS_CIRCLE XI_BKA30_STEPS_CIRCLE
#define XI_STEP4_STEPS_CIRCLE XI_BKA30_STEPS_CIRCLE

#define XI_STEP1_MAX_SPEED XI_BKA30_STEPS_MAXSPEED
#define XI_STEP2_MAX_SPEED XI_BKA30_STEPS_MAXSPEED
#define XI_STEP3_MAX_SPEED XI_BKA30_STEPS_MAXSPEED
#define XI_STEP4_MAX_SPEED XI_BKA30_STEPS_MAXSPEED

#define XI_STEP1_ACCEL XI_BKA30_STEPS_ACCELERATION
#define XI_STEP2_ACCEL XI_BKA30_STEPS_ACCELERATION
#define XI_STEP3_ACCEL XI_BKA30_STEPS_ACCELERATION
#define XI_STEP4_ACCEL XI_BKA30_STEPS_ACCELERATION

#define XI_STEP1_MINPULSEWIDTH XI_BKA30_STEPS_MINPULSEWIDTH
#define XI_STEP2_MINPULSEWIDTH XI_BKA30_STEPS_MINPULSEWIDTH
#define XI_STEP3_MINPULSEWIDTH XI_BKA30_STEPS_MINPULSEWIDTH
#define XI_STEP4_MINPULSEWIDTH XI_BKA30_STEPS_MINPULSEWIDTH

#endif

#ifdef XI_DUAL_STEPPER_35
// specs voor quad stepper board
#define XI_STEP1_STEPS_CIRCLE XI_BKA30_STEPS_CIRCLE
#define XI_STEP2_STEPS_CIRCLE XI_BKA30_STEPS_CIRCLE

#define XI_STEP1_MAX_SPEED XI_BKA30_STEPS_MAXSPEED
#define XI_STEP2_MAX_SPEED XI_BKA30_STEPS_MAXSPEED

<<<<<<< HEAD
#define XI_STEP1_ACCEL XI_BKA30_STEPS_ACCELERATION
#define XI_STEP2_ACCEL XI_BKA30_STEPS_ACCELERATION

#define XI_STEP1_MINPULSEWIDTH XI_BKA30_STEPS_MINPULSEWIDTH
#define XI_STEP2_MINPULSEWIDTH XI_BKA30_STEPS_MINPULSEWIDTH
=======

#define XI_STEP1_ACCEL XI_BKA30_STEPS_ACCELERATION
#define XI_STEP2_ACCEL XI_BKA30_STEPS_ACCELERATION
>>>>>>> 9ae1e597c7d566f0290c51dd73e4281a1b8a47df

#endif

#ifdef XI_DUAL_STEPPER_25
// specs voor 2'5" dual stepper board
#define XI_STEP1_STEPS_CIRCLE XI_BKA30_STEPS_CIRCLE
#define XI_STEP2_STEPS_CIRCLE XI_BKA30_STEPS_CIRCLE

#define XI_STEP1_MAX_SPEED XI_BKA30_STEPS_MAXSPEED
#define XI_STEP2_MAX_SPEED XI_BKA30_STEPS_MAXSPEED

#define XI_STEP1_ACCEL XI_BKA30_STEPS_ACCELERATION
#define XI_STEP2_ACCEL XI_BKA30_STEPS_ACCELERATION

<<<<<<< HEAD
#define XI_STEP1_MINPULSEWIDTH XI_BKA30_STEPS_MINPULSEWIDTH
#define XI_STEP2_MINPULSEWIDTH XI_BKA30_STEPS_MINPULSEWIDTH
=======
#define XI_STEP1_ACCEL XI_BKA30_STEPS_ACCELERATION
#define XI_STEP2_ACCEL XI_BKA30_STEPS_ACCELERATION

#define XI_STEP1_ACCEL XI_BKA30_STEPS_ACCELERATION
#define XI_STEP2_ACCEL XI_BKA30_STEPS_ACCELERATION

>>>>>>> 9ae1e597c7d566f0290c51dd73e4281a1b8a47df
#endif

//===================================================================================================================
// INSTRUMENT SPECIFIC SETTINGS - FUNCTIONAL CONFIGURATION
//===================================================================================================================

#ifdef XI_INSTRUMENT_FUEL_GAUGE

#define XI_INSTRUMENT_CODE "FEULG"

// max data elements to share with XPlane
#define XI_INSTRUMENT_MAX_ELEMENTS 2

// define # of steppers used in this instrument
#define XI_STEPPERS_USED	2

// define type of stepper range and info item they display
//#define XI_STEP1_360
#define XI_STEP1_PIE
#define XI_STEP1_MIN_PIE 0
#define XI_STEP1_MAX_PIE 120
#define XI_STEP1_MIN_RANGE 0
#define XI_STEP1_MAX_RANGE 32
#define XI_STEP1_MAX_BACKSTOP -10
#define XI_STEP1_ITEM XP_FUEL_LEFT

//#define XI_STEP2_360
#define XI_STEP2_PIE
#define XI_STEP2_MIN_PIE 0
#define XI_STEP2_MAX_PIE 120
#define XI_STEP2_MIN_RANGE 0
#define XI_STEP2_MAX_RANGE 32
#define XI_STEP2_MAX_BACKSTOP -10
#define XI_STEP2_ITEM XP_FUEL_RIGHT

#endif

#ifdef  XI_INSTRUMENT_OIL_GAUGE

#endif

#ifdef  XI_INSTRUMENT_EXHFLOW_GAUGE

#endif

#ifdef  XI_INSTRUMENT_VACAM_GAUGE

#endif


// ------------------------------------------
// codes not used in instruments yet :-)
// ------------------------------------------

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

<<<<<<< HEAD
//===================================================================================================================
// data emlement definitions for communication
//===================================================================================================================
#define XP_WET_COMPASS		="CMPWET"
#define XP_GYRO_DIRECTION	="GYRDIR"
#define XP_GYRO_HEADING		="GIRHEA"
#define XP_ALTITUTE			="ALTITU"
#define XP_ALT_BARO			="ALTBAR"
=======
>>>>>>> 9ae1e597c7d566f0290c51dd73e4281a1b8a47df

//===================================================================================================================
// VALIDATION SECTION
//===================================================================================================================
#if defined(XI_CONTROLER_35) && defined(XI_CONTROLER_25)
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
