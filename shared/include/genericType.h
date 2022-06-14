#pragma once

#include "imgui.h"
#include <cstdint>
#include <string>
#include "cpuFeatures.h"


#if PAVO_WIN32
#include <Windows.h>

using PID = DWORD;
using PROCESS = HANDLE;

namespace dwarf_wrapper
{
        struct dwarf { dwarf(auto&&...) {} };
        struct die   { die(auto&&...)   {} };
        struct line_table
        {
                line_table(auto&&...) {}
                struct iterator { iterator(auto&&...) {} };
        };

        struct elf
        {
                elf(auto&&...) {}

                static int create_loader(auto&&...)
                {
                        return 0;
                }
        };
}

namespace elf_wrapper
{
        struct elf { elf(auto&&...) {} };

        int create_mmap_loader(auto&&...)
        {
                return 0;
        }
}

#elif defined PAVO_UNIX 
using PID = pid_t;
using PROCESS = PID;
#include "elf++.hh"
#include "dwarf++.hh"
namespace dwarf_wrapper = dwarf;
namespace elf_wrapper   = elf;
#endif

//for imgui
#ifdef _MSC_VER
#define IM_PRId64   "I64d"
#define IM_PRIu64   "I64u"
#define IM_PRIx64   "I64X"
#else
#define IM_PRId64   "lld"
#define IM_PRIu64   "llu"
#define IM_PRIx64   "llX"
#endif

enum Types
{
	t_signed8,
	t_unsigned8,
	t_signed16,
	t_unsigned16,
	t_signed32,
	t_unsigned32,
	t_signed64,
	t_unsigned64,
	t_real32,
	t_real64,
	t_sse_float,
	t_sse_double,
	t_sse_signed64int,
	t_sse_unsigned64int,
	t_sse_signed32int,
	t_sse_unsigned32int,
	t_sse_signed16int,
	t_sse_unsigned16int,
	t_sse_signed8int,
	t_sse_unsigned8int,
	t_avx_float,
	t_avx_double,
	t_avx_signed64int,
	t_avx_unsigned64int,
	t_avx_signed32int,
	t_avx_unsigned32int,
	t_avx_signed16int,
	t_avx_unsigned16int,
	t_avx_signed8int,
	t_avx_unsigned8int,
	t_string,
	typesCount
};


constexpr const char *types[] =
{
	"signed8",
	"unsigned8",
	"signed16",
	"unsigned16",
	"signed32",
	"unsigned32",
	"signed64",
	"unsigned64",
	"float",
	"double",

	//SSE
	"float vector(4)",
	"double vector(2)",
	"signed64 vector(2)",
	"unsigned64 vector(2)",
	"signed32 vector(4)",
	"unsigned vector(4)",
	"signed16 vector(8)",
	"unsigned16 vector(8)",
	"signed8 vector(16)",
	"unsigned8 vector(16)",
	
	//AVX
	"float vector(8)",
	"double vector(4)",

	//AVX2
	"signed64 vector(4)",
	"unsigned64 vector(4)",
	"signed32 vector(8)",
	"unsigned vector(8)",
	"signed16 vector(16)",
	"unsigned16 vector(16)",
	"signed8 vector(32)",
	"unsigned8 vector(32)",

	"string",
};

union Type
{
	int8_t signed8;
	uint8_t unsigned8;
	int16_t signed16;
	uint16_t unsigned16;
	int32_t signed32;
	uint32_t unsigned32;
	int64_t signed64;
	uint64_t unsigned64;
	
	SSE_float_t sseFloat;
	SSE_double_t sseDouble;
	SSE_int_t sseInt;
	int64_t sseInt64[2];
	int32_t sseInt32[4];
	int16_t sseInt16[8];
	int8_t sseInt8[16];
	uint64_t sseUnsignedInt64[2];
	uint32_t sseUnsignedInt32[4];
	uint16_t sseUnsignedInt16[8];
	uint8_t sseUnsignedInt8[16];

	AVX_float_t avxFloat;
	AVX_double_t avxDobule;
	AVX_int_t avxInt;
	int64_t avxInt64[4];
	int32_t avxInt32[8];
	int16_t avxInt16[16];
	int8_t avxInt8[32];
	uint64_t avxUnsignedInt64[4];
	uint32_t avxUnsignedInt32[8];
	uint16_t avxUnsignedInt16[16];
	uint8_t avxUnsignedInt8[32];

