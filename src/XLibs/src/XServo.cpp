// setting up the UPD stuff
#include "XServo.h"


//
//	constructor
//
XServo::XServo(float rangeMin, float rangeMax, uint8_t pieMin, uint8_t pieMax, uint8_t pin1 ) {
	
	DPRINTLN("Start XServo constructor");
	
	// set internal params
	_rangeMin 		= rangeMin;
	_rangeMax 		= rangeMax;
	_totalRange		= rangeMax - rangeMin;
	_pieMin		 	= (pieMin<0) ? 0: pieMin;;
	_pieMax			= (pieMax>180) ? 180 : pieMax;;	
	_pinPWM			= pin1;
	//TODO: range checking


	_stepsPerItem 	= _totalRange/(pieMax-pieMin);
	
	DPRINT("New Servo Wedge dail (");
	DPRINT(_rangeMin);
	DPRINT("->");
	DPRINT(_rangeMax);
	DPRINT(") stepsize:");
	DPRINTLN(_stepsPerItem);	
	
	// set some defaults
	_currentPos 	= 0 ;	
	_newPos 		= 0 ;
	_moveSize 		= 0 ;
	
	_myServo = new Servo();
	_myServo->attach(pin1);

	moveTo0();

	DPRINTLN("End XServo constructor");	
}

//
// reset dail to zero position
//
void XServo::moveTo0(void){

	DPRINTLN("Start XServo moveTo0");	
// set to scale zero
	if (_rangeMin>0 && _rangeMax>0) {
		_myServo->write(_pieMin);
	}

	else if (_rangeMin<0 && _rangeMax<0) {
		_myServo->write(-_pieMax);
	}
	else {
		_myServo->write(map(0,_rangeMin,_rangeMax,_pieMin,_pieMax));
	}
	DPRINTLN("End XServo moveTo0");	
}


//
// move to main position
//
void XServo::moveMin(void) {
	moveTo(_pieMin);

}

//
// move to max position
//
void XServo::moveMax(void) {
	moveTo(_pieMax);
}


//
// deconstructor
//
XServo::~XServo(){
	powerOff();
	delete _myServo;
}

//
// set the dail to new value_comp
//
void XServo::moveTo(float newPos){	
	DPRINTLN("Start XServo moveTo");
  	_myServo->write(map(newPos,_rangeMin,_rangeMax,_pieMin,_pieMax));
	DPRINTLN("End XServo moveTo");
};

//
// power on actions;
//
void XServo::powerOn(){
	
	DPRINTLN("Start XServo powerOn");
	_powerOn=true;
	_myServo->attach(_pinPWM);
	DPRINTLN("End XServo powerOn");
}

//
// power off actions;
//
void XServo::powerOff(){
	
	DPRINTLN("Start XServo powerOff");  
	_powerOn = false;
	_myServo->detach();
	DPRINTLN("End XServo powerOff");

}

