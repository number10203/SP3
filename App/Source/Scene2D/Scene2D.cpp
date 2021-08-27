#include "Scene2D.h"
#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

#include "System\filesystem.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CScene2D::CScene2D(void)
	: cMap2D(NULL)
	, cPlayer2D(NULL)
	, cKeyboardController(NULL)
	, cGUI_Scene2D(NULL)
	, cGameManager(NULL)
	, cSoundController(NULL)
{
}

/**
 @brief Destructor
 */
CScene2D::~CScene2D(void)
{
	if (cSoundController)
	{
		// We won't delete this since it was created elsewhere
		cSoundController = NULL;
	}

	if (cGameManager)
	{
		cGameManager->Destroy();
		cGameManager = NULL;
	}

	if (cGUI_Scene2D)
	{
		cGUI_Scene2D->Destroy();
		cGUI_Scene2D = NULL;
	}

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// Destroy the enemies
	for (int i = 0; i < enemyVector.size(); i++)
	{
		delete enemyVector[i];
		enemyVector[i] = NULL;
	}
	enemyVector.clear();

	if (cPlayer2D)
	{
		cPlayer2D->Destroy();
		cPlayer2D = NULL;
	}

	if (cMap2D)
	{
		cMap2D->Destroy();
		cMap2D = NULL;
	}

	// Clear out all the shaders
	//CShaderManager::GetInstance()->Destroy();
}

/**
@brief Init Initialise this instance
*/ 
bool CScene2D::Init(int level)
{
	// Include Shader Manager
	//CShaderManager::GetInstance()->Add("2DShader", "Shader//Scene2D.vs", "Shader//Scene2D.fs");
	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	// Create and initialise the Map 2D
	cMap2D = CMap2D::GetInstance();
	// Set a shader to this class
	cMap2D->SetShader("2DShader");
	// Initialise the instance

	// Game Manager
	cGameManager = CGameManager::GetInstance();
	cGameManager->Init();

	if (cMap2D->Init(4, 24, 32) == false)
	{
		cout << "Failed to load CMap2D" << endl;
		return false;
	}
	// Load the map into an array
	if (0 - level >= 0) {
		if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01.csv", 0 - level) == false)
		{
			cout << " The loading of a map has failed 1." << endl;
			return false;
		}
	}
	
	// Load the map into an array
	if (1 - level >= 0) {
		if (cMap2D->LoadMap("Maps/DM2213_Map_Level_02.csv", 1 - level) == false)
		{
			cout << " The loading of a map has failed 2." << endl;
			return false;
		}
	}

	if (2 - level >= 0) {
		if (cMap2D->LoadMap("Maps/DM2213_Map_Level_03.csv", 2 - level) == false)
		{
			cout << " The loading of a map has failed 3." << endl;
			return false;
		}
	}

	if (3 - level >= 0) {
		if (cMap2D->LoadMap("Maps/DM2213_Map_Level_04.csv", 3 - level) == false)
		{
			cout << " The loading of a map has failed 4." << endl;
			return false;
		}
	}
// RECHECK THIS FILE FOR extra characters

	// Activate diagonal movement
	cMap2D->SetDiagonalMovement(false);

	// Load Scene2DColor into ShaderManager
	CShaderManager::GetInstance()->Add("2DColorShader", "Shader//Scene2DColor.vs", "Shader//Scene2DColor.fs");
	CShaderManager::GetInstance()->Use("2DColorShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
	// Create and initialise the CPlayer2D
	cPlayer2D = CPlayer2D::GetInstance();
	// Pass shader to cPlayer2D
	cPlayer2D->SetShader("2DColorShader");
	// Initialise the instance
	if (cPlayer2D->Init() == false)
	{
		cout << "Failed to load CPlayer2D" << endl;
		return false;
	}

	// Create and initialise the CEnemy2D
	enemyVector.clear();
	while (true)
	{
		CEnemy2D3* cEnemy2D3 = new CEnemy2D3();
		CEnemy2D2* cEnemy2D2 = new CEnemy2D2();
		CEnemy2D* cEnemy2D = new CEnemy2D();
		// Pass shader to cEnemy2D
		cEnemy2D3->SetShader("2DColorShader");
		cEnemy2D2->SetShader("2DColorShader");
		cEnemy2D->SetShader("2DColorShader");
		// Initialise the instance
		if (cEnemy2D->Init() == true)
		{
			cEnemy2D->SetPlayer2D(cPlayer2D);
			enemyVector.push_back(cEnemy2D);
		}
		else if (cEnemy2D2->Init() == true)
		{
			cEnemy2D2->SetPlayer2D(cPlayer2D);
			enemyVector.push_back(cEnemy2D2);
		}
		else if (cEnemy2D3->Init() == true)
		{
			cEnemy2D3->SetPlayer2D(cPlayer2D);
			enemyVector.push_back(cEnemy2D3);
		}
		else
		{
			// Break out of this loop if the enemy has all been loaded
			break;
		}
	}

	// Setup the shaders
	CShaderManager::GetInstance()->Add("textShader", "Shader//text.vs", "Shader//text.fs");
	CShaderManager::GetInstance()->Use("textShader");

	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();

	// Store the cGUI_Scene2D singleton instance here
	cGUI_Scene2D = CGUI_Scene2D::GetInstance();
	cGUI_Scene2D->Init();

	

	// Load the sounds into CSoundController
	cSoundController = CSoundController::GetInstance();
	cSoundController->Init();
	// SOUND PLAYLIST
	//BACKGROUND MUSIC 1 - 10
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Background\\BackGroundMusic1.ogg"), 1, true, true);
	// MENU SOUNDS 10 - 20
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\MenuSounds\\LoseSound.ogg"), 10, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\MenuSounds\\WinGameSound.ogg"), 11, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\MenuSounds\\LevelCompleteSound.ogg"), 12, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\MenuSounds\\Test.ogg"), 13, true);
	// HOME SOUND 20 - 30
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Home\\HomeAlertSound.ogg"), 20, true);
	// MEDIEVAL SOUND 30 - 40
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Medieval\\MedievalAlertSound.ogg"), 30, true);
	// CAVE SOUND 40 - 50
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Cave\\CaveAlertSound.ogg"), 40, true);
	// SKY 50-60
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sky\\SkyAlertSound.ogg"), 50, true);
	// PLAYER 60 - 70
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Player\\PlayerInjuredSound.ogg"), 60, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Player\\PlayerDeathSound.ogg"), 61, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Player\\PlayerCityWalkSound.ogg"), 62, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Player\\PlayerCaveWalkSound.ogg"), 63, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Player\\PlayerMediWalkSound.ogg"), 64, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Player\\PlayerSkyWalkSound.ogg"), 65, true);
	// COLLECTIBLES 70 - 90 
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Collect\\UnlockDoorSound.ogg"), 70, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Collect\\HealSound.ogg"), 71, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Collect\\ButtonTimer1Min.ogg"), 72, true);
	// ENEMIES 90 - 100
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Enemy\\GuardianStabSound.ogg"), 90, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Enemy\\SentryDetectSound.ogg"), 91, true);
	
	return true;
}

