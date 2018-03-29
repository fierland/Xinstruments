// setting up the UPD stuff
#include <StepperPie.h>
#include <mydebug.h>


//
//	constructor
//
StepperPie::StepperPie( float rangeMax , float rangeMin, uint8_t pieSize , uint16_t stepsCircle , uint8_t interface , uint8_t pin1 , uint8_t pin2 , uint8_t pin3 , uint8_t pin4 , bool enable  ): AccelStepper( interface ,  pin1 ,  pin2 , pin3 ,pin4 , enable) {
	
	DPRINTLN("Start StepperPie constructor");
	
	
	// set internal params
	_rangeMin 			= rangeMin;
	_rangeMax 			= rangeMax;
	_totalRange			= rangeMax - rangeMin;
	_stepsPerRotation 	= stepsCircle;	
	
	// define step size for later calculations
	/// first define part of circle used
	_stepsPerItem = (stepsCircle*360/pieSize)/_totalRange;
	
	DPRINT("New wedge dail (");
	DPRINT(_rangeMin);
	DPRINT("->");
	DPRINT(_rangeMax);
	DPRINT(") stepsize:");
	DPRINTLN(_stepsPerItem);	
	
	// set some defaults
	_currentPos 	= 0 ;	
	_newPos 		= 0 ;
	_moveSize 		= 0 ;
	
	setMaxSpeed(10000);
	setAcceleration(1000.0);
	setSpeed(5000); 
	setMinPulseWidth(1);
	
	DPRINTLN("End StepperPie constructor");	
}

//
// deconstructor
//
StepperPie::~StepperPie(){
	powerOff();
}
//
//
//
/*
	int StepperPie::move(){
	DPRINTLN("Start StepperPie move");
	
	DPRINTLN("End StepperPie move");
	};
*/
void StepperPie::runToNewPosition(float newPos){	
	moveTo(newPos);
	runToPosition();
};

//
// set the dail to new value_comp
//
int StepperPie::moveTo(float newPos){	
	int dir = 1;
	int moveSteps = 0;
	long stepsPos;
	long diffPos;
	
	DPRINTLN("Start StepperPie moveTO");
	
	// check if in range and dail is on
	if (!_powerOn || newPos > _rangeMax || newPos < _rangeMin)	{
		DPRINT("ERR: OutOfRange (");
		DPRINT(_rangeMin);
		DPRINT("->");
		DPRINT(_rangeMax);
		DPRINT("):");
		DPRINTLN(newPos);
		
		return 1;
	}		
	
	_newPos = newPos ;
	_moveSize = newPos - _currentPos;
	
	
	DPRINT("Move wedge to new pos:");
	DPRINTLN(newPos);
	AccelStepper::moveTo(newPos * _stepsPerItem);   
	_currentPos = newPos ;
	
	return 0;
	
	DPRINTLN("End StepperPie moveTo");
};

//
// set dail to 0 position
//
void StepperPie::calibrate() {
	
	DPRINTLN("Start StepperPie calibrate");
	// TODO: code to set zero pos using sensor or by moving to backstop
	
	move(-_rangeMax);
	runToPosition();
	setCurrentPosition(0);
	_currentPos =0;
	
	DPRINTLN("End StepperPie calibrate");
	
};
//
// power on actions;
//
void StepperPie::powerOn(){
	
	DPRINTLN("Start StepperPie powerOn");
	_powerOn=true;
	DPRINTLN("End StepperPie powerOn");
}
//
// power off actions;
//
void StepperPie::powerOff(){
	
	DPRINTLN("Start StepperPie powerOff");
	AccelStepper::moveTo(0);   
	_currentPos =0;
	_powerOn = false;
	
	DPRINTLN("End StepperPie powerOff");
	
}

