#include <iostream>
#include <fmt/core.h>
#include "cpuFeatures.h"

int main()
{

	fmt::print("Bonjour le monde!\n");

	fmt::print("sse: {}, avx: {}, avx2: {}\n", getCpuFeatures().hasSSE, getCpuFeatures().hasAVX, getCpuFeatures().hasAVX2);


	std::cin.get();
	return 0;
}