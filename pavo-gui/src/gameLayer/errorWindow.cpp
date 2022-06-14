#include "errorWindow.h"

bool ErrorWindow::render()
{
	ImGui::PushID(id);

	if (ImGui::Begin("error", &isOpen))
	{

		ImGui::Text(errorMessage.c_str());

	}
	ImGui::End();

	ImGui::PopID();

	return isOpen;
}
