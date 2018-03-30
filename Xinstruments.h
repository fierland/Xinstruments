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
#include "src\XLibs\src\XpUDP.h"
#include "src\XLibs\src\Xcomm.h"


//===================================================================================================================
// INSTRUMENT SPECIFIC SETTINGS - HARDWARE
//===================================================================================================================

//-------------------------------------------------------------------------------------------------------------------
// define all hardware interfaces for the pins
//	Curently i have designed 2 controler boards one for 3,5" instruments and one for 2,5" instruments
//-------------------------------------------------------------------------------------------------------------------

// controler type used
#define XI_CONTROLER_35
//#define XI_CONTROLER_25

//-------------------------------------------------------------------------------------------------------------------
// define which stepper board is used
//-------------------------------------------------------------------------------------------------------------------

//#define XI_QUAD_STEPPER_35
#define XI_DUAL_STEPPER_35
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
#define XI_MAX_HAL		2
#define XI_MAX_RE		0

// set the pin mappings for the stepper motors
#define XI_STEP1_DIR 1
#define XI_STEP1_STP 2
#define XI_STEP2_DIR 3
#define XI_STEP2_STP 4
#define XI_STEP3_DIR 0
#define XI_STEP3_STP 0
#define XI_STEP4_DIR 0
#define XI_STEP4_STP 0

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

#define XI_PWR_MON_PIN 0

#endif
//-------------------------------------------------------------------------------------------------------------------
//Stepper board settings
//-------------------------------------------------------------------------------------------------------------------

// generic hardware settings

// for VID29 and BKA30 steppers
// theorecticlay 360*12
#define XI_BKA30_STEPS_CIRCLE 900
#define XI_BKA30_STEPS_MAXSPEED 5000
#define XI_BKA30_STEPS_ACCELERATION 300
#define XI_BKA30_STEPS_MINPULSEWIDTH 1

// for 28BYJ steppers
#define XI_28BYJ_STEPS_CIRCLE 2400
#define XI_28BYJ_STEPS_MAXSPEED 5000
#define XI_28BYJ_STEPS_ACCELERATION 300
#define XI_28BYJ_STEPS_MINPULSEWIDTH 1




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

#define XI_STEP1_ACCEL XI_28BYJ_STEPS_ACCELERATION
#define XI_STEP2_ACCEL XI_28BYJ_STEPS_ACCELERATION
#define XI_STEP3_ACCEL XI_28BYJ_STEPS_ACCELERATION
#define XI_STEP4_ACCEL XI_28BYJ_STEPS_ACCELERATION

#endif

#ifdef XI_DUAL_STEPPER_35
// specs voor quad stepper board
#define XI_STEP1_STEPS_CIRCLE XI_BKA30_STEPS_CIRCLE
#define XI_STEP2_STEPS_CIRCLE XI_BKA30_STEPS_CIRCLE


#define XI_STEP1_MAX_SPEED XI_BKA30_STEPS_MAXSPEED
#define XI_STEP2_MAX_SPEED XI_BKA30_STEPS_MAXSPEED
D

#define XI_STEP1_ACCEL XI_28BYJ_STEPS_ACCELERATION
#define XI_STEP2_ACCEL XI_28BYJ_STEPS_ACCELERATION

#endif

#ifdef XI_DUAL_STEPPER_25
// specs voor quad stepper board
#define XI_STEP1_STEPS_CIRCLE XI_BKA30_STEPS_CIRCLE
#define XI_STEP2_STEPS_CIRCLE XI_BKA30_STEPS_CIRCLE


#define XI_STEP1_MAX_SPEED XI_BKA30_STEPS_MAXSPEED
#define XI_STEP2_MAX_SPEED XI_BKA30_STEPS_MAXSPEED


#define XI_STEP1_ACCEL XI_28BYJ_STEPS_ACCELERATION
#define XI_STEP2_ACCEL XI_28BYJ_STEPS_ACCELERATION
#endif

//===================================================================================================================
// INSTRUMENT SPECIFIC SETTINGS - FUNCTIONAL CONFIGURATION
//===================================================================================================================

#define XI_INSTRUMENT_CODE "GYROCMPS"
// max data elements to share with XPlane
#define XI_INSTRUMENT_MAX_ELEMENTS 2

// set these flags if the instrument has a Power and ready indication flag controled by a servo  
#define USE_PWR_FLAG_SERVO

#define PWR_FLAG_SERVO_PIN		XI_SERVO1_PIN
#define PWR_FLAG_SERVO_MAXPOS	50

// define # of steppers used in this instrument
#define XI_STEPPERS_USED	2

// define type of stepper range and info item they display
#define XI_STEP1_360
//#define XI_STEP1_PIE
//#define XI_STEP1_MIN_PIE 0
//#define XI_STEP1_MAX_PIE 180
#define XI_STEP1_MIN_RANGE 0
#define XI_STEP1_MAX_RANGE 360
#define XI_STEP1_ITEM "GYRO_COMPAS"

#define XI_STEP2_360
//#define XI_STEP2_PIE
//#define XI_STEP2_MIN_PIE 0
//#define XI_STEP2_MAX_PIE 180
#define XI_STEP2_MIN_RANGE 0
#define XI_STEP2_MAX_RANGE 360
#define XI_STEP2_ITEM "GYRO_HEADING"

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

define XI_RE1_ITEM "GYRO_HEADING"

//define XI_RE2_ITEM "GYRO_HEADING"




//===================================================================================================================
// data emlement definitions for communication
//===================================================================================================================
#define XP_WET_COMPASS		="CMPWET"
#define XP_GYRO_DIRECTION	="GYRDIR"
#define XP_GYRO_HEADING		="GIRHEA"
#define XP_ALTITUTE			="ALTITU"
#define XP_ALT_BARO			="ALTBAR"

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
