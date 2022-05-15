#include "virtualQuery.h"


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