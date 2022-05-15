#include <iostream>
#include "virtualQuery.h"

//todo move later
//https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
std::string getLastErrorString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
	{
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}

int main()
{
	PROCESS p = GetCurrentProcess();
	OppenedQuery q = initVirtualQuery(p);
	void* low;
	void* high;
	int flags;

	constexpr int SIZE = 5;
	
	void* allocated_memory[SIZE] = {};
	size_t sizes[SIZE] = { 50 * sizeof(int), 30 * sizeof(int), 100 * sizeof(int), 123 * sizeof(int), 200 * sizeof(int)};
	int allocType = MEM_COMMIT | MEM_RESERVE;
	int flagsArray[SIZE] = {	
							memQueryFlags_Comitted | memQueryFlags_Read,
							memQueryFlags_Comitted | memQueryFlags_Execute,
							memQueryFlags_Comitted | memQueryFlags_Read | memQueryFlags_Write,
							memQueryFlags_Comitted | memQueryFlags_Read | memQueryFlags_Execute,
							memQueryFlags_Comitted | memQueryFlags_Read | memQueryFlags_Write | memQueryFlags_Execute  };

	int virtualAllocFlags[SIZE] = {
		PAGE_READONLY,
		PAGE_EXECUTE,
		PAGE_READWRITE,
		PAGE_EXECUTE_READ,
		PAGE_EXECUTE_READWRITE
	};


	// memory allocation
	for(int i = 0; i < SIZE; i++)
		allocated_memory[i] = VirtualAlloc(nullptr, sizes[i], allocType, virtualAllocFlags[i]);

	// memory validation
	std::string err_code[SIZE];
	bool valid = true;
	while (getNextQuery(q, low, high, flags))
	{
		for (int i = 0; i < SIZE; i++)
			if (low == allocated_memory[i])
			{
					
				if (flags != flagsArray[i])
				{
					err_code[i].append("flags ");
					valid = false;
				}

				break;
			}
	}


	if (!valid)
	{
		for (int i = 0; i < SIZE; i++)
			if (!err_code[i].empty())
				std::cout << "pentru zona " << i << " nu merge: " << err_code[i] << "\n";
	}
	else
	{
		std::cout << "e bine vere\n";
	}

	std::cin.ignore();
	std::cin.get();
	return 0;
}