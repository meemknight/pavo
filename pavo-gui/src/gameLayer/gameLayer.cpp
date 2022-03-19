#include "gameLayer.h"
#include "gl2d/gl2d.h"
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imguiThemes.h"
#include "genericType.h"

gl2d::Renderer2D renderer;

gl2d::Font font;
gl2d::Texture texture;

struct GameData
{
	float posx=100;
	float posy=100;

	int test = 0;

}gameData;


bool initGame()
{

	imguiThemes::yellow();

	renderer.create();
	font.createFromFile(RESOURCES_PATH "roboto_black.ttf");
	texture.loadFromFile(RESOURCES_PATH "test.jpg");

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
	
	renderer.updateWindowMetrics(w, h);
	renderer.clearScreen();
#pragma endregion


#pragma region input
	float speed = 400 * deltaTime;

	if(platform::isKeyHeld(platform::Button::Up) 
		|| platform::getControllerButtons().buttons[platform::ControllerButtons::Up].held
		)
	{
		gameData.posy -= speed;
	}
	if (platform::isKeyHeld(platform::Button::Down)
		|| platform::getControllerButtons().buttons[platform::ControllerButtons::Down].held
		)
	{
		gameData.posy += speed;
	}
	if (platform::isKeyHeld(platform::Button::Left)
		|| platform::getControllerButtons().buttons[platform::ControllerButtons::Left].held
		)
	{
		gameData.posx -= speed;
	}
	if (platform::isKeyHeld(platform::Button::Right)
		|| platform::getControllerButtons().buttons[platform::ControllerButtons::Right].held
		)
	{
		gameData.posx += speed;
	}

	if (platform::isKeyTyped(platform::Button::NR1)
		)
	{
		gameData.test -= 1;
	}
	if (platform::isKeyTyped(platform::Button::NR2)
		)
	{
		gameData.test += 1;
	}


	if (platform::isKeyPressedOn(platform::Button::F) && platform::isKeyHeld(platform::Button::LeftCtrl))
	{
		platform::setFullScreen(!platform::isFullScreen());
	}
#pragma endregion

	
	
	//ImGui::ShowDemoWindow();

	ImGui::Begin("Fereastra1");
	static GenericType reg = {};
	genericTypeInput<__COUNTER__>(reg);

	ImGui::End();





	//std::cout << platform::getTypedInput();

#pragma region set finishing stuff
	renderer.flush();

	return true;
#pragma endregion

}

void closeGame()
{

	platform::writeEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

}
