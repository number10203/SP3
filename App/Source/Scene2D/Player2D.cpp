﻿/**
 Player2D
 By: Toh Da Jun
 Date: Mar 140
 */
#include "Player2D.h"

#include "Projectile2D.h"

#include <iostream>
using namespace std;

bool DeGrapple = false;

enum Dimensions
{
	HOME = 0,
	MEDI = 1,
	CAVE = 2,
	SKY = 3,
	DIMEN_NUM,
};

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
#include "Primitives/MeshBuilder.h"

// Include Game Manager
#include "GameManager.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::CPlayer2D(void)
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
	, cSoundController(NULL)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	i32vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	i32vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	//Phasing 
	PhaseWalking = false;
	CooldownTimer = 0;
	//Death
	DeathTimer = 0;
	count = 0;
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::~CPlayer2D(void)
{
	// We won't delete this since it was created elsewhere
	cSoundController = NULL;

	// We won't delete this since it was created elsewhere
	cInventoryManager = NULL;

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

/**
  @brief Initialise this instance
  */
bool CPlayer2D::Init(void)
{
	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(101, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	// Load the player texture
	if (LoadTexture("Image/Player/Player.png", iTextureID) == false)
	{
		std::cout << "Failed to load player tile texture" << std::endl;
		return false;
	}
	
	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(15, 3, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idle", 0, 3);
	animatedSprites->AddAnimation("right", 3, 6);
	animatedSprites->AddAnimation("death", 6, 9);
	animatedSprites->AddAnimation("skyidle", 9, 12);
	animatedSprites->AddAnimation("skyleft", 12, 15);
	animatedSprites->AddAnimation("skyright", 15, 18);
	animatedSprites->AddAnimation("left", 18, 21);
	animatedSprites->AddAnimation("phaseleft", 21, 24);
	animatedSprites->AddAnimation("phaseright", 24, 27);
	animatedSprites->AddAnimation("phaseidle", 27, 30);
	animatedSprites->AddAnimation("shootright", 30, 33);
	animatedSprites->AddAnimation("grappleright", 33, 36);
	animatedSprites->AddAnimation("grappleleft", 36, 39);
	animatedSprites->AddAnimation("shootleft", 39, 42);
	animatedSprites->AddAnimation("shootup", 42, 45);
	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	// Get the handler to the CInventoryManager instance
	cInventoryManager = CInventoryManager::GetInstance();
	// Add a Lives icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Lives", "Image/Scene2D_Lives.tga", 3, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Health icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Health", "Image/HUD/Health.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a DImension icon as one of the inventory items to showcase the current dimension (Medieval)
	cInventoryItem = cInventoryManager->Add("DimensionMedeival", "Image/HUD/Medieval.tga", 100, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a DImension icon as one of the inventory items to showcase the current dimension (Home)
	cInventoryItem = cInventoryManager->Add("DimensionHome", "Image/HUD/Home.tga", 100, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a DImension icon as one of the inventory items to showcase the current dimension (Ascend)
	cInventoryItem = cInventoryManager->Add("DimensionSky", "Image/HUD/Ascend.tga", 100, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a DImension icon as one of the inventory items to showcase the current dimension (cave)
	cInventoryItem = cInventoryManager->Add("DimensionCave", "Image/HUD/Cave.tga", 100, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);



	jumpCount = 0;

	// Get the handler to the CSoundController
	cSoundController = CSoundController::GetInstance();


	return true;
}

/**
 @brief Reset this instance
 */
bool CPlayer2D::Reset()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(101, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	//Set it to fall upon entering new level
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	//CS: Reset double jump
	jumpCount = 0;

	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	//CS: Init the color to white
	//currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	return true;
}

/**
 @brief Update this instance
 */


void CPlayer2D::Update(const double dElapsedTime)
{
	if (CGameManager::GetInstance()->bLevelPaused == false)
	{
		// Store the old position
		i32vec2OldIndex = i32vec2Index;

		// IDLE ANIMATIONS SWITCH
		if (CGameManager::GetInstance()->currDimem == CAVE)
		{
			animatedSprites->PlayAnimation("phaseidle", -1, 1.0f);
		}
		else if (CGameManager::GetInstance()->currDimem == HOME || CGameManager::GetInstance()->currDimem == MEDI)
		{
			animatedSprites->PlayAnimation("idle", -1, 1.0f);
		}
		else if (CGameManager::GetInstance()->currDimem == SKY)
		{
			animatedSprites->PlayAnimation("skyidle", -1, 1.0f);
		}

		if (CGameManager::GetInstance()->bPlayerDeath == false)
		{
			// Get keyboard updates
			if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
			{
				// Calculate the new position to the left
				if (i32vec2Index.x >= 0)
				{
					i32vec2NumMicroSteps.x--;
					if (i32vec2NumMicroSteps.x < 0)
					{
						i32vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
						i32vec2Index.x--;
					}
				}

				// Constraint the player's position within the screen boundary
				Constraint(LEFT);

				// If the new position is not feasible, then revert to old position
				if (CheckPosition(LEFT) == false)
				{
					i32vec2Index = i32vec2OldIndex;
					i32vec2NumMicroSteps.x = 0;
				}

				//// Check if player is in mid-air, such as walking off a platform
				//if (IsMidAir() == true)
				//{
				//	if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
				//		cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				//}

				//CS: Play the "left" animation		
				if (CGameManager::GetInstance()->currDimem == CAVE)
				{
					animatedSprites->PlayAnimation("phaseleft", -1, 1.0f);
					cSoundController->PlaySoundByID(63);
				}
				else if (CGameManager::GetInstance()->currDimem == HOME)
				{
					animatedSprites->PlayAnimation("left", -1, 1.0f);
					cSoundController->PlaySoundByID(62);
				}
				else if (CGameManager::GetInstance()->currDimem == MEDI)
				{
					animatedSprites->PlayAnimation("left", -1, 1.0f);
					cSoundController->PlaySoundByID(64);
				}
				else if (CGameManager::GetInstance()->currDimem == SKY)
				{
					animatedSprites->PlayAnimation("skyleft", -1, 1.0f);
					cSoundController->PlaySoundByID(65);
				}



				//CS: Change Color
				//currentColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
			}
			else if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
			{
				// Calculate the new position to the right
				if (i32vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
				{
					i32vec2NumMicroSteps.x++;

					if (i32vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
					{
						i32vec2NumMicroSteps.x = 0;
						i32vec2Index.x++;
					}
				}

				// Constraint the player's position within the screen boundary
				Constraint(RIGHT);

				// If the new position is not feasible, then revert to old position
				if (CheckPosition(RIGHT) == false)
				{
					i32vec2NumMicroSteps.x = 0;
				}

				//// Check if player is in mid-air, such as walking off a platform
				//if (IsMidAir() == true)
				//{
				//	if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
				//		cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				//}

				//CS: Play the "right" animation
				if (CGameManager::GetInstance()->currDimem == CAVE)
				{
					animatedSprites->PlayAnimation("phaseright", -1, 1.0f);
					cSoundController->PlaySoundByID(63);
				}
				else if (CGameManager::GetInstance()->currDimem == HOME)
				{
					animatedSprites->PlayAnimation("right", -1, 1.0f);
					cSoundController->PlaySoundByID(62);
				}
				else if (CGameManager::GetInstance()->currDimem == MEDI)
				{
					animatedSprites->PlayAnimation("right", -1, 1.0f);
					cSoundController->PlaySoundByID(64);
				}
				else if (CGameManager::GetInstance()->currDimem == SKY)
				{
					animatedSprites->PlayAnimation("skyright", -1, 1.0f);
					cSoundController->PlaySoundByID(65);
				}

				//CS: Change Color
				//currentColor = glm::vec4(1.0, 1.0, 0.0, 1.0);
			}
			//if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
			//{
			//	// Calculate the new position up
			//	if (i32vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
			//	{
			//		i32vec2NumMicroSteps.y++;
			//		if (i32vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
			//		{
			//			i32vec2NumMicroSteps.y = 0;
			//			i32vec2Index.y++;
			//		}
			//	}

			//	// Constraint the player's position within the screen boundary
			//	Constraint(UP);

			//	// If the new position is not feasible, then revert to old position
			//	if (CheckPosition(UP) == false)
			//	{
			//		i32vec2NumMicroSteps.y = 0;
			//	}

			//	//CS: Play the "idle" animation
			//	animatedSprites->PlayAnimation("idle", -1, 1.0f);

			//	//CS: Change Color
			//	//currentColor = glm::vec4(0.0, 1.0, 1.0, 0.5);
			//}
			//else if (cKeyboardController->IsKeyDown(GLFW_KEY_S))
			//{
			//	// Calculate the new position down
			//	if (i32vec2Index.y >= 0)
			//	{
			//		i32vec2NumMicroSteps.y--;
			//		if (i32vec2NumMicroSteps.y < 0)
			//		{
			//			i32vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
			//			i32vec2Index.y--;
			//		}
			//	}

			//	// Constraint the player's position within the screen boundary
			//	Constraint(DOWN);

			//	// If the new position is not feasible, then revert to old position
			//	if (CheckPosition(DOWN) == false)
			//	{
			//		i32vec2Index = i32vec2OldIndex;
			//		i32vec2NumMicroSteps.y = 0;
			//	}

			//	//CS: Play the "idle" animation
			//	animatedSprites->PlayAnimation("idle", -1, 1.0f);

			//	//CS: Change Color
			//	//currentColor = glm::vec4(1.0, 0.0, 1.0, 0.5);
			//}
			//if (cKeyboardController->IsKeyPressed(GLFW_KEY_C))
			//{
			//	cout << "Creative Mode" << endl;

			//	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE)
			//	{
			//		cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
			//		cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 3.5f));
			//		jumpCount += 1;
			//		// Play a jump sound
			//		cSoundController->PlaySoundByID(3);
			//	}
			//	else
			//	{
			//		if (jumpCount < 2)
			//		{
			//			cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
			//			cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 1.5f));
			//			jumpCount += 1;
			//			// Play a jump sound
			//			cSoundController->PlaySoundByID(3);
			//		}
			//	}
			//}
			//debug code
			//if (cKeyboardController->IsKeyPressed(GLFW_KEY_L))
			//{
			//	CGameManager::GetInstance()->bLevelCompleted = true;
			//}

			// DIMENSION SWAPPING

			if (CooldownTimer <= 0)
			{
				currentColor = glm::vec4(1.0, 1.0, 1.0, 0.5);

				if (cKeyboardController->IsKeyPressed(GLFW_KEY_U))
				{
					currentColor = glm::vec4(1.0, 0.0, 1.0, 0.5);

					CGameManager::GetInstance()->currDimem = HOME;
					/*CGameManager::GetInstance()->bPlayerHome = true;
					CGameManager::GetInstance()->bPlayerMedieval = false;
					CGameManager::GetInstance()->bPlayerCave = false;
					CGameManager::GetInstance()->bPlayerSky = false;*/

					cout << "Home Mode" << endl;

					cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
					CooldownTimer = 1;

					cout << "Cooldown Applied" << endl;
					cSoundController->PlaySoundByID(20);
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_I))
				{
					currentColor = glm::vec4(0.0, 1.0, 0.0, 0.5);

					CGameManager::GetInstance()->currDimem = MEDI;
					cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
					CooldownTimer = 1;
					cSoundController->PlaySoundByID(30);
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_O))
				{
					currentColor = glm::vec4(0.2, 0.2, 0.2, 0.5);

					CGameManager::GetInstance()->currDimem = CAVE;
					cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
					CooldownTimer = 1;
					cSoundController->PlaySoundByID(40);
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_P))
				{
					currentColor = glm::vec4(0.2, 0.6, 0.0, 0.5);

					CGameManager::GetInstance()->currDimem = SKY;
					cPhysics2D.SetStatus(CPhysics2D::STATUS::RISE);
					CooldownTimer = 100;
					cSoundController->PlaySoundByID(50);
				}
			}
			else if (CooldownTimer > 0)
			{
				CooldownTimer -= 1 * dElapsedTime;
			}
			if (ButtonTimer > 0)
			{
				ButtonTimer -= 1 * dElapsedTime;
				if (ButtonTimer <= 0) {
					for (int x = 0; x < 32; x++) {
						for (int y = 0; y < 24; y++) {
							if (cMap2D->GetMapInfo(y, x) == 206) {
								cMap2D->SetMapInfo(y, x, 205);
							}
							if (cMap2D->GetMapInfo(y, x) == 207) {
								cMap2D->SetMapInfo(y, x, 20);
							}
						}
					}
				}
			}
			switch (CGameManager::GetInstance()->currDimem)
			{
			case HOME:

				PhaseWalking = false;

				if (cKeyboardController->IsKeyPressed(GLFW_KEY_LEFT))
				{
					ability_dir = ALEFT;
					//cout << "Gun Left" << endl;
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_RIGHT))
				{
					ability_dir = ARIGHT;
					//cout << "Gun Right" << endl;
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_UP))
				{
					ability_dir = AUP;
					//cout << "Gun Up" << endl;
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_DOWN))
				{
					ability_dir = ADOWN;
					//cout << "Gun Down" << endl;
				}


				//if (cKeyboardController->IsKeyPressed(GLFW_KEY_SPACE))
				//{
				//	if (ability_dir == ARIGHT)
				//	{
				//		if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) == 0)
				//		{
				//			cout << "Shooting Right" << endl;
				//			cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 21);
				//			Shoot = true;
				//			//CProjectile2D* aProjectile2D = new CProjectile2D();
				//			//aProjectile2D->SetShader("2DColorShader");
				//			//aProjectile2D->Seti32vec2Index(i32vec2Index.y, i32vec2Index.x + 1);
				//		} 
				//	}
				//	else if (ability_dir == ALEFT)
				//	{
				//		if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x - 1) == 0)
				//		{
				//			cout << "Shooting Left" << endl;
				//			//cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x - 1, 104);
				//		}
				//	}
				//	else if (ability_dir == AUP)
				//	{
				//		if (cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) == 0)
				//		{
				//			cout << "Shooting Up" << endl;
				//			//cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x, 104);
				//		}
				//	}
				//	else if (ability_dir == ADOWN)
				//	{
				//		if (cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x - 1) == 0)
				//		{
				//			cout << "Shooting Down" << endl;
				//			//cMap2D->SetMapInfo(i32vec2Index.y - 1, i32vec2Index.x, 104);
				//		}
				//	}
				//}

				break;

			case MEDI:

				PhaseWalking = false;

				if (cKeyboardController->IsKeyPressed(GLFW_KEY_LEFT))
				{
					ability_dir = ALEFT;
					cSoundController->PlaySoundByID(68);
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_RIGHT))
				{
					ability_dir = ARIGHT;
					cSoundController->PlaySoundByID(69);
				}

				if (cKeyboardController->IsKeyPressed(GLFW_KEY_SPACE))
				{
					if (ability_dir == ARIGHT)
					{
						//Move towards the hookblock
						cPhysics2D.SetStatus(CPhysics2D::STATUS::GRAPPLE_RIGHT);
						cSoundController->PlaySoundByID(66);
					}
					else if (ability_dir == ALEFT)
					{
						//Move towards the hookblock
						cPhysics2D.SetStatus(CPhysics2D::STATUS::GRAPPLE_LEFT);
						cSoundController->PlaySoundByID(66);
					}

					if (DeGrapple == true)
					{
						cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
						DeGrapple = !DeGrapple;
						cSoundController->PlaySoundByID(67);
					}
				}
				break;

			case CAVE:

				PhaseWalking = true;
				break;

			case SKY:

				PhaseWalking = false;
				break;

			default:
				PhaseWalking = false;
				break;
			}
		}
			// Update Death
			if (CGameManager::GetInstance()->bPlayerDeath == true)
			{
				animatedSprites->PlayAnimation("death", -1, 1.0f);
				DeathTimer += 1 * dElapsedTime;
				if (DeathTimer >= 1)
				{
					// Player loses, goes to lose screen
					CGameManager::GetInstance()->bPlayerLost = true;
					DeathTimer = 0;
					CGameManager::GetInstance()->bPlayerDeath = false;
				}
			}
		

		// Update Jump or Fall
		//CS: Will cause error when debugging. Set to default elapsed time
		UpdateJumpFall(dElapsedTime);

		// Interact with the Map
		InteractWithMap();

		// Update the Health and Lives
		UpdateHealthLives();

		//CS: Update the animated sprite
		animatedSprites->Update(dElapsedTime);

		if (CGameManager::GetInstance()->bPlayerTouched == true || 
			CGameManager::GetInstance()->bPlayerStabbed == true || 
			CGameManager::GetInstance()->bPlayerSpiked == true)
		{
			currentColor = glm::vec4(1.0, 0.0, 0.0, 0.5);
		}
		else
		{
			CGameManager::GetInstance()->bPlayerTouched = false;
			CGameManager::GetInstance()->bPlayerStabbed = false;
			CGameManager::GetInstance()->bPlayerSpiked = false;
			currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
		}
		

		// Update the UV Coordinates
		vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, i32vec2Index.x, false, i32vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
		vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, i32vec2Index.y, false, i32vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
	}
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CPlayer2D::PreRender(void)
{
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CPlayer2D::Render(void)
{
	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtime_color");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x,
													vec2UVCoordinate.y,
													0.0f));
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(currentColor));

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	//CS: Render the animated sprite
	animatedSprites->Render();

	glBindVertexArray(0);

}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CPlayer2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

/**
@brief Load a texture, assign it a code and store it in MapOfTextureIDs.
@param filename A const char* variable which contains the file name of the texture
*/
bool CPlayer2D::LoadTexture(const char* filename, GLuint& iTextureID)
{
	// Variables used in loading the texture
	int width, height, nrChannels;
	
	// texture 1
	// ---------
	glGenTextures(1, &iTextureID);
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	CImageLoader* cImageLoader = CImageLoader::GetInstance();
	unsigned char *data = cImageLoader->Load(filename, width, height, nrChannels, true);
	if (data)
	{
		if (nrChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		return false;
	}
	// Free up the memory of the file data read in
	free(data);

	return true;
}

/**
 @brief Constraint the player's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CPlayer2D::Constraint(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if (i32vec2Index.x < 0)
		{
			i32vec2Index.x = 0;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == RIGHT)
	{
		if (i32vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			i32vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS) - 1;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == UP)
	{
		if (i32vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			cInventoryItem = cInventoryManager->GetItem("Health");
			cInventoryItem->Remove(50);
			i32vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS) - 1;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else if (eDirection == DOWN)
	{
		if (i32vec2Index.y < 0)
		{
			cInventoryItem = cInventoryManager->GetItem("Health");
			cInventoryItem->Remove(50);
			i32vec2Index.y = 0;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else
	{
		cout << "CPlayer2D::Constraint: Unknown direction." << endl;
	}
}

/**
 @brief Check if a position is possible to move into
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
bool CPlayer2D::CheckPosition(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		// If the new position is fully within a row, then check this row only
		if (i32vec2NumMicroSteps.y == 0)
		{
			if (PhaseWalking == true)
			{
				if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) == 19)
				{
					return true;
				}
			}

			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) > 1 && cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x)<=100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (i32vec2NumMicroSteps.y != 0)
		{
			if (PhaseWalking == true)
			{
				if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) == 19) &&
					(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) == 19))
				{
					return true;
				}
			}

			// If the 2 grids are not accessible, then return false
			if (((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) > 1)
				&&((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) <= 100)))
				|| ((cMap2D->GetMapInfo(i32vec2Index.y+1, i32vec2Index.x) > 1)&&
				(cMap2D->GetMapInfo(i32vec2Index.y+1, i32vec2Index.x) <= 100)))
			{
				return false;
			}
		}
	}
	else if (eDirection == RIGHT)
	{
		// If the new position is at the top row, then return true
		if (i32vec2Index.x >= cSettings->NUM_TILES_XAXIS - 1)
		{
			i32vec2NumMicroSteps.x = 0;
			return true;
		}

		// If the new position is fully within a row, then check this row only
		if (i32vec2NumMicroSteps.y == 0)
		{

			if (PhaseWalking == true)
			{
				if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) == 19)
				{
					return true;
				}
			}
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) > 1 && cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) <= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (i32vec2NumMicroSteps.y != 0)
		{

			if (PhaseWalking == true)
			{
				if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) == 19) &&
					(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) == 19))
				{
					return true;
				}
			}

			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) >1)&& (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) <= 100) ||
				(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) >1)&& (cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) <= 100))
			{
				return false;
			}
		}

	}
	else if (eDirection == UP)
	{
		// If the new position is at the top row, then return true
		if (i32vec2Index.y >= cSettings->NUM_TILES_YAXIS - 1)
		{
			i32vec2NumMicroSteps.y = 0;
			return true;
		}

		// If the new position is fully within a column, then check this column only
		if (i32vec2NumMicroSteps.x == 0)
		{

			if (PhaseWalking == true)
			{
				if (cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) == 19)
				{
					return true;
				}
			}

			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) > 1 && cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) <= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (i32vec2NumMicroSteps.x != 0)
		{
			if (PhaseWalking == true)
			{
				if ((cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) ==19) &&
					(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) == 19))
				{
					return true;
				}
			}

			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) > 1)&& (cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) <= 100) ||
				(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) > 1)&& (cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) <= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == DOWN)
	{
		// If the new position is fully within a column, then check this column only
		if (i32vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false

			if (PhaseWalking == true)
			{
				if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) == 19 || cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) == 0)
				{
					return true;
				}
			}

			if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) > 1 && cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) <= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (i32vec2NumMicroSteps.x != 0)
		{

			if (PhaseWalking == true)
			{
				if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) == 19 || cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) == 0) &&
					(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1 ) == 19|| cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x+1) == 0))
				{
					return true;
				}
			}

			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) > 1)&& (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) <= 100) ||
				(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) > 1)&& (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) <= 100))
			{
				return false;
			}
		}
	}
	else
	{
		cout << "CPlayer2D::CheckPosition: Unknown direction." << endl;
	}

	return true;
}

// Check if the player is in mid-air
bool CPlayer2D::IsMidAir(void)
{
	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL) {

		// if the player is at the bottom row, then he is not in mid-air for sure
		if (i32vec2Index.y == 0)
		{
			return false;
		}
	
		if ((cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x) < 100) || (cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x) > 110))
		{

			return true;
		}
		else {

			return false;
		}

	}
	else {
		if (i32vec2Index.y == 23) {
			CGameManager::GetInstance()->bPlayerLost = true;
		}
		if ((cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) <= 1))
		{
		
			return true;
		}

		return false;
	}
}

// Update Jump or Fall
void CPlayer2D::UpdateJumpFall(const double dElapsedTime)
{
	glm::vec2 v2Displacement;
	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL)
	{

		// Update the elapsed time to the physics engine
		cPhysics2D.AddElapsedTime((float)dElapsedTime);

		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();

		// Store the current i32vec2Index.y
		int iIndex_YAxis_OLD = i32vec2Index.y;

		// Translate the displacement from pixels to indices
		int iDisplacement_MicroSteps = (int)(v2Displacement.y / cSettings->MICRO_STEP_YAXIS);

		if (i32vec2Index.y >= 0)
		{
			i32vec2NumMicroSteps.y -= fabs(iDisplacement_MicroSteps);
			if (i32vec2NumMicroSteps.y < 0)
			{
				i32vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				i32vec2Index.y--;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(DOWN);

		// Iterate through all rows until the proposed row
		// Check if the player will hit a tile; stop fall if so.
		int iIndex_YAxis_Proposed = i32vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
		{
			// Change the player's index to the current i value
			i32vec2Index.y = i;
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(DOWN) == false)
			{
				// Revert to the previous position
				if (i != iIndex_YAxis_OLD)
					i32vec2Index.y = i + 1;
				cPhysics2D.SetTime(0);
				i32vec2NumMicroSteps.y = 0;
				break;
			}
		}
	}
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::RISE)
	{
		if (i32vec2Index.y == 23) {
			//CGameManager::GetInstance()->bPlayerLost = true;
		}
		else if ((cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) <= 1))
		{
			CooldownTimer = 100;
		}
		else {
	
			CooldownTimer = 0;
		}
		
		// Update the elapsed time to the physics engine
		cPhysics2D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();

		// Store the current i32vec2Index.y
		int iIndex_YAxis_OLD = i32vec2Index.y;

		i32vec2NumMicroSteps.y++;
		if (i32vec2NumMicroSteps.y >= 4)
		{
			i32vec2Index.y++;
			i32vec2NumMicroSteps.y = 0;
		}

		Constraint(UP);

		// Iterate through all rows until the proposed row
		// Check if the player will hit a tile; stop rising if so.
		int iIndex_YAxis_Proposed = i32vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
		{
			// Change the player's index to the current i value
			i32vec2Index.y = i;
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(UP) == false)
			{
				// Revert to the previous position
				if (i != iIndex_YAxis_OLD)
					i32vec2Index.y = i - 1;
				// Set the Physics to idle status
				i32vec2NumMicroSteps.y = 0;
				//bgvccPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
				break;
			}
		}
	}
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::GRAPPLE_RIGHT)
	{
		animatedSprites->PlayAnimation("grappleright", -1, 1.0f);
		// Update the elapsed time to the physics engine
		cPhysics2D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();

		// Store the current i32vec2Index.x
		int iIndex_XAxis_OLD = i32vec2Index.x;

		i32vec2NumMicroSteps.x++;
		if (i32vec2NumMicroSteps.x >= 4)
		{
			i32vec2Index.x++;
			i32vec2NumMicroSteps.x = 0;
		}

		Constraint(UP);

		// Iterate through all rows until the proposed row
		// Check if the player will hit a tile; stop moving right if so.
		int iIndex_XAxis_Proposed = i32vec2Index.x;
		for (int i = iIndex_XAxis_OLD; i >= iIndex_XAxis_Proposed; i--)
		{
			// Change the player's index to the current i value
			i32vec2Index.x = i;
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(UP) == false)
			{
				// Revert to the previous position
				if (i != iIndex_XAxis_OLD)
					i32vec2Index.x = i - 1;
				// Set the Physics to idle status
				i32vec2NumMicroSteps.x = 0;
				break;
			}
			DeGrapple = true;
		}
	}
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::GRAPPLE_LEFT)
	{
		animatedSprites->PlayAnimation("grappleleft", -1, 1.0f);
		// Update the elapsed time to the physics engine
		cPhysics2D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();

		// Store the current i32vec2Index.x
		int iIndex_XAxis_OLD = i32vec2Index.x;

		i32vec2NumMicroSteps.x--;
		if (i32vec2NumMicroSteps.x <= 0)
		{
			i32vec2Index.x--;
			i32vec2NumMicroSteps.x = 4;
		}

		Constraint(UP);

		// Iterate through all rows until the proposed row
		// Check if the player will hit a tile; stop moving left if so.
		int iIndex_XAxis_Proposed = i32vec2Index.x;
		for (int i = iIndex_XAxis_OLD; i >= iIndex_XAxis_Proposed; i--)
		{
			// Change the player's index to the current i value
			i32vec2Index.x = i;
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(UP) == false)
			{
				// Revert to the previous position
				if (i != iIndex_XAxis_OLD)
					i32vec2Index.x = i + 1;
				// Set the Physics to idle status
				i32vec2NumMicroSteps.x = 0;
				break;
			}
			DeGrapple = true;
		}
	}
}

/**
 @brief Let player interact with the map. You can add collectibles such as powerups and health here.
 */
void CPlayer2D::InteractWithMap(void)
{
	switch (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x))
	{
	case 203:
		CGameManager::GetInstance()->bPlayerSpiked = true;
		// Decrease the health by 1
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Remove(1);
		if (CGameManager::GetInstance()->bPlayerDeath == false)
		{
			cSoundController->PlaySoundByID(60);
		}
		break;
	case 21:
		// Increase the health
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Add(1);
		break;
	case 201:
		// Level has been completed
		CGameManager::GetInstance()->bLevelCompleted = true;
	
		break;
	case 204:
		// Erase the key from this position
		cSoundController->PlaySoundByID(70);
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		for (int x = 0; x < 32; x++) {
			for (int y = 0; y < 24; y++) {
				if (cMap2D->GetMapInfo(y, x) == 202) {
					cMap2D->SetMapInfo(y, x, 201);
				}
			}
		}
		break;
	case 205:
		if (ButtonTimer <= 0) {
			ButtonTimer = 30;
			cSoundController->PlaySoundByID(72);
			for (int x = 0; x < 32; x++) {
				for (int y = 0; y < 24; y++) {
					if (cMap2D->GetMapInfo(y, x) == 60) {
						cMap2D->SetMapInfo(y+1, x, 102);
					}
					if (cMap2D->GetMapInfo(y, x) == 205) {
						cMap2D->SetMapInfo(y, x, 206);
					}
					if (cMap2D->GetMapInfo(y, x) == 20) {
						cMap2D->SetMapInfo(y, x, 207);
					}
				}
			}
		}
		break;
	case 208:
		CGameManager::GetInstance()->bPlayerWon = true;
	case 210:
		cSoundController->PlaySoundByID(71);
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Add(50);
		if (cInventoryItem->GetCount() >= 100)
		{
			cInventoryItem->Remove(cInventoryItem->GetCount() - 100);
		}
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
	default:
		CGameManager::GetInstance()->bPlayerSpiked = false;
		break;
	}
}

/**
 @brief Update the health and lives.
 */
void CPlayer2D::UpdateHealthLives(void)
{
	// Touched by enemy
	if (CGameManager::GetInstance()->bPlayerTouched == true)
	{
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Remove(1);
		cSoundController->PlaySoundByID(60);

	}
	else if (CGameManager::GetInstance()->bPlayerStabbed == true)
	{
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Remove(10);
		cSoundController->PlaySoundByID(60);
	}

	// Update health 
	cInventoryItem = cInventoryManager->GetItem("Health");
	// Check if Health is 0
	if (cInventoryItem->GetCount() <= 0)
	{
		// Alerts player death
		CGameManager::GetInstance()->bPlayerDeath = true;

		bool DeathSound = true;
		if (DeathSound == true && count == 0)
		{
			cSoundController->PlaySoundByID(61);
			count++;
		}
		
		
		
	}
}

void CPlayer2D::KillSound()
{
	cSoundController->PlaySoundByID(92);
}
