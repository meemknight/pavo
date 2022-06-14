#include <glad/glad.h>
#include "gameLayer.h"
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imguiThemes.h"
#include "genericType.h"
#include "debugger.h"
#include <vector>

#include <fstream>
#include <imfilebrowser.h>

#include "fileDialogue.h"
#include "debugWindow.h"
#include "errorWindow.h"

struct GameData
{


}gameData;


std::vector<FileDialogue> fileDialogues;
std::vector<DebuggerWindow> debuggers;
std::vector<ErrorWindow> errorWindows;
int uniqueId = 1000;

bool initGame()
{	

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

#pragma region window
	//static ImGui::FileBrowser fileDialog;

	ImVec2 vWindowSize = ImGui::GetMainViewport()->Size;
	ImVec2 vPos0 = ImGui::GetMainViewport()->Pos;
	ImGui::SetNextWindowPos(ImVec2((float)vPos0.x, (float)vPos0.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2((float)vWindowSize.x, (float)vWindowSize.y), 0);
	if (ImGui::Begin(
		"Pavo",
		/*p_open=*/nullptr,
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoTitleBar
		)
		)
	{

		static const ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
		ImGuiID dockSpace = ImGui::GetID("MainWindowDockspace");
		ImGui::DockSpace(dockSpace, ImVec2(0.0f, 0.0f), dockspaceFlags);
	
		if (ImGui::BeginMenuBar())
		{
	
			if (ImGui::BeginMenu("FILE"))
			{
	
				if (ImGui::Button("Open"))
				{
					//open file dialog
					fileDialogues.push_back(FileDialogue{});
					fileDialogues.back().start();
				}
				
				ImGui::EndMenu();
			}
	
	
			ImGui::EndMenuBar();
		}
		
		ImGui::Begin("test window");

			ImGui::Text("ceva");
			ImGui::Text("ceva");
			ImGui::Text("ceva");
			ImGui::Text("ceva");

		ImGui::End();
		
		
	}
	ImGui::End();



	for (int i = 0; i < fileDialogues.size(); i++)
	{
		if (!fileDialogues[i].render())
		{
			fileDialogues.erase(fileDialogues.begin() + i);
			i--;
		}

		auto rez = fileDialogues[i].getRezult();
		if (rez != "")
		{
			std::cout << rez << "\n";
			fileDialogues.erase(fileDialogues.begin() + i);
			i--;

			DebuggerWindow d = {};
			auto error = d.start(rez, uniqueId++);

			if (error == "")
			{
				debuggers.push_back(d);
			}
			else
			{
				errorWindows.push_back(ErrorWindow(error, uniqueId++));
			}

		}

	}

	for (int i = 0; i < debuggers.size(); i++)
	{
		if (!debuggers[i].render())
		{
			debuggers.erase(debuggers.begin() + i);
			i--;
		}
	}

	for (int i = 0; i < errorWindows.size(); i++)
	{
		if (!errorWindows[i].render())
		{
			errorWindows.erase(errorWindows.begin() + i);
			i--;
		}
	}

#pragma endregion
	
	
	//imgui_addons::ImGuiFileBrowser file_dialog;
	
	//std::string path = "E:\\info\\C++\\asmax\\main.cpp";
	//std::ifstream fin(path);
	//std::string source_code((std::istreambuf_iterator<char>(fin)), (std::istreambuf_iterator<char>()));
	//
	////std::cout << source_code.size() << "\n";
	//int line_number = 0;
	//source_code.insert(0, std::to_string(line_number) + "\t");
	//line_number++;
	//for (int i = 0; i < source_code.size(); i++)
	//{
	//	if (source_code[i] == '\n')
	//	{
	//		source_code.insert(i + 1, std::to_string(line_number) + "\t");
	//		line_number++;
	//	}
	//
	//	//std::cout << i << "\n";
	//}
	//
	//
	//static float wrap_width = 600.0f;
	////ImGui::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");
	//ImDrawList* draw_list = ImGui::GetWindowDrawList();
	//
	//ImGui::Text("Continut fisier:");
	//ImGui::Spacing();
	//ImVec2 pos = ImGui::GetCursorScreenPos();
	//ImVec2 marker_min = ImVec2(pos.x + wrap_width, pos.y);
	//ImVec2 marker_max = ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight());
	//ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
	//ImGui::Text(source_code.c_str(), wrap_width);
	//
	//// Draw actual text bounding box, following by marker of our expected limit (should not overlap!)
	//draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
	////draw_list->AddRectFilled(marker_min, marker_max, IM_COL32(255, 0, 255, 255));
	//ImGui::PopTextWrapPos();


#pragma region set finishing stuff

	return true;
#pragma endregion

}

void closeGame()
{

	platform::writeEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

}