/**
@brief Update Update this instance
*/
bool CScene2D::Update(const double dElapsedTime)
{

	// Call the cPlayer2D's update method before Map2D as we want to capture the inputs before map2D update
  	cPlayer2D->Update(dElapsedTime);

	// Call all the cEnemy2D's update method before Map2D 
	// as we want to capture the updates before map2D update
	for (int i = 0; i < enemyVector.size(); i++)
	{
		enemyVector[i]->Update(dElapsedTime);
	}
	 
	// Call the Map2D's update method
	cMap2D->Update(dElapsedTime);

	// Get keyboard updates
	if (cKeyboardController->IsKeyDown(GLFW_KEY_F6))
	{
		// Save the current game to a save file
		// Make sure the file is open
		try {
			if (cMap2D->SaveMap("Maps/DM2213_Map_Level_01_SAVEGAMEtest.csv") == false)
			{
				throw runtime_error("Unable to save the current game to a file");
			}
		}
		catch (runtime_error e)
		{
			cout << "Runtime error: " << e.what();
			return false;
		}
	}

	// Call the cGUI_Scene2D's update method
	cGUI_Scene2D->Update(dElapsedTime);

	// Check if the game should go to the next level
	if (cGameManager->bLevelCompleted == true)
	{
		CGameStateManager::GetInstance()->LevelNow++;
		cSoundController->PlaySoundByID(12);
		cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() + 1);
		cout << "Next Level" << endl;
		cPlayer2D->Reset();
		enemyVector.clear();
		while (true)
		{
			CEnemy2D3* cEnemy2D3 = new CEnemy2D3();
			CEnemy2D2* cEnemy2D2 = new CEnemy2D2();
			CEnemy2D* cEnemy2D = new CEnemy2D();
			// Pass shader to cEnemy2D
			cEnemy2D3->SetShader("2DColorShader");
			cEnemy2D2->SetShader("2DColorShader");
			cEnemy2D->SetShader("2DColorShader");
			// Initialise the instance
			if (cEnemy2D->Init() == true)
			{
				cEnemy2D->SetPlayer2D(cPlayer2D);
				enemyVector.push_back(cEnemy2D);
			}
			else if (cEnemy2D2->Init() == true)
			{
				cEnemy2D2->SetPlayer2D(cPlayer2D);
				enemyVector.push_back(cEnemy2D2);
			}
			else if (cEnemy2D3->Init() == true)
			{
				cEnemy2D3->SetPlayer2D(cPlayer2D);
				enemyVector.push_back(cEnemy2D3);
			}
			else
			{
				// Break out of this loop if the enemy has all been loaded
				break;
			}
		}
		cGameManager->bLevelCompleted = false;
	}
	
	// Check if the game has been won by the player
	if (cGameManager->bPlayerWon == true)
	{
		// End the game and switch to Win screen
		CGameStateManager::GetInstance()->SetActiveGameState("WinState");
		cSoundController->StopAllSounds();
		cSoundController->PlaySoundByID(11);
	}
	// Check if the game should be ended
	else if (cGameManager->bPlayerLost == true)
	{
		CGameStateManager::GetInstance()->SetActiveGameState("LoseState");
		cSoundController->StopAllSounds();
		cSoundController->PlaySoundByID(10);
		return false;
	}

	if (cPlayer2D->ButtonTimer > 0 && enemyRespawned == false) {
		enemyRespawned = true;
		cout << "enemyrespawned";
		enemyVector.clear();
		while (true)
		{
			CEnemy2D3* cEnemy2D3 = new CEnemy2D3();
			CEnemy2D2* cEnemy2D2 = new CEnemy2D2();
			CEnemy2D* cEnemy2D = new CEnemy2D();
			// Pass shader to cEnemy2D
			cEnemy2D3->SetShader("2DColorShader");
			cEnemy2D2->SetShader("2DColorShader");
			cEnemy2D->SetShader("2DColorShader");
			// Initialise the instance
			if (cEnemy2D->Init() == true)
			{
				cEnemy2D->SetPlayer2D(cPlayer2D);
				enemyVector.push_back(cEnemy2D);
			}
			else if (cEnemy2D2->Init() == true)
			{
				cEnemy2D2->SetPlayer2D(cPlayer2D);
				enemyVector.push_back(cEnemy2D2);
			}
			else if (cEnemy2D3->Init() == true)
			{
				cEnemy2D3->SetPlayer2D(cPlayer2D);
				enemyVector.push_back(cEnemy2D3);
			}
			else
			{
				// Break out of this loop if the enemy has all been loaded
				break;
			}
			for (int i = 0; i < enemyVector.size(); i++)
			{
				// Call the CEnemy2D's PreRender()
				enemyVector[i]->PreRender();
				// Call the CEnemy2D's Render()
				enemyVector[i]->Render();
				// Call the CEnemy2D's PostRender()
				enemyVector[i]->PostRender();
			}
		}
	}
	else if (cPlayer2D->ButtonTimer <= 0 && enemyRespawned == true)
		enemyRespawned = false;
	cSoundController->PlaySoundByID(1);

	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CScene2D::PreRender(void)
{
	// Reset the OpenGL rendering environment
	glLoadIdentity();

	// Clear the screen and buffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Enable 2D texture rendering
	glEnable(GL_TEXTURE_2D);
}

