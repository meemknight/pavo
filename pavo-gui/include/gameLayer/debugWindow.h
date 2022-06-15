#pragma once
#include "imgui.h"
#include <string>
#include "debugger.h"
#include "hexEditor.h"

struct DebugRezult
{
	std::string err = "";
	bool running = 1;
};

struct DebuggerWindow
{
	
	DebuggerWindow() {};

	char input[256] = {};
	std::uint64_t cliRezult = 0;

	std::string name;

	//returns error
	std::string start(std::string name, int id);
	int currentItemBreakPoint = 0;

	HexEditor hexEditor;
	DebugRezult render();
	int id = 0;

	debugger_t debugger;
};