// Xcomm.h
//
// author: Frank van Ierland
// version: 0.1
//
// Communication module for comm between interface master and instruments
//
//------------------------------------------------------------------------------------
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef Xcomm_h_
#define Xcomm_h_

#include <stdlib.h>
#include <QList.h>
#include "InstrumentStepper.h"
#include <esp32_can.h>	
#include "CanOpen.h"
#include "mydebug.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <limits.h>

	
		
class Xcomm
{
public:
	

	//constructor
	Xcomm(char* instrumentCode, int maxElements);

		//int registerDataRef(int iFreq, const char *sCode );
	
	int addElement(char* type, InstrumentStepper *stepObject, uint32_t rangeMax, uint32_t rangeMin = 0, boolean readOnly =true);
	int setValue(char* type, float value);
	int processInput(char* type, float value);	
	void checkQueue();

protected:
/*
*/
	CanOpen * _canOpenBus = null;

	struct _DataLinks {
		int linkId;
		char elementId[10];
		uint32_t rangeMax;
		uint32_t rangeMin;
		uint32_t lastVal;
		boolean readOnly = true;
		bool subscribed = false;
		unsigned long timestamp = 0;	// last read time
		InstrumentStepper* myStepper = NULL ;		// link to linked stepper driver
	};

	//struct _DataRefs 
	//{
	//	uint8_t	refID					;	//Internal number of dataref
	//	uint8_t frequency = 5			;
	//	bool subscribed	= false 		;
	//	char command[_Xp_rref_size]		;	//short internal name
	//	float value						;	// last received value
	//	void (*setdata)(float)	= NULL	;	// function to set updated data in instrument
	//	unsigned long timestamp	= 0 	;	// last read time
	//	
	//};

	//
	// a place to hold all referenced items
	//
	QList<_DataLinks *> _listRefs;
	

	
private:
/*
	string[] _XpCommands = {
		"XXXX", // novalid command
		"BECN",	//get beacon and inintate connection
		"CMND",	// run a command 
		"RREF",	//SEND ME ALL THE DATAREFS I WANT: RREF	
		"DREF",	//SET A DATAREF TO A VALUE: DREF
		"DATA",	//SET A DATA OUTPUT TO A VALUE: DATA	
		"ALRT",	//MAKE AN ALERT MESSAGE IN X-PLANE: ALRT
		"FAIL",	//FAIL A SYSTEM: FAIL	
		"RECO"	//RECOVER A SYSTEM: RECO
	} ;
*/
	
#define _MAX_INTERVAL 5000

	
	//Just look up the datarefs at http://www.xsquawkbox.net/.
	//Easy!
	//	DREF0+(4byte byte value)+dref_path+0+spaces to complete the whole message to 509 bytes

	
	int _findInList(char* toFind);
	int _updateValue(char * type, float value);

};
#endif

