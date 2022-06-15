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

DebugRezult DebuggerWindow::render()
{
	DebugRezult rez = {"", 1};

	if (name == "") { return DebugRezult{"", 0}; }

	ImGui::PushID(id);

	if (ImGui::Begin(name.c_str()))
	{

		ImGui::Text("Pavo debugger");

		ImGui::BeginChild(111);
		{

			if (ImGui::InputText("Enter command", input, sizeof(input) - 1,
				ImGuiInputTextFlags_EnterReturnsTrue))
			{
				auto rezult = debugger.handle_command(input);

				if (!rezult.error.empty())
				{
					rez.err = rezult.error;
				}


				memset(input, 0, sizeof(input));
			}

			ImGui::Text("Rezult: %" IM_PRIu64, cliRezult);

		}
		ImGui::EndChild();

	}
	ImGui::End();

	ImGui::PopID();

	return rez;
}