/**
 @brief Render Render this instance
 */
void CScene2D::Render(void)
{
	for (int i = 0; i < enemyVector.size(); i++)
	{
		// Call the CEnemy2D's PreRender()
		enemyVector[i]->PreRender();
		// Call the CEnemy2D's Render()
		enemyVector[i]->Render();
		// Call the CEnemy2D's PostRender()
		enemyVector[i]->PostRender();
	}

	
	
	// Call the Map2D's PreRender()
	cMap2D->PreRender();
	// Call the Map2D's Render()
	cMap2D->Render();
	// Call the Map2D's PostRender()
	cMap2D->PostRender();


	for (int i = 0; i < enemyVector.size(); i++)
	{
		// Call the CEnemy2D's PreRender()
		enemyVector[i]->PreRender();
		// Call the CEnemy2D's Render()
		enemyVector[i]->Render();
		// Call the CEnemy2D's PostRender()
		enemyVector[i]->PostRender();
	}

	// Call the cGUI_Scene2D's PreRender()
	cGUI_Scene2D->PreRender();
	// Call the cGUI_Scene2D's Render()
	cGUI_Scene2D->Render();
	// Call the cGUI_Scene2D's PostRender()
	cGUI_Scene2D->PostRender();

	// Call the CPlayer2D's PreRender()
	cPlayer2D->PreRender();
	// Call the CPlayer2D's Render()
	cPlayer2D->Render();
	// Call the CPlayer2D's PostRender()
	cPlayer2D->PostRender();

}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CScene2D::PostRender(void)
{
}