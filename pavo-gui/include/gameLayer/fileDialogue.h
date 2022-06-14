#pragma once

#include "imgui.h"
#include <imfilebrowser.h>
#include <string>


struct FileDialogue
{
	FileDialogue() {};

	ImGui::FileBrowser fileDialog;

	void start();

	bool render();

	std::string getRezult();

};