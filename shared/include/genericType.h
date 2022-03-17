#pragma once

#include "imgui.h"
#include <cstdint>
#include <string>
#include "cpuFeatures.h"

//todo(vlod) add vector types and CPU feature detector



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
	"sse float"
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
		return *(*T)(&data);
	}

};


template<int ID>
inline void typeInput(GenericType &data, bool *pressedEnter = 0, bool *changed = 0, std::string *str = 0)
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
	else if (itemCurrent == t_sse_float)
	{
		pressed = ImGui::InputScalarN("##sseFloat", ImGuiDataType_Float, data.ptr(), 4, 0, 0, 0);
	}else
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
