// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "PauseState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include Mesh Builder
#include "Primitives/MeshBuilder.h"
// Include ImageLoader
#include "System\ImageLoader.h"
// Include Shader Manager
#include "RenderControl\ShaderManager.h"

 // Include shader
#include "RenderControl\shader.h"

// Include CSettings
#include "GameControl/Settings.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

// Include SoundController
#include "../SoundController/SoundController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CPauseState::CPauseState(void)
	//: background(NULL)
{
	
}

/**
 @brief Destructor
 */
CPauseState::~CPauseState(void)
{

}

/**
 @brief Init this class instance
 */
bool CPauseState::Init(void)
{
	cout << "CPauseState::Init()\n" << endl;

	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);


	// Load the images for buttons
	CImageLoader* il = CImageLoader::GetInstance();
	VolumeIncreaseButtonData.fileName = "Image\\GUI\\VolumeUpButton.png";
	VolumeIncreaseButtonData.textureID = il->LoadTextureGetID(VolumeIncreaseButtonData.fileName.c_str(), false);
	VolumeDecreaseButtonData.fileName = "Image\\GUI\\VolumeDownButton.png";
	VolumeDecreaseButtonData.textureID = il->LoadTextureGetID(VolumeDecreaseButtonData.fileName.c_str(), false);
	ExitButtonData.fileName = "Image\\GUI\\ExitButton.png";
	ExitButtonData.textureID = il->LoadTextureGetID(ExitButtonData.fileName.c_str(), false);
	

	return true;
}

/**
 @brief Update this class instance
 */
bool CPauseState::Update(const double dElapsedTime)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;

	float buttonWidth = 256;
	float buttonHeight = 128;

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		// Create a window called "Hello, world!" and append into it.
		ImGui::Begin("Main Menu", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth/2.0 - buttonWidth/2.0, 
			CSettings::GetInstance()->iWindowHeight/6.0));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

		//Added rounding for nicer effect
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;
		
		// Display words
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "In-game Menu");

		// Add codes for Up button here
		if (ImGui::ImageButton((ImTextureID)VolumeIncreaseButtonData.textureID, 
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0),int(-1), ImVec4(0.5, 0, 0.5, 1), ImVec4(1, 1, 1, 1)))
		{
			// Reset the CKeyboardController
			CKeyboardController::GetInstance()->Reset();

			CSoundController::GetInstance()->MasterVolumeIncrease();
			CSoundController::GetInstance()->PlaySoundByID(23);
		

		}
		// Add codes for Down button here
		if (ImGui::ImageButton((ImTextureID)VolumeDecreaseButtonData.textureID,
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0), int(-1), ImVec4(0.5, 0, 0.5, 1), ImVec4(1, 1, 1, 1)))
		{
			// Reset the CKeyboardController
			CKeyboardController::GetInstance()->Reset();

			CSoundController::GetInstance()->MasterVolumeDecrease();
			CSoundController::GetInstance()->PlaySoundByID(23);
		
		}
		// Add codes for Exit button here
		if (ImGui::ImageButton((ImTextureID)ExitButtonData.textureID,
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0), int(-1), ImVec4(0.5, 0, 0.5, 1), ImVec4(1, 1, 1, 1)))
		{
			// Reset the CKeyboardController
			CKeyboardController::GetInstance()->Reset();

			// Load the menu state
			cout << "Quitting the game" << endl;

			return false;

		}
	ImGui::End();
	}
	

	//For keyboard controls
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_F10))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

	

		// Load the menu state
		cout << "UnLoading PauseState" << endl;
		CGameStateManager::GetInstance()->SetPauseGameState(nullptr);
		return true;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CPauseState::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);


	//cout << "CPauseState::Render()\n" << endl;
}

/**
 @brief Destroy this class instance
 */
void CPauseState::Destroy(void)
{
	//cout << "CPauseState::Destroy()\n" << endl;
}
