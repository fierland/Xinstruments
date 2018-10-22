#include "Can2XPlane.h"

Can2XPlane::Can2XPlane()
{
}

Can2XPlane::~Can2XPlane()
{
}

char* Can2XPlane::fromCan2Xplane(uint8_t canID)
{
	DPRINTINFO("START");

	int i = 0;

	while (canasXplaneTable[i].canasId != 0 && canasXplaneTable[i].canasId != canID);

	DPRINTINFO("STOP");

	return canasXplaneTable[i].xplaneId;
}