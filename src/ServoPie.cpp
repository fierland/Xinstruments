// setting up the UPD stuff
#include <ServoPie.h>
#include <mydebug.h>


//
//	constructor
//
ServoPie::ServoPie( float rangeMax, float rangeMin, uint8_t pieMin, uint8_t pieMax, uint8_t pin1 ) {
	
	DPRINTLN("Start ServoPie constructor");
	
	// set internal params
	_rangeMin 		= rangeMin;
	_rangeMax 		= rangeMax;
	_totalRange		= rangeMax - rangeMin;
	_pieMin		 	= (pieMin<0) ? 0: pieMin;;
	_pieMax			= (pieMax>180) ? 180 : pieMax;;	
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

	DPRINTLN("End ServoPie constructor");	
}

//
// reset dail to zero position
//
void ServoPie::moveTo0(void){

	DPRINTLN("Start ServoPie moveTo0");	
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
	DPRINTLN("End ServoPie moveTo0");	
}

//
// deconstructor
//
ServoPie::~ServoPie(){
	powerOff();
	delete _myServo;
}

//
// set the dail to new value_comp
//
int ServoPie::moveTo(float newPos){	
	DPRINTLN("Start ServoPie moveTo");
  	_myServo->write(map(newPos,_rangeMin,_rangeMax,_pieMin,_pieMax));
	DPRINTLN("End ServoPie moveTo");
};

//
// power on actions;
//
void ServoPie::powerOn(){
	
	DPRINTLN("Start ServoPie powerOn");
	_powerOn=true;
	DPRINTLN("End ServoPie powerOn");
}

//
// power off actions;
//
void ServoPie::powerOff(){
	
	DPRINTLN("Start ServoPie powerOff");  
	_currentPos =0;
	_powerOn = false;
	moveTo0();
	DPRINTLN("End ServoPie powerOff");

}

