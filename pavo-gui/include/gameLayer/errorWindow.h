#pragma once
#include "imgui.h"
#include <string>


struct ErrorWindow
{

	std::string errorMessage;
	int id = 0;

	ErrorWindow() {};
	ErrorWindow(std::string errorMessage, int id):errorMessage(errorMessage), id(id) {};

	bool render();
	bool isOpen = true;

};