// Xinstruments.h
//
// author: Frank van Ierland
// version: 0.1
//
// base class to control simulated flight instruemnts
//
//------------------------------------------------------------------------------------
/// \author  Frank van Ierland (frank@van-ierland.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE LISTS
// Copyright (C) 2018 Frank van Ierland

#ifndef Xinstruments_h_
#define Xinstruments_h_
/*
#define _DEBUG_		// toggle all print statements for debugging
#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.

#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
  #define DPRINTBUFFER(x,y) DPRINT("Buffer:");for(int i=0;i<y;i++){DPRINT(x[i]);DPRINT(":");}	DPRINTLN("END");
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
  #define DPRINTBUFFER(x,y)
#endif
*/
#include <stdlib.h>
#include <QList.h>
#include <debug.h>

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <limits.h>

#define _Xinstruments_rotary 	1
#define _Xinstruments_range 	2

			
		
class Xinstruments
{
public:
	

	//constructor
	Xinstruments(uint8_t type, uint32_t rangeMax, uint32_t rangeMin =0);

	// start connection
	int (IPAddress ipOwn) ;
	int dataReader();				// poll for new data need to be in loop()
	
	//int registerDataRef(int iFreq, const char *sCode );
	int registerDataRef(int iFreq, const char *sCode, void (*callbackFunc)(float par) = NULL );
	

protected:
/*
	IPAddress   XpMulticastAdress(239, 255,1, 1);
	uint16_t  	XpMulticastPort = 49707;
*/
	struct _DataRefs 
	{
		uint8_t	refID					;	//Internal number of dataref
		uint8_t frequency = 5			;
		bool subscribed	= false 		;
		char command[_Xp_rref_size]		;	//short internal name
		float value						;	// last received value
		void (*setdata)(float)	= NULL	;	// function to set updated data in instrument
		unsigned long timestamp	= 0 	;	// last read time
		
	};

	//
	// a place to hold all referenced items
	//
	QList<_DataRefs *> _listRefs;
	

	int 	GetBeacon();
	void 	sendUDPdata(const char *header,const byte *dataArr,const int arrSize,const int sendSize);
	int 	sendRREF(_DataRefs* newRef);
	
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
	static const int 	_UDP_MAX_PACKET_SIZE = 550; // max size of message
	static const int	_MAX_INTERVAL = 500; 		// Max interval in ms between data messages
	
	byte 		_packetBuffer[_UDP_MAX_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
	WiFiUDP 	_Udp;
	int			_newDataRef;
	
	// typedefs for the xplane specific types
	typedef uint32_t xint;
	typedef char xchr;
	typedef float xflt;
	typedef double xdob;

	// 
	// Internal variables 
	//
	int _LastError = Xinstruments::SUCCESS;
	uint16_t _XpListenPort = 49000;
	IPAddress _XPlaneIP ;
	

	
	//Send in a “dref_freq” of 0 to stop having X-Pane send the dataref values.
	struct _Xp_dref_struct_in
	{
		xint dref_freq			;	//IS ACTUALLY THE NUMBER OF TIMES PER SECOND YOU WANT X-PLANE TO SEND THIS DATA!
		xint dref_en			;	// reference code to pass when returning d-ref
		xchr dref_string[400]	;	// dataref string with optional index [xx]
	} ;
	
	_Xp_dref_struct_in _dref_struct_in;


	struct _dref_struct_out
	{
		xint dref_en	;	//Where dref_en is the integer code you sent in for this dataref in the struct above.
		xflt dref_flt	;	//Where dref_flt is the dataref value, in machine-native floating-point value, even for ints!
	};

	//Use this to set ANY data-ref by UDP! With this power, you can send in any floating-point value to any data-ref in the entire sim!
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

};
#endif

