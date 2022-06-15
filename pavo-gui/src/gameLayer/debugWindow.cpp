#include "debugWindow.h"
#include "imgui_internal.h"
#undef min
#undef max

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

								cliRezult = rezult.value;
				memset(input, 0, sizeof(input));
			}

			ImGui::Text("Rezult: %" IM_PRIu64, cliRezult);

		}
		ImGui::EndChild();

		ImGui::BeginChild(112);
		{
			std::uint64_t data = 0;
			if (ImGui::InputScalar("Add break Point##u64", ImGuiDataType_U64, &data, 0, 0, 0,
				ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
			{
				debugger.set_breakpoint(data);
			}

			ImGui::Text("break points: %d", (int)debugger.breakpoints.size());

			std::vector<std::uint64_t> vectorOfBreakPoints = {};
			
			for (auto &it : debugger.breakpoints)
			{
				vectorOfBreakPoints.push_back(it.first);
			}

			for (int i = 0; i < vectorOfBreakPoints.size(); i++)
			{
				std::string str = fmt::format("{:#018x}", vectorOfBreakPoints[i]);

				if (ImGui::Button(str.c_str()))
				{
					debugger.remove_breakpoint(vectorOfBreakPoints[i]);
				}

			}

			//ImGui::ListBox(, &currentItemBreakPoint, vectorOfBreakPoints.data(),
			//	vectorOfBreakPoints.size(),
			//	std::min((decltype(vectorOfBreakPoints.size()))10ull, vectorOfBreakPoints.size()));


		}
		ImGui::EndChild();



	}
	ImGui::End();

	ImGui::PopID();

	return rez;
}
