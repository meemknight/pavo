#include "debugWindow.h"

std::string DebuggerWindow::start(std::string name, int id)
{
	this->name = name;
	this->id = id;

	//no error
	return "error, couldn't fint the\npath specified.";
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
