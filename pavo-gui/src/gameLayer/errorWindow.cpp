#include "errorWindow.h"

bool ErrorWindow::render()
{
	ImGui::PushID(id);

	if (ImGui::Begin("error", &isOpen))
	{

		ImGui::SetWindowSize(ImVec2(100, 50));

		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), errorMessage.c_str());

		//ImGui::Text(errorMessage.c_str());

	}
	ImGui::End();

	ImGui::PopID();

	return isOpen;
}
