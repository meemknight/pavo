#include <glad/glad.h>
#include "gameLayer.h"
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imguiThemes.h"
#include "genericType.h"

#include <fstream>
#include <imfilebrowser.h>

struct GameData
{


}gameData;

ImGui::FileBrowser fileDialog;



bool initGame()
{

	fileDialog.SetTitle("title");
	fileDialog.SetTypeFilters({".h", ".cpp"});

	fileDialog.Open();

	imguiThemes::green();

	if(!platform::readEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData)))
	{
		gameData = GameData();
	}

	return true;
}

bool gameLogic(float deltaTime)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w= platform::getWindowSizeX();
	h = platform::getWindowSizeY();
	
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, w, h);

#pragma endregion


#pragma region input

	if (platform::isKeyPressedOn(platform::Button::F) && platform::isKeyHeld(platform::Button::LeftCtrl))
	{
		platform::setFullScreen(!platform::isFullScreen());
	}
#pragma endregion

	

	ImGui::Begin("Pavo");
	//static GenericType reg = {};
	//genericTypeInput<__COUNTER__>(reg);
	
	
	//imgui_addons::ImGuiFileBrowser file_dialog;
	std::string path = "E:\\info\\C++\\asmax\\main.cpp";
	std::ifstream fin(path);
	std::string source_code((std::istreambuf_iterator<char>(fin)), (std::istreambuf_iterator<char>()));
	
	//std::cout << source_code.size() << "\n";
	int line_number = 0;
	source_code.insert(0, std::to_string(line_number) + "\t");
	line_number++;
	for (int i = 0; i < source_code.size(); i++)
	{
		if (source_code[i] == '\n')
		{
			source_code.insert(i + 1, std::to_string(line_number) + "\t");
			line_number++;
		}
	
		//std::cout << i << "\n";
	}
	
	
	static float wrap_width = 600.0f;
	//ImGui::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	
	ImGui::Text("Continut fisier:");
	ImGui::Spacing();
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 marker_min = ImVec2(pos.x + wrap_width, pos.y);
	ImVec2 marker_max = ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight());
	ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
	ImGui::Text(source_code.c_str(), wrap_width);
	
	// Draw actual text bounding box, following by marker of our expected limit (should not overlap!)
	draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
	//draw_list->AddRectFilled(marker_min, marker_max, IM_COL32(255, 0, 255, 255));
	ImGui::PopTextWrapPos();
	
	ImGui::Text("ceva");
	
	ImGui::End();

	fileDialog.Display();


	if (fileDialog.HasSelected())
	{
		std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
		fileDialog.ClearSelected();
	}

	//std::cout << platform::getTypedInput();

#pragma region set finishing stuff

	return true;
#pragma endregion

}

void closeGame()
{

	platform::writeEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

}