	float real32;
	double real64;
};

//https://github.com/meemknight/memGrab
struct GenericType
{
	Type data = {};
	int type = 0;

	void *ptr() { return &data; }
	int getBytesSize()
	{
		if (type == t_signed8 ||
			type == t_unsigned8)
		{
			return 1;
		}
		if (type == t_signed16 ||
			type == t_unsigned16)
		{
			return 2;
		}
		if (type == t_signed32 ||
			type == t_unsigned32 ||
			type == t_real32)
		{
			return 4;
		}
		if (type == t_signed64 ||
			type == t_unsigned64 ||
			type == t_real64)
		{
			return 8;
		}
		if (type == t_sse_float)
		{
			return 4 * 4;
		}

		return 0;
	}

	//todo(vlod) some sfinae on T?
	template<class T>
	T &getData() 
	{
		return *(T*)(&data);
	}

};


template<int ID>
inline void genericTypeInput(GenericType &data, bool *pressedEnter = 0, bool *changed = 0, std::string *str = 0)
{
	ImGui::PushID(ID);

	bool pressed = 0;

	//static int v = 0;
	//static float f = 0;
	//static char c = 0;

	bool acceptStrings = str != nullptr;

	static int itemCurrent = t_signed32;
	static int lastItemCurrent = t_signed32;
	if (itemCurrent != lastItemCurrent)
	{
		data = {};
		lastItemCurrent = itemCurrent;
		if (changed) { *changed = 1; }
	}
	else
	{
		if (changed) { *changed = 0; }
	}

	ImGui::Combo("##types list box", &itemCurrent, types, IM_ARRAYSIZE(types) - (int)!acceptStrings);

	if (itemCurrent == t_signed8)
	{
		pressed = ImGui::InputScalar("##s8", ImGuiDataType_S8, data.ptr(), 0, 0, 0);
	}
	else
	if (itemCurrent == t_unsigned8)
	{
		pressed = ImGui::InputScalar("##u8", ImGuiDataType_U8, data.ptr(), 0, 0, 0);
	}
	else
	if (itemCurrent == t_signed16)
	{
		pressed = ImGui::InputScalar("##s16", ImGuiDataType_S16, data.ptr(), 0, 0, 0);
	}
	else
	if (itemCurrent == t_unsigned16)
	{
		pressed = ImGui::InputScalar("##u16", ImGuiDataType_U16, data.ptr(), 0, 0, 0);
	}
	else
	if (itemCurrent == t_signed32)
	{
		pressed = ImGui::InputScalar("##s32", ImGuiDataType_S32, data.ptr(), 0, 0, 0);
	}
	else
	if (itemCurrent == t_unsigned32)
	{
		pressed = ImGui::InputScalar("##u32", ImGuiDataType_U32, data.ptr(), 0, 0, 0);
	}
	else
	if (itemCurrent == t_signed64)
	{
		pressed = ImGui::InputScalar("##s64", ImGuiDataType_S64, data.ptr(), 0, 0, 0);
	}
	else
	if (itemCurrent == t_unsigned8)
	{
		pressed = ImGui::InputScalar("##u64", ImGuiDataType_U64, data.ptr(), 0, 0, 0);
	}
	else
	if (itemCurrent == t_real32)
	{
		pressed = ImGui::InputScalar("##real32", ImGuiDataType_Float, data.ptr(), 0, 0, 0);
	}
	else
	if (itemCurrent == t_real64)
	{
		pressed = ImGui::InputScalar("##real64", ImGuiDataType_Double, data.ptr(), 0, 0, 0);
	}

	//SSE
	else if (itemCurrent == t_sse_float)
	{
		pressed = ImGui::InputScalarN("##sseFloat", ImGuiDataType_Float, data.ptr(), 4, 0, 0, 0);
	}
	else if (itemCurrent == t_sse_double)
	{
	pressed = ImGui::InputScalarN("##sseDouble", ImGuiDataType_Double, data.ptr(), 2, 0, 0, 0);
	}
	else if (itemCurrent == t_sse_signed64int)
	{
		pressed = ImGui::InputScalarN("##sseSigned64", ImGuiDataType_S64, data.ptr(), 2, 0, 0, 0);
	}
	else if (itemCurrent == t_sse_unsigned64int)
	{
		pressed = ImGui::InputScalarN("##sseUnsigned64", ImGuiDataType_U64, data.ptr(), 2, 0, 0, 0);
	}
	else if (itemCurrent == t_sse_signed32int)
	{
		pressed = ImGui::InputScalarN("##sseSigned32", ImGuiDataType_S32, data.ptr(), 4, 0, 0, 0);
	}
	else if (itemCurrent == t_sse_unsigned32int)
	{
		pressed = ImGui::InputScalarN("##sseUnsigned32", ImGuiDataType_U32, data.ptr(), 4, 0, 0, 0);
	}
	else if (itemCurrent == t_sse_signed16int)
	{
		pressed = ImGui::InputScalarN("##sseSigned16", ImGuiDataType_S16, data.ptr(), 8, 0, 0, 0);
	}
	else if (itemCurrent == t_sse_unsigned16int)
	{
		pressed = ImGui::InputScalarN("##sseUnsigned16", ImGuiDataType_U16, data.ptr(), 8, 0, 0, 0);
	}
	else if (itemCurrent == t_sse_signed8int)
	{
		pressed = ImGui::InputScalarN("##sseSigned8", ImGuiDataType_S8, data.ptr(), 16, 0, 0, 0);
	}
	else if (itemCurrent == t_sse_unsigned8int)
	{
		pressed = ImGui::InputScalarN("##sseUnsigned8", ImGuiDataType_U8, data.ptr(), 16, 0, 0, 0);
	}

	//AVX
	else if (itemCurrent == t_avx_float)
	{
		pressed = ImGui::InputScalarN("##avxFloat", ImGuiDataType_Float, data.ptr(), 8, 0, 0, 0);
	}
	else if (itemCurrent == t_avx_double)
	{
		pressed = ImGui::InputScalarN("##avxDouble", ImGuiDataType_Double, data.ptr(), 4, 0, 0, 0);
	}

	//AVX2
	else if (itemCurrent == t_avx_signed64int)
	{
		pressed = ImGui::InputScalarN("##avxSigned64", ImGuiDataType_S64, data.ptr(), 4, 0, 0, 0);
	}
	else if (itemCurrent == t_avx_unsigned64int)
	{
		pressed = ImGui::InputScalarN("##avxUnsigned64", ImGuiDataType_U64, data.ptr(), 4, 0, 0, 0);
	}
	else if (itemCurrent == t_avx_signed32int)
	{
		pressed = ImGui::InputScalarN("##avxSigned32", ImGuiDataType_S32, data.ptr(), 8, 0, 0, 0);
	}
	else if (itemCurrent == t_avx_unsigned32int)
	{
		pressed = ImGui::InputScalarN("##avxUnsigned32", ImGuiDataType_U32, data.ptr(), 8, 0, 0, 0);
	}
	else if (itemCurrent == t_avx_signed16int)
	{
		pressed = ImGui::InputScalarN("##avxSigned16", ImGuiDataType_S16, data.ptr(), 16, 0, 0, 0);
	}
	else if (itemCurrent == t_avx_unsigned16int)
	{
		pressed = ImGui::InputScalarN("##avxUnsigned16", ImGuiDataType_U16, data.ptr(), 16, 0, 0, 0);
	}
	else if (itemCurrent == t_avx_signed8int)
	{
		pressed = ImGui::InputScalarN("##avxSigned8", ImGuiDataType_S8, data.ptr(), 32, 0, 0, 0);
	}
	else if (itemCurrent == t_avx_unsigned8int)
	{
		pressed = ImGui::InputScalarN("##avxUnsigned8", ImGuiDataType_U8, data.ptr(), 32, 0, 0, 0);
	}


	if (itemCurrent == t_string && acceptStrings)
	{
		char buff[260];
		strcpy(buff, str->c_str());
		pressed = ImGui::InputText("##string", buff, 260);
		*str = buff;
	}

	ImGui::PopID();

	if (pressedEnter)
	{
		*pressedEnter = 0;
	}

	data.type = itemCurrent;

	return;
}
