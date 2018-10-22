/*
 Name:		XinstrumentsMaster.ino
 Created:	3/30/2018 6:41:46 PM
 Author:	Frank
*/

#include "XinstrumentsMaster.h"
#include <WiFiUdp.h>
#include <WiFiType.h>
#include <WiFiSTA.h>
#include <WiFiServer.h>
#include <WiFiScan.h>
#include <WiFiMulti.h>
#include <WiFiGeneric.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WiFi.h>
#include <ETH.h>
#include "XinstrumentsMaster.h"
#include "src/XLibs/src/mydebug.h"
#include "src/XLibs/src/XpUDP.h"
#include "src/XLibs/src/Can2XPlane.h"
#include "src/CanAs/Src/CANaero.h"

const char* ssid = "OnzeStek";
const char* password = "MijnLief09";

XpUDP myXpInterface;
CANaero* myCANbus;

void xpCallBackFunction(float val, uint8_t canId) {
	CanasMessageData newData;
	int service_code = 0;

	DPRINTINFO("START");

	newData.type = CANAS_DATATYPE_FLOAT;
	newData.container.FLOAT = val;
	newData.length = 4;

	myCANbus->ParamPublish(canId, &newData, service_code);

	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int callbackNewXitem(uint8_t canID)
{
	DPRINTINFO("START");
	// check if item exist
	char* xplaneId = Can2XPlane::fromCan2Xplane(canID);

	if (strlen(xplaneId) == 0)
		return -1;

	myXpInterface.registerDataRef(5, xplaneId, xpCallBackFunction);

	DPRINTINFO("STOP");
	return 0;
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int callbackRemoveXitem(uint8_t canID)
{
	DPRINTINFO("START");
	DPRINTINFO("STOP");
	return 0;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	// setup wifi connection
		//TODO: Store wifi details in perm memory
		//TODO: use wifi  autosetup for first connect
		  // Connect to WiFi network
	DPRINTINFO("START");
	DPRINTLN();
	DPRINT("Connecting to ");
	DPRINTLN(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		DPRINT(".");
	};
	DPRINTLN("<");
	DPRINTLN("WiFi connected");
	// Print the IP address
	IPAddress ipOwn = WiFi.localIP();
	DPRINTLN(WiFi.localIP());

	myXpInterface.start(ipOwn);

	// startup tghe CAN areospace bus
	myCANbus = new CANaero(XI_Instrument_NodeID, XI_Instrument_Service_Chan, XI_Hardware_Revision, XI_Software_Revision);
	assert(myCANbus != NULL);

	// as we are master we will listen to serviceRequestdata;
	myCANbus->ServiceRegister_requestData(callbackNewXitem, callbackRemoveXitem);

	DPRINTINFO("STOP");
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// the loop function runs over and over again until power down or reset
void loop() {
	myXpInterface.dataReader();
	myCANbus->Update();
	delay(10);
};