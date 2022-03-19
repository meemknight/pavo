#pragma once

#ifdef PAVO_UNIX
#include <x86intrin.h>
#else
#include <intrin.h>
#endif

//https://en.wikipedia.org/wiki/Streaming_SIMD_Extensions
//http://www.nacad.ufrj.br/online/intel/vtune/users_guide/mergedProjects/analyzer_ec/mergedProjects/reference_olh/mergedProjects/instructions/instruct32_hh/intrins_api.htm
//https://en.wikipedia.org/wiki/Advanced_Vector_Extensions

using SSE_float_t = __m128;
using SSE_double_t = __m128d;
using SSE_int_t = __m128i;

using AVX_float_t = __m256;
using AVX_double_t = __m256d;
using AVX_int_t = __m256d;



struct CpuFeatures
{
	//all 128 bit XMM registers
	bool hasSSE = false;

	//256 bit YMM floating point registers
	bool hasAVX = false;
	
	//256 bit YMM integer registers
	bool hasAVX2 = false;

	friend CpuFeatures getCpuFeatures();

private:
	CpuFeatures() = default;

};
