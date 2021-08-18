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

#include "SoundState.h"

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

// Include CSoundController
#include "../SoundController/SoundController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CSoundState::CSoundState(void)
	: background(NULL)
	
{
}

/**
 @brief Destructor
 */
CSoundState::~CSoundState(void)
{
	
}

/**
 @brief Init this class instance
 */
bool CSoundState::Init(void)
{
	cout << "CSoundState::Init()\n" << endl;

	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	//Create Background Entity
	background = new CBackgroundEntity("Image/Menus/SoundMenu.png");
	background->SetShader("2DShader");
	background->Init();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(CSettings::GetInstance()->pWindow, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Load the images for buttons
	CImageLoader* il = CImageLoader::GetInstance();
	
	// Goback button
	backButtonData.fileName = "Image\\GUI\\RetryButton.png";
	backButtonData.textureID = il->LoadTextureGetID(backButtonData.fileName.c_str(), false);
	// Sound Up Button
	VolUpButtonData.fileName = "Image\\GUI\\VolumeUpButton.png";
	VolUpButtonData.textureID = il->LoadTextureGetID(VolUpButtonData.fileName.c_str(), false);
	// Sound Down Button
	VolDownButtonData.fileName = "Image\\GUI\\VolumeDownButton.png";
	VolDownButtonData.textureID = il->LoadTextureGetID(VolDownButtonData.fileName.c_str(), false);

	
	return true;
}

/**
 @brief Update this class instance
 */
bool CSoundState::Update(const double dElapsedTime)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

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
			CSettings::GetInstance()->iWindowHeight/3.0));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

		//Added rounding for nicer effect
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;
		
		// Add codes for Vol Up button here
		if (ImGui::ImageButton((ImTextureID)VolUpButtonData.textureID, 
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0), int(-1), ImVec4(0.5, 0, 0.5, 1), ImVec4(1, 1, 1, 1)))
		{
			// Reset the CKeyboardController
			CKeyboardController::GetInstance()->Reset();
			CSoundController::GetInstance()->MasterVolumeIncrease();
			CSoundController::GetInstance()->PlaySoundByID(4);

		}

		// Add codes for Vol Down button here
		if (ImGui::ImageButton((ImTextureID)VolDownButtonData.textureID,
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0), int(-1), ImVec4(0.5, 0, 0.5, 1), ImVec4(1,1,1,1)))
		{
			// Reset the CKeyboardController
			CKeyboardController::GetInstance()->Reset();
			CSoundController::GetInstance()->MasterVolumeDecrease();
			CSoundController::GetInstance()->PlaySoundByID(4);

		}

		// Add codes for back button here
		if (ImGui::ImageButton((ImTextureID)backButtonData.textureID,
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0), int(-1), ImVec4(0.5, 0, 0.5, 1), ImVec4(1, 1, 1, 1)))
		{
			// Reset the CKeyboardController
			CKeyboardController::GetInstance()->Reset();

			// Load the Instruction state
			cout << "Switching to play State" << endl;
			CGameStateManager::GetInstance()->SetActiveGameState("MenuState");

		}
		ImGui::End();
	}



	//For keyboard controls
	
	return true;
}

/**
 @brief Render this class instance
 */
void CSoundState::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Render Background
	background->Render();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	//cout << "CMenuState::Render()\n" << endl;
}

/**
 @brief Destroy this class instance
 */
void CSoundState::Destroy(void)
{
	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	cout << "CMenuState::Destroy()\n" << endl;
}
