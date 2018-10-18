#include "VidStepper.h"

VidStepper::VidStepper(uint8_t pinDir, uint8_t pinStep, bool is360, bool isReverse)
{
	_pinStep = pinStep;
	_pinDir = pinDir;
	_is360 = is360;
	_isReverse = isReverse;
	_stepsPerDegree = _stepsPerRotation / 360;

	pinMode(_pinStep, OUTPUT);     // set Stepper step pin mode
	pinMode(_pinDir, OUTPUT);      // set Stepper direction pin mode
}

VidStepper::~VidStepper()
{
}

int VidStepper::setStepsPerRotation(int steps)
{
	int oldValue = _stepsPerRotation;
	_stepsPerRotation = steps;

	return oldValue;
}

bool VidStepper::set360(bool is360)
{
	bool oldValue = _is360;
	_is360 = is360;
	return oldValue;
}

void VidStepper::calibrate()
{
	DPRINTLN("Start VidStepper calibrate");

	_targetPosition = -_stepsPerRotation;
	runToPosition();
	_currentPosition = 0;
	_targetPosition = 0;

	DPRINTLN("End VidStepper calibrate");
}

void VidStepper::calibrate(float backstopPos)
{
	DPRINT("Start VidStepper calibrate with value");
	DPRINTLN(backstopPos);

	calibrate();
	_currentPosition = backstopPos;
	_targetPosition = 0;
	runToPosition();

	DPRINTLN("End VidStepper calibrate with value");
}

//
// set the part of the circle used for a non 360 indicator will default _is360 to false
//
float VidStepper::setRange(float newRange)
{
	float oldValue = _rangeInDegree;
	DPRINTLN("Start VidStepper setRange");

	_rangeInDegree = newRange;
	_is360 = false;
	_rangeMax = _rangeInDegree / 360 * _stepsPerRotation;

	DPRINT("New Range:");
	DPRINT(_rangeInDegree);
	DPRINT(" degree: in steps:");
	DPRINTLN(_rangeMax);

	DPRINTLN("End VidStepper setRange");
	return oldValue;
}

int VidStepper::setRangeValues(float rangeMax, float rangeMin)
{
	float oldValue = _totalValueRange;

	DPRINTLN("Start VidStepper setRangeValues");

	_valueMin = rangeMin;
	_valueMax = rangeMax;

	// TODO: fix for negative rangemin
	if (rangeMin < 0) {
		_totalValueRange = rangeMax + rangeMin;
	}
	else {
		_totalValueRange = rangeMax - rangeMin;
	}

	DPRINT("Min:");
	DPRINT(_valueMin);
	DPRINT(":Max:");
	DPRINT(_valueMax);
	DPRINT(":Total range:");
	DPRINTLN(_totalValueRange);

	DPRINTLN("End VidStepper setRangeValues");
	return oldValue;
}

float VidStepper::moveTo(float newPos)
{
	float oldValue = _targetPosition;

	DPRINT("Start VidStepper moveTo From:");
	DPRINT(_currentPosition);
	DPRINT("-To:");
	DPRINTLN(newPos);

	// check if in range
	if (newPos > _rangeMax || newPos < _rangeMin) {
		DPRINT("ERR: OutOfRange (");
		DPRINT(_rangeMin);
		DPRINT("->");
		DPRINT(_rangeMax);
		DPRINT("):");
		DPRINTLN(newPos);
		return -1;
	}

	_targetPosition = newPos;

	DPRINTLN("End VidStepper moveTo");
	return oldValue;
}

float VidStepper::move(float relativePos)
{
	float oldValue = _targetPosition;
	float newPos = _currentPosition + relativePos;

	DPRINT("Start VidStepper move From:");
	DPRINT(_currentPosition);
	DPRINT("-change in position:");
	DPRINTLN(relativePos);

	// check if in range
	if (newPos > _valueMax || newPos < _valueMin) {
		DPRINT("ERR: OutOfRange (");
		DPRINT(_rangeMin);
		DPRINT("->");
		DPRINT(_rangeMax);
		DPRINT("):");
		DPRINTLN(newPos);
		return -1;
	}

	_targetPosition = newPos;
	DPRINTLN("End VidStepper move");
	return oldValue;
}

int VidStepper::setValue(float newValue)
{
	float tempValue;

	DPRINT("Start VidStepper set Value From:");
	DPRINT(_currentValue);
	DPRINT("-To:");
	DPRINTLN(newValue);

	// check if in range
	if (newValue > _rangeMax || newValue < _rangeMin) {
		DPRINT("ERR: OutOfRange (");
		DPRINT(_rangeMin);
		DPRINT("->");
		DPRINT(_rangeMax);
		DPRINT("):");
		DPRINTLN(newValue);
		return -1;
	}

	_currentValue = newValue;

	tempValue = newValue - _valueMin;

	DPRINT("step1:");
	DPRINT(newValue);
	DPRINT("-");
	DPRINT(_valueMin);
	DPRINT("=");
	DPRINTLN(tempValue);

	tempValue /= _totalValueRange;

	DPRINT("step2: divide by");
	DPRINT(_totalValueRange);
	DPRINT("=");
	DPRINTLN(tempValue);

	DPRINT("step2: *");
	DPRINT(_rangeInDegree);
	DPRINT("*");
	DPRINTLN(_stepsPerDegree);

	_targetPosition = round(tempValue * _rangeInDegree * _stepsPerDegree);

	DPRINT("New state:");
	DPRINT(_currentValue);
	DPRINT(":Dail position:");
	DPRINTLN(_targetPosition);
	DPRINT("End VidStepper setValue");

	return 0;
}

int VidStepper::run()
{
	bool moveForward;
	int i;

	if (_currentPosition != _targetPosition)
	{
		moveForward = (_targetPosition > _currentPosition);
		_doStep(moveForward);
		_currentPosition += moveForward ? 1 : -1;
		return 1;
	}

	return 0;
}

void VidStepper::powerOn()
{
	_isPowered = true;
}

void VidStepper::powerOff()
{
	_isPowered = false;
}

void _myDelay(int delayTime)
{
	int i, j;

	for (i = 0; i < delayTime; i++)
		for (j = 0; j < 100; j++);
}

// actual function to make one step
void VidStepper::_doStep(bool moveForward)
{
	//	DPRINTLN("Start VidStepper _doStep");

	if (_isReverse) moveForward = !moveForward;

	digitalWrite(_pinDir, moveForward ? LOW : HIGH);
	//delay(_pulseSetupWidth);

	digitalWrite(_pinStep, HIGH);
	delayMicroseconds(_pulseWidth);
	//_myDelay(_pulseWidth);
	digitalWrite(_pinStep, LOW);
	//delay(_pulseWidth);

//	DPRINTLN("End VidStepper _doStep");
}

bool VidStepper::setReverse(bool isReverse)
{
	bool oldValue = _isReverse;
	_isReverse = isReverse;

	return oldValue;
}

// Blocks until the target position is reached and stopped
void VidStepper::runToPosition()
{
	DPRINTLN("Start VidStepper runToPosition");
	int i = 0;
	while (run()) {
#if defined(ARDUINO_ARCH_ESP8266)
		if (i++ > 1000) {
			ESP.wdtFeed();
			i = 0;
		}
#endif
	};

	DPRINTLN("End VidStepper runToPosition");
}

void VidStepper::setMinPulseWidth(unsigned int minWidth)
{
	_pulseWidth = minWidth;
}