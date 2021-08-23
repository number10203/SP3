/**
CGameManager: This class manages the gameplay and determines 
			  if it should go to next level or end.
By: Toh Da Jun
Date: May 2021
*/
#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"

#include <map>
#include <string>
#include "InventoryItem.h"

class CGameManager : public CSingletonTemplate<CGameManager>
{
	friend CSingletonTemplate<CGameManager>;
public:
	// Initialise this class instance
	void Init(void);


	//// Boolean flag to indicate if the player is in Medieval Dimension
	//bool bPlayerMedieval;
	//// Boolean flag to indicate if the player is in Home Dimension
	//bool bPlayerHome;
	//// Boolean flag to indicate if the player is in Sky Dimension
	//bool bPlayerSky;
	//// Boolean flag to indicate if the player is in Cave Dimension
	//bool bPlayerCave;
	// Boolean flag to indicate if the player won the game
	bool bPlayerWon;
	// Boolean flag to indicate if the player lost the game
	bool bPlayerLost;
	// Boolean flag to indicate if the level is completed
	bool bLevelCompleted;
	// Boolean flag to indicate if the level is paused
	bool bLevelPaused;
	// Boolean flag to indicate if switching is on cooldown
	bool bPlayerCooldown;
	// Boolean flag to indicate if the level is be replayed
	bool bLevelToReplay;
	// Boolean flag to indicate if the game is be restarted
	bool bGameToRestart;

	// Boolean flag to indicate if the player has died
	bool bPlayerDeath;

	//indicator which dimension
	string currDimem;

	enum Dimension
	{
		HOME = 0,
		MEDI = 1,
		CAVE = 2,
		SKY = 3,
		DIMEN_NUM,
	};


protected:
	// Constructor
	CGameManager(void);

	// Destructor
	virtual ~CGameManager(void);
};
