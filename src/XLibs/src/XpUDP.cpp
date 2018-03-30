// setting up the UPD stuff
#include <XpUDP.h>
#include <debug.h>
#include <XPUtils.h>

//
//	constructor
//
XpUDP::XpUDP(){
	_newDataRef = 1;
	//_listRefs = new QList<XpUDP::_DataRefs>() ;
}

//
//	start the session with the X-Plane server
//
int XpUDP::start(IPAddress ipOwn) {
	IPAddress   XpMulticastAdress(239, 255,1, 1);
	uint16_t  	XpMulticastPort = 49707;
	int 	result;
	

	DPRINTLN("Starting connection");
	DPRINTLN(ipOwn);
#ifdef ARDUINO_ARCH_ESP32
	if (_Udp.beginMulticast(XpMulticastAdress, XpMulticastPort)){
#else
	if (_Udp.beginMulticast(ipOwn, XpMulticastAdress, XpMulticastPort)){
#endif		
		DPRINTLN("Udp start ok");	
		do {
			result = GetBeacon();
#if defined(ARDUINO_ARCH_ESP8266)
			ESP.wdtFeed();
#endif
			delay(10);
			DPRINT("Result:");DPRINTLN(	result);
		} while ( result !=0);
	}
	else{
		DPRINTLN("Udp start failed");	
	}
}

// 
//	get beacon from X Plane server and store adres and port
//
int XpUDP::GetBeacon(){
int message_size;
int noBytes;

	noBytes=0;
	message_size=0;

	_LastError = XpUDP::SUCCESS;

	noBytes= _Udp.parsePacket(); //// returns the size of the packet
	if ( noBytes>0) {

		DPRINT(millis() / 1000);
		DPRINT(":Packet of ");
		DPRINT(noBytes);
		DPRINT(" received from ");
		DPRINT(_Udp.remoteIP());
		DPRINT(":");
		DPRINTLN(_Udp.remotePort());
		_XPlaneIP = _Udp.remoteIP();

		if (noBytes<_UDP_MAX_PACKET_SIZE) {
			
			message_size = _Udp.read(_packetBuffer,noBytes); // read the packet into the buffer  
			
			if (message_size <sizeof( _becn_struct )) {
				// copy to breacon struct and compensate byte alignment at 8th byte
				
				memcpy( &_becn_struct, _packetBuffer, 8 );
				memcpy( &((char*)&_becn_struct)[8], (_packetBuffer)+7, sizeof( _becn_struct)-8 );
				 ;
				
				if (strcmp(_becn_struct.header,XPMSG_BECN)==0) { 

					DPRINTLN( "UDP beacon received");
					DPRINT( "beacon_major_version:");
					DPRINTLN( _becn_struct.beacon_major_version);
					DPRINT( "beacon_minor_version:");
					DPRINTLN( _becn_struct.beacon_minor_version);
					DPRINT( "application_host_id:");
					DPRINTLN( _becn_struct.application_host_id);
					DPRINT( "version_number:");
					DPRINTLN( _becn_struct.version_number);       
					DPRINT( "application_host_id:");
					DPRINTLN( _becn_struct.application_host_id);
					DPRINT( "version_number:");
					DPRINTLN( _becn_struct.version_number);
					DPRINT( "role:");
					DPRINTLN( _becn_struct.role);       
					DPRINT( "port:");
					DPRINTLN( _becn_struct.port);
					DPRINT( "computer_name:");
					DPRINTLN( _becn_struct.computer_name); 
				
					if 	(_becn_struct.beacon_major_version!=1 || _becn_struct.beacon_minor_version!=1){

						DPRINTLN("Beacon: Version wrong");	
						DPRINT("Version found: ");
						DPRINT(_becn_struct.beacon_major_version);
						DPRINT(".");
						DPRINTLN(_becn_struct.beacon_minor_version);				

						_LastError = XpUDP::WRONG_HEADER;			
					}	
					// store listen port
					_XpListenPort = _becn_struct.port;		
				}
				else {

					DPRINTLN("Beacon: Header wrong");	
					DPRINTLN(_becn_struct.header);							

					_LastError = XpUDP::WRONG_HEADER;
				}
				
			}
			else {

				DPRINTLN("Beacon: Message size to big for struct");			

				_LastError = XpUDP::BUFFER_OVERFLOW;	
			}	  
		  
		}
		else {

				DPRINTLN("Beacon: Message size to big for buffer");			

				_LastError = XpUDP::BUFFER_OVERFLOW;	
		}
	}
	else {

	DPRINTLN("Beacon: Nothing to read");			

	  _LastError = XpUDP::BEACON_NOT_FOUND;	  
	}
	// everything ok :-)

	return _LastError;	
	
}

//
//	data reader for in loop() pols the udp port for new messages and dispaces them to controls
//
int XpUDP::dataReader()
{
	int message_size =0;
	int noBytes =0 ;
	int curRecord = -1;
	_DataRefs* tmpRef;
	_DataRefs* curRef = NULL;
	
	// noBytes=0;
	// message_size=0;

	_LastError = XpUDP::SUCCESS;

	noBytes= _Udp.parsePacket(); //// returns the size of the packet
	
	if ( noBytes>0) {
  
		DPRINT(millis() / 1000);
		DPRINT(":Packet of ");
		DPRINT(noBytes);
		DPRINT(" received from ");
		DPRINT(_Udp.remoteIP());
		DPRINT(":");
		DPRINTLN(_Udp.remotePort());
		
		message_size = _Udp.read(_packetBuffer,noBytes); // read the packet into the buffer  
		DPRINT("Header:");
		DPRINTLN((char *)_packetBuffer);
		DPRINTBUFFER(_packetBuffer,noBytes);
	
		// check if header is RREF
		if (noBytes >= 4 && _packetBuffer[0] == 'R' && _packetBuffer[1] == 'R' && _packetBuffer[2] == 'E' && _packetBuffer[3] == 'F') {
			// read the values into array		
			for (int idx = 5; idx < noBytes; idx += 8) {
#if defined(ARDUINO_ARCH_ESP8266)
				ESP.wdtFeed();
#endif
				// get value
				float value = xflt2float(&(_packetBuffer[idx + 4]));
				// get en
				uint32_t en = xint2uint32(&(_packetBuffer[idx]));
				DPRINT("Index value:");
				DPRINTLN(en);				
				DPRINT("param value:");
				DPRINTLN(value);				
				
				// check if we know the dataref number 
				for(int i=0; i < _listRefs.size();i++){
					tmpRef = _listRefs[i];
					if(tmpRef->refID == en) {
						curRef = _listRefs[i];
						break;
					}
				}
				
				// process data
				if (curRef != NULL) {
					DPRINTLN("Code found in subscription list");
					curRef->timestamp = millis();
					if (curRef->value != value) {
						DPRINT("Updating value from:");
						DPRINT(curRef->value);
						DPRINT(":to:");
						DPRINTLN(value);
						// TODO: Call to update function
						if (curRef->setdata){
							curRef->setdata(value);
						}
						curRef->value = value;
					}
				}
				else {
					// TODO: Code for signing out of a RREF
					DPRINTLN("Code not found in subscription list");
				}
				
			}
			
		}
		// test for other headers here


	}
	else {
		_LastError = XpUDP::NO_DATA;
	}
	
	// now loop to check if we are receiving all data
	//
	unsigned long timeNow = millis();
	
	for(int i=0; i < _listRefs.size();i++){
		tmpRef = _listRefs[i];
		if((timeNow - tmpRef->timestamp) > _MAX_INTERVAL ) {
			DPRINT("Item not receiving data:");
			DPRINTLN(i);
			sendRREF(_listRefs[i]);
			break;
		}
	}		

	return _LastError;
}

//
// create new data referece link with X-Plane
//
// int XpUDP::registerDataRef(int iFreq, const char *sCode ){
// }

int XpUDP::registerDataRef(int iFreq, const char *sCode, void (*callBack)(float) ){
	bool notFound = true;	
	int curRecord= -1;
	_DataRefs* newRef;
	_DataRefs* tmpRef;
	
	
	DPRINT("RegisterDataRef:START:");
	
	// first test if we already have the item. 
	for (int i=0; i< _listRefs.size();i++){
			tmpRef = _listRefs[i];
			if (strcmp(tmpRef->command,sCode) ==0) {
				notFound = false;
				curRecord = i;
				break;
			}
	}
	
	if (curRecord == -1) {
		//not found so create new item
		DPRINT("New record");
		newRef = new _DataRefs;
		_listRefs.push_back(newRef);
		newRef->refID = _listRefs.size();
		strcpy(newRef->command,sCode);
		newRef->setdata = callBack;
		
		
		
	}
	else {
		// found a record so update item 
		DPRINT("existing reccord");
		newRef = _listRefs[curRecord];
		
	}
		
	// check if active and for last timestamp
	if (not newRef->subscribed || (millis() - newRef->timestamp) > _MAX_INTERVAL){
		// create new request
		DPRINT("create x-plane request");
		newRef->subscribed = true;
		sendRREF(newRef);
		// read first returned value
		dataReader();
	}
	
	return XpUDP::SUCCESS;
}
  
// 
// send a data request to x-plane
// 
int XpUDP::sendRREF(_DataRefs* newRef){
	
	// fill Xplane struct array
	_dref_struct_in.dref_freq 	=	newRef->frequency ;
	_dref_struct_in.dref_en		= 	newRef->refID	;	
	strcpy(_dref_struct_in.dref_string,newRef->command);
	
	DPRINT("Sendinf New Ref:");
	DPRINT(newRef->refID);
	DPRINT(":");
	DPRINTLN(newRef->command);

	//_arrDataRefs[_newDataRef].setdata = 
#if defined(ARDUINO_ARCH_ESP8266)
		ESP.wdtFeed();
#endif
	sendUDPdata(XPMSG_RREF,(byte *)&_dref_struct_in,sizeof(_dref_struct_in),_Xp_dref_in_msg_size);	
	
}

//
//	Send a packet to X-Plane
//
void XpUDP::sendUDPdata(const char *header,const byte *dataArr,const int arrSize,const int sendSize){
	
	
	DPRINTLN("sending XPlane packet...");
	
	// set all bytes in the buffer to 0	
	memset(_packetBuffer, 0, _UDP_MAX_PACKET_SIZE);
	 // Initialize values needed to form request to x-plane
	 
	memcpy(_packetBuffer, header, 4 );
	memcpy((_packetBuffer)+5 , dataArr ,arrSize  );
	
	DPRINT("Header:");
	DPRINTLN((char *)_packetBuffer);
	DPRINTBUFFER(_packetBuffer,sendSize);
	//printBuffer(_packetBuffer);
	DPRINT("Sending to:");
	DPRINT(_XPlaneIP);
	DPRINT(": Port:");
	DPRINTLN(_XpListenPort);
	DPRINT("Sending size:");
	DPRINTLN( sendSize);
	
	_Udp.beginPacket(_XPlaneIP, _XpListenPort); 	
	_Udp.write(_packetBuffer, sendSize );
	_Udp.endPacket();
	
	
}
