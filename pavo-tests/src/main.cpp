#include <iostream>
#include "virtualQuery.h"
#include "errorLogging.h"

#undef min
#undef max

#include <catch2/catch_template_test_macros.hpp>




TEST_CASE("Correct memory flags", "[memory parse test 1]") // 1
{
	PROCESS p = GetCurrentProcess();
	OppenedQuery q = initVirtualQuery(p);

	int allocType = MEM_COMMIT | MEM_RESERVE;
	int alloc_flags = PAGE_READONLY;
	size_t size = 50 * sizeof(int);
	int verif_flags = memQueryFlags_Comitted | memQueryFlags_Read;

	void* low;
	void* high;
	int flags;

	void* allocated_memory = VirtualAlloc(nullptr, size, allocType, alloc_flags);

	while (getNextQuery(q, low, high, flags))
		if (low == allocated_memory)
		{ 
			REQUIRE(flags == verif_flags);
			break;
		}
}

TEST_CASE("Correct memory flags", "[memory parse test 2]") // 2
{
	PROCESS p = GetCurrentProcess();
	OppenedQuery q = initVirtualQuery(p);

	int allocType = MEM_COMMIT | MEM_RESERVE;
	int alloc_flags = PAGE_EXECUTE;
	size_t size = 30 * sizeof(int);
	int verif_flags = memQueryFlags_Comitted | memQueryFlags_Execute;

	void* low;
	void* high;
	int flags;

	void* allocated_memory = VirtualAlloc(nullptr, size, allocType, alloc_flags);

	while (getNextQuery(q, low, high, flags))
		if (low == allocated_memory)
		{
			REQUIRE(flags == verif_flags);
			break;
		}
}

TEST_CASE("Correct memory flags", "[memory parse test 3]") // 3
{
	PROCESS p = GetCurrentProcess();
	OppenedQuery q = initVirtualQuery(p);

	int allocType = MEM_COMMIT | MEM_RESERVE;
	int alloc_flags = PAGE_READWRITE;
	size_t size = 100 * sizeof(int);
	int verif_flags = memQueryFlags_Comitted | memQueryFlags_Read | memQueryFlags_Write;

	void* low;
	void* high;
	int flags;

	void* allocated_memory = VirtualAlloc(nullptr, size, allocType, alloc_flags);

	while (getNextQuery(q, low, high, flags))
		if (low == allocated_memory)
		{
			REQUIRE(flags == verif_flags);
			break;
		}
}

TEST_CASE("Correct memory flags", "[memory parse test 4]") // 4
{
	PROCESS p = GetCurrentProcess();
	OppenedQuery q = initVirtualQuery(p);

	int allocType = MEM_COMMIT | MEM_RESERVE;
	int alloc_flags = PAGE_EXECUTE_READ;
	size_t size = 123 * sizeof(int);
	int verif_flags = memQueryFlags_Comitted | memQueryFlags_Read | memQueryFlags_Execute;

	void* low;
	void* high;
	int flags;

	void* allocated_memory = VirtualAlloc(nullptr, size, allocType, alloc_flags);

	while (getNextQuery(q, low, high, flags))
		if (low == allocated_memory)
		{
			REQUIRE(flags == verif_flags);
			break;
		}
}

TEST_CASE("Correct memory flags", "[memory parse test 5]") // 5
{
	PROCESS p = GetCurrentProcess();
	OppenedQuery q = initVirtualQuery(p);

	int allocType = MEM_COMMIT | MEM_RESERVE;
	int alloc_flags = PAGE_EXECUTE_READWRITE;
	size_t size = 200 * sizeof(int);
	int verif_flags = memQueryFlags_Comitted | memQueryFlags_Read | memQueryFlags_Write | memQueryFlags_Execute;

	void* low;
	void* high;
	int flags;

	void* allocated_memory = VirtualAlloc(nullptr, size, allocType, alloc_flags);

	while (getNextQuery(q, low, high, flags))
		if (low == allocated_memory)
		{
			REQUIRE(flags == verif_flags);
			break;
		}
}

//TEST_CASE("Correct memory flags", "[virtual query api]")
//{
//	PROCESS p = GetCurrentProcess();
//	OppenedQuery q = initVirtualQuery(p);
//	void* low;
//	void* high;
//	int flags;
//
//	constexpr int SIZE = 5;
//	
//	void* allocated_memory[SIZE] = {};
//	size_t sizes[SIZE] = { 50 * sizeof(int), 30 * sizeof(int), 100 * sizeof(int), 123 * sizeof(int), 200 * sizeof(int)};
//	int allocType = MEM_COMMIT | MEM_RESERVE;
//	int flagsArray[SIZE] = {	
//							memQueryFlags_Comitted | memQueryFlags_Read,
//							memQueryFlags_Comitted | memQueryFlags_Execute,
//							memQueryFlags_Comitted | memQueryFlags_Read | memQueryFlags_Write,
//							memQueryFlags_Comitted | memQueryFlags_Read | memQueryFlags_Execute,
//							memQueryFlags_Comitted | memQueryFlags_Read | memQueryFlags_Write | memQueryFlags_Execute  };
//
//	int virtualAllocFlags[SIZE] = {
//		PAGE_READONLY,
//		PAGE_EXECUTE,
//		PAGE_READWRITE,
//		PAGE_EXECUTE_READ,
//		PAGE_EXECUTE_READWRITE
//	};
//
//
//	// memory allocation
//	for(int i = 0; i < SIZE; i++)
//		allocated_memory[i] = VirtualAlloc(nullptr, sizes[i], allocType, virtualAllocFlags[i]);
//
//	while (getNextQuery(q, low, high, flags))
//	{
//		for (int i = 0; i < SIZE; i++)
//			if (low == allocated_memory[i])
//			{
//				REQUIRE(flags == flagsArray[i]);
//				break;
//			}
//	}
//
//
//
//}



//unsigned int factorial(unsigned int number)
//{
//	return number <= 1 ? number : factorial(number - 1) * number;
//}
//
//TEST_CASE("Factorials are computed", "[factorial]")
//{
//	REQUIRE(factorial(1) == 1);
//	REQUIRE(factorial(2) == 2);
//	REQUIRE(factorial(3) == 6);
//	REQUIRE(factorial(10) == 3628800);
//
//	// This find a bug in the factorial implementation,
//	// try uncommenting it.
//	//REQUIRE( factorial(0) == 1 );
//
//
//	//std::cin.get();
//	//std::cin.get();
//	//std::cin.get();
//	//std::cin.get();
//}
