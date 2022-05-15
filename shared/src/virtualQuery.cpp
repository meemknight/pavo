#include "virtualQuery.h"

#ifdef PAVO_WIN32

OppenedQuery initVirtualQuery(PROCESS process)
{
	OppenedQuery q = {};

	q.queriedProcess = process;
	q.baseQueriedPtr = 0;
	return q;
}

bool getNextQuery(OppenedQuery &query, void *&low, void *&hi, int &flags)
{

	if (query.queriedProcess == 0) { return false; }

	flags = memQueryFlags_None;
	low = nullptr;
	hi = nullptr;

	MEMORY_BASIC_INFORMATION memInfo;

	bool rez = 0;
	while (true)
	{
		rez = VirtualQueryEx(query.queriedProcess, (void *)query.baseQueriedPtr, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

		if (!rez)
		{
			query = {};
			return false;
		}

		query.baseQueriedPtr = (char *)memInfo.BaseAddress + memInfo.RegionSize;

		if (memInfo.State == MEM_COMMIT)
		{
			flags = memQueryFlags_Comitted;

			//good page
			if (memInfo.Protect == PAGE_READONLY)
			{
				flags |= memQueryFlags_Read;
			}
			else if (memInfo.Protect == PAGE_READWRITE)
			{
				flags |= (memQueryFlags_Read | memQueryFlags_Write);
			}
			else if (memInfo.Protect == PAGE_EXECUTE)
			{
				flags |= memQueryFlags_Execute;
			}
			else if (memInfo.Protect == PAGE_EXECUTE_READ)
			{
				flags |= (memQueryFlags_Execute | memQueryFlags_Read);
			}
			else if (memInfo.Protect == PAGE_EXECUTE_READWRITE)
			{
				flags |= (memQueryFlags_Execute | memQueryFlags_Read | memQueryFlags_Write);
			}
		}

		low = memInfo.BaseAddress;
		hi = (char *)memInfo.BaseAddress + memInfo.RegionSize;
		return true;
	}
}

#endif

#ifdef PAVO_LINUX
#include <vector>
#include <algorithm>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <stdio.h>
#include <fstream>

OppenedQuery initVirtualQuery(PROCESS process)
{
	OppenedQuery query{};

	char fileName[256] = {};
	sprintf(fileName, "/proc/%ld/maps", (long)process);

	std::ifstream file(fileName);
	if (!file.is_open()) { return query; /*fail*/ }

	std::vector<char> data{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
	query.mapData = std::stringstream(std::string(data.begin(), data.end()));

	file.close();

	return query;
}

bool getNextQuery(OppenedQuery &query, void *&low, void *&hi, int &flags)
{
	flags = memQueryFlags_Read | memQueryFlags_Write | memQueryFlags_Comitted;

	if (query.mapData.eof()) { query = OppenedQuery(); return false; }

	std::string line;
	std::getline(query.mapData, line);

	std::stringstream lineStream(line);

	std::string adress;
	std::string permisions;
	std::string offset;
	std::string device;
	std::string inode;
	std::string pathName;

	lineStream >> adress >> permisions >> offset >> device >> inode >> pathName;

	auto pos = adress.find('-');

	if (pos == adress.npos)
	{
		return false;
	}

	std::string beg(adress.begin(), adress.begin() + pos);
	std::string end(adress.begin() + pos + 1, adress.end());

	size_t lowVal = std::stoull(beg, 0, 16);
	size_t highVal = std::stoull(end, 0, 16);

	low = (void *)lowVal;
	hi = (void *)highVal;

	return true;
}


#endif