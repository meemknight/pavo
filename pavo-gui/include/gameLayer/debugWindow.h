#pragma once
#include "imgui.h"
#include <string>
#include "debugger.h"

struct DebuggerWindow
{

	DebuggerWindow() {};

	std::string name;

	//returns error
	std::string start(std::string name, int id);

	bool render();
	int id = 0;

	debugger_t debugger;
};