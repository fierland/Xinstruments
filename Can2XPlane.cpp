#include "Can2XPlane.h"

Can2XPlane::Can2XPlane()
{}

Can2XPlane::~Can2XPlane()
{}

char* Can2XPlane::fromCan2Xplane(uint8_t canID)
{
	DPRINTINFO("START");

	int i = 0;

	while (canasXplaneTable[i].canasId != 0 && canasXplaneTable[i].canasId != canID);

	DPRINTINFO("STOP");

	return canasXplaneTable[i].xplaneId;
}

const CanasXplaneTrans* Can2XPlane::fromCan2XplaneElement(uint8_t canID)
{
	DPRINTINFO("START");
	const CanasXplaneTrans* foundItem;

	int i = 0;

	while (canasXplaneTable[i].canasId != 0 && canasXplaneTable[i].canasId != canID);

	foundItem = &(canasXplaneTable[i]);

	DPRINTINFO("STOP");

	return foundItem;
}