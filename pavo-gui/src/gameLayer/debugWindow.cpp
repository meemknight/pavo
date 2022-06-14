#include "debugWindow.h"

std::string DebuggerWindow::start(std::string name, int id)
{
	this->name = name;
	this->id = id;

	if (!debugger.start(name))
	{
		return "error, couldn't open process";

	}

	//no error
	return "";
}

bool DebuggerWindow::render()
{
	ImGui::PushID(id);

	if (ImGui::Begin(name.c_str()))
	{

		ImGui::Text("Pavo debugger");

	}
	ImGui::End();

	ImGui::PopID();

	return true;
}
