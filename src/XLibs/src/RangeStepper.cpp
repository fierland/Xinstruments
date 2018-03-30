// setting up the UPD stuff
#include "RangeStepper.h"
#include "XPUtils.h""

//
//	constructor
//

void _initStepper(uint8_t stepperType) {
	// set some defaults
	_CurrentPos = 0;
	_NewPos = 0;
	_moveSize = 0;
	_isOn = false;


	case (stepperType) {
		switch RangeStepper::typeVID27 :
		// set the speed at 60 rpm:
		// myStepper.setSpeed(500);
		_rotationSteps = RangeStepper::typeVID27;
		_stepper.setMaxSpeed(5000);
		_stepper.setAcceleration(300.0);
		_stepper.setSpeed(1000);
		_stepper.setMinPulseWidth(1);
		break;
		switch RangeStepper::type28BYJ :
		_rotationSteps = RangeStepper::type28BYJ;
		_stepper.setMaxSpeed(5000);
		_stepper.setAcceleration(300.0);
		_stepper.setSpeed(3000);
		_stepper.setMinPulseWidth(4);
		break;
	}
	// TODO include code to calibrate dail seek 0

}



//
//	constructor
//
void _constructAll(int8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5, uint8_t stepperType) {
	uint8_t tmpType;

	DPRINTLN("Start RangeStepper construct-all");
	// create the stepper itself
	if (pin3 == 0 && pin4 == 0)
	{
		tmpType = AccelStepper::DRIVER;
	}
	else {
		switch (stepperType) {

			case RangeStepper::typeVID27 :
			tmpType = AccelStepper::FULL4WIRE;
			break;
			case RangeStepper::type28BYJ :
			tmpType = AccelStepper::HALF4WIRE;
			break;
		}
		
	};

	_stepper = new AccelStepper(tmpType, pin1, pin2, pin3, pin4, true);
	// create the stepper itself
	
	// set some defaults
	_CurrentPos = 0;
	_NewPos = 0;
	_moveSize = 0;
	_isOn = false;


	case (stepperType) {
		switch RangeStepper::typeVID27 :
		// set the speed at 60 rpm:
		// myStepper.setSpeed(500);
		_rotationSteps = RangeStepper::typeVID27;
		_stepper.setMaxSpeed(5000);
		_stepper.setAcceleration(300.0);
		_stepper.setSpeed(1000);
		_stepper.setMinPulseWidth(1);
		break;
		switch RangeStepper::type28BYJ :
		_rotationSteps = RangeStepper::type28BYJ;
		_stepper.setMaxSpeed(5000);
		_stepper.setAcceleration(300.0);
		_stepper.setSpeed(3000);
		_stepper.setMinPulseWidth(4);
		break;
	}
	// TODO include code to calibrate dail seek 0
	DPRINTLN("End RangeStepper construct-all");

};


//
// constructor for segmented dails
//
RangeStepper::RangeStepper (uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5,float RangeMax, float RangeMin = 0, float wedgeSize, uint8_t stepperType = RangeStepper::typeVID27){

	DPRINTLN("Start RangeStepper wedge constructor");
		
	// set internal params
	_rangeMin 			= RangeMin;
	_rangeMax 			= RangeMax;
	_totalRange			= RangeMax - RangeMin;
	_isContinious 		= false;	
	_stepsPerRotation 	= stepperType;	
	
	// define step size for later calculations
	/// first define part of circle used
	
	_stepsPerItem = (stepperType*360/wedgeSize)/_totalRange;

	DPRINT("New wedge dail (");
	DPRINT(_rangeMin);
	DPRINT("->");
	DPRINT(_rangeMax);
	DPRINT(") stepsize:");
	DPRINTLN(_stepsPerItem);	
	
	_constructAll(pin1, pin2, pin3, pin4, stepperType );
	
	
	DPRINTLN("End RangeStepper wedge constructor");	
}

int RangeStepper::calibrate()
{
	return 0;
}

RangeStepper::RangeStepper(uint8_t pin1 = 2, uint8_t pin2 = 3, float RangeMax, float RangeMin = 0, float wedgeSize, uint8_t stepperType = RangeStepper::typeVID27) {

	DPRINTLN("Start RangeStepper wedge constructor");

	// set internal params
	_rangeMin = RangeMin;
	_rangeMax = RangeMax;
	_totalRange = RangeMax - RangeMin;
	_isContinious = false;
	_stepsPerRotation = stepperType;

	// define step size for later calculations
	/// first define part of circle used

	_stepsPerItem = (stepperType * 360 / wedgeSize) / _totalRange;

	DPRINT("New wedge dail (");
	DPRINT(_rangeMin);
	DPRINT("->");
	DPRINT(_rangeMax);
	DPRINT(") stepsize:");
	DPRINTLN(_stepsPerItem);

	_constructAll(pin1, pin2, 0,0, stepperType);

	DPRINTLN("End RangeStepper wedge constructor");
}

//
// constructor for continious dails 
//
RangeStepper::RangeStepper (uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5,float RangeMax, float RangeMin = 0, uint8_t stepperType = RangeStepper::typeVID27){
	
	DPRINTLN("Start RangeStepper continiuos constructor");
		
	// set internal params
	
	_rangeMin 		= RangeMin;
	_rangeMax 		= RangeMax;
	_totalRange		= RangeMax - RangeMin;
	_isContinious 	= true;	
	
	// define step size for later calculations
	_stepsPerItem = stepperType/_totalRange;

	DPRINT("New continius dail (");
	DPRINT(_rangeMin);
	DPRINT("->");
	DPRINT(_rangeMax);
	DPRINT(") stepsize:");
	DPRINTLN(_stepsPerItem);	
	
	_constructAll(pin1, pin2, pin3, pin4, stepperType );
	
	DPRINTLN("End RangeStepper continiuos constructor");	
}
//
// constructor for continious dails 
//
RangeStepper::RangeStepper(uint8_t pin1 = 2, uint8_t pin2 = 3, float RangeMax, float RangeMin = 0, uint8_t stepperType = RangeStepper::typeVID27) {

	DPRINTLN("Start RangeStepper continiuos constructor");

	// set internal params

	_rangeMin = RangeMin;
	_rangeMax = RangeMax;
	_totalRange = RangeMax - RangeMin;
	_isContinious = true;

	// define step size for later calculations
	_stepsPerItem = stepperType / _totalRange;

	DPRINT("New continius dail (");
	DPRINT(_rangeMin);
	DPRINT("->");
	DPRINT(_rangeMax);
	DPRINT(") stepsize:");
	DPRINTLN(_stepsPerItem);

	constructAll(pin1, pin2, 0, 0, stepperType);


	DPRINTLN("End RangeStepper continiuos constructor");
}




//
//
//
int RangeStepper::move(){
	DPRINTLN("Start RangeStepper move");

	DPRINTLN("End RangeStepper move");
};


//
// set the dail to new value_comp
//
virtual int RangeStepper::moveTo(uint32_t newPos){	
   int dir = 1;
   int moveSteps = 0;
   long stepsPos;
   long diffPos;

    DPRINTLN("Start RangeStepper moveTO");
	
	// check if in range
	if (newPos > _rangeMax || newPos < _rangeMin)	{
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
	
	if (_isContinious) {
		// now see how we get there the fastes way for continues dails
		// if move more than 50% of range change direction to take shortest way 
		DPRINT("Adapting for Continues dails:")
		if (abs(_moveSize) > (_totalRange/2)) {
			// make move shortest route
			DPRINT("Adjust to shortest route:")
			(_moveSize>)? _moveSize-=(_totalRange/2) : _moveSize += (_totalRange/2);
		};
		// pointer moves from low thru 0 to high
		if (_moveSize < 0) && (_moveSize + _currentPos) <0 ){
			DPRINT("Reset position + full rotation:");
			_myStepper.currentPosition(_currentPos+_stepsPerRotation);
		}	
		// pointer moves via max thru 0
		if (_moveSize > 0) && (_moveSize + _currentPos) > _rangeMax ){
			DPRINT("Reset position - full rotation:");
			_myStepper.currentPosition(_currentPos-_stepsPerRotation);
		}		
		DPRINTLN("DONE")
		
	}

	DPRINT("Move wedge to new pos:");
	DPRINTLN(newPos);
	_myStepper.runToNewPosition(newPos * _stepsPerItem);   
	_currentPos = newPos ;

	DPRINTLN("End RangeStepper moveTo");
};

//
// set dail to 0 position
//
virtual int calibrate(uint8_t pinHal = -1) {
	
	DPRINTLN("Start RangeStepper calibrate");
	// TODO: code to set zero pos using sensor or by moving to backstop
	
	DPRINTLN("End RangeStepper calibrate");

};

//
// poll for new data need to be in loop()
//
virtual void powerState(bool isOn){
	
	DPRINTLN("Start RangeStepper powerState");
	
	_isOn = isOn;
	// aditional code to set / reset power indicator or flag
	// TODO code to move pointer to rest position
	
	
	DPRINTLN("End RangeStepper powerState");
	
} ;

