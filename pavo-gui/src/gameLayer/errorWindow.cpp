#include "errorWindow.h"

bool ErrorWindow::render()
{
	ImGui::PushID(id);

	if (ImGui::Begin("error", &isOpen, ImGuiWindowFlags_AlwaysAutoResize | 
		 ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize))
	{

		ImGui::SetWindowFontScale(1.2);
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), errorMessage.c_str());

		if (ImGui::Button("Close"))
		{
			isOpen = false;
		}
	}
	ImGui::End();

	ImGui::PopID();

	return isOpen;
}
