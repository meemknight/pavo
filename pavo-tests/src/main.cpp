#include <iostream>
#include "virtualQuery.h"


int main()
{
	PROCESS p = GetModuleHandle(nullptr);
	OppenedQuery q = initVirtualQuery(p);
	void* low;
	void* high;
	int flags;

	void* allocated_memory[10] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	size_t sizes[10] = { 50 * sizeof(int), 30 * sizeof(int), 100 * sizeof(int), 123 * sizeof(int), 200 * sizeof(int),
						 80 * sizeof(int), 144 * sizeof(int), 20 * sizeof(int), 31 * sizeof(int), 64 * sizeof(int) };
	int alloc_type = MEM_COMMIT | MEM_RESERVE;
	int flags_array[10] = { memQueryFlags_None,
							memQueryFlags_Read,
							memQueryFlags_Write,
							memQueryFlags_Execute,
							memQueryFlags_Comitted,
							memQueryFlags_Read || memQueryFlags_Write,
							memQueryFlags_Read || memQueryFlags_Execute,
							memQueryFlags_Comitted || memQueryFlags_Write,
							memQueryFlags_Write || memQueryFlags_Execute || memQueryFlags_Read,
							memQueryFlags_Read || memQueryFlags_Write || memQueryFlags_Execute || memQueryFlags_Comitted };

	// memory allocation
	for(int i = 0; i < 10; i++)
		allocated_memory[i] = VirtualAlloc(nullptr, sizes[i], alloc_type, flags_array[i]);

	// memory validation
	std::string err_code[10];
	bool valid = true;
	while (getNextQuery(q, low, high, flags))
	{
		for (int i = 0; i < 10; i++)
			if (low == allocated_memory[i])
			{
				if ((char*)allocated_memory[i] + sizes[i] != (char*)high)
					err_code[i].append("high ");
				if (flags != flags_array[i])
					err_code[i].append("flags ");
				valid = false;

				i = 10;
			}
	}

	if (!valid)
	{
		for (int i = 0; i < 10; i++)
			if (!err_code[i].empty())
				std::cout << "pentru zona " << i << " nu merge: " << err_code[i] << "\n";
		// cerr
	}
	else
	{
		//
		std::cout << "e bine vere\n";
	}

	return 0;
}