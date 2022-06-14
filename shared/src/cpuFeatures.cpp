#include "cpuFeatures.h"
#include "cpuinfo_x86.h"

static const cpu_features::X86Features detectedFeatures = cpu_features::GetX86Info().features;

CpuFeatures getCpuFeatures()
{
	CpuFeatures features;
	
	features.hasSSE = detectedFeatures.sse;
	features.hasAVX = detectedFeatures.avx;
	features.hasAVX2 = detectedFeatures.avx2;

	return features;
}
