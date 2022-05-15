#pragma once
#include "genericType.h"
#include <sstream>


enum
{
	memQueryFlags_ = 0,
	memQueryFlags_None = 0,
	memQueryFlags_Read = 0b0001,
	memQueryFlags_Write = 0b0010,
	memQueryFlags_Execute = 0b0100,
	memQueryFlags_Comitted = 0b1000,
};

#if PAVO_WIN32

struct OppenedQuery
{
	PROCESS queriedProcess = 0;
	char *baseQueriedPtr = (char *)0x0;
	bool oppened() { return queriedProcess != 0; }
};

#endif

#if defined PAVO_UNIX

struct OppenedQuery
{
	std::stringstream mapData;
	bool oppened() { return mapData.rdbuf()->in_avail() != 0; }
};

#endif


OppenedQuery initVirtualQuery(PROCESS process);
bool getNextQuery(OppenedQuery &query, void *&low, void *&hi, int &flags);