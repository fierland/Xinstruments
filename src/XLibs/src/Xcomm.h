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

#ifndef Xcmm_h_
#define Xcomm_h_

#include <stdlib.h>
#include <QList.h>
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
	
	int addElement(char* type, void(*callBack)(float), uint32_t rangeMax, uint32_t rangeMin = 0, boolean readOnly =true);
	int setValue(char* type, uint32_t value);
	void checkQueue();

protected:
/*
*/

	struct _DataLinks {
		int linkId;
		char* elementId;
		uint32_t rangeMax;
		uint32_t rangeMin;
		uint32_t lastVal;
		boolean readOnly = true;
		bool subscribed = false
		void(*setdata)(float) = NULL;	// function to set updated data in instrument
		unsigned long timestamp = 0;	// last read time
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
	
	

	
	//Just look up the datarefs at http://www.xsquawkbox.net/.
	//Easy!
	//	DREF0+(4byte byte value)+dref_path+0+spaces to complete the whole message to 509 bytes

	struct _dref_struct
	{
		xflt var;
		xchr dref_path[500];
	};

	//SEND A -999 FOR ANY VARIABLE IN THE SELECTION LIST THAT YOU JUST WANT TO LEAVE ALONE, OR RETURN TO DEFAULT CONTROL IN THE SIMULATOR RATHER THAN UDP OVER-RIDE.

	struct _data_struct
	{
		uint32_t index;		 // data index, the index into the list of variables you can output from the Data Output screen in X-Plane.
		float data[8]; 	// the up to 8 numbers you see in the data output screen associated with that selection.. many outputs do not use all 8, though.
	};

	struct _Xp_becn_struct
	{
		char 		header[5];
		uint8_t 	beacon_major_version;		// 1 at the time of X-Plane 10.40
		uint8_t 	beacon_minor_version;		// 1 at the time of X-Plane 10.40
		xint 		application_host_id;		// 1 for X-Plane, 2 for PlaneMaker
		xint 		version_number;			// 104103 for X-Plane 10.41r3
		uint32_t	role;						// 1 for master, 2 for extern visual, 3 for IOS
		uint16_t 	port;					// port number X-Plane is listening on, 49000 by default
		xchr		computer_name[500];		// the hostname of the computer, e.g. “Joe’s Macbook”
	};
	
	_Xp_becn_struct _becn_struct;

	int _findInList(char* toFind);

};
#endif

