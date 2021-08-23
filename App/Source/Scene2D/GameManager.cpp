#include "GameManager.h"

/**
@brief Constructor
*/
CGameManager::CGameManager(void)
	: currDimem(0)
	, bPlayerWon(false)
	, bPlayerLost(false)
	, bLevelCompleted(false)
	, bLevelPaused(false)
	, bPlayerCooldown(false)
	, bLevelToReplay(false)
	, bGameToRestart(false)
	, bPlayerDeath(false) {
}

/**
@brief Destructor
*/
CGameManager::~CGameManager(void)
{
}

// Initialise this class instance
void CGameManager::Init(void)
{
	//bPlayerHome = true;
	//bPlayerMedieval = false;
	//bPlayerCave = false;
	//bPlayerSky = false;
	currDimem = HOME;
	bPlayerWon = false;
	bPlayerLost = false;
	bLevelCompleted = false;
	bLevelPaused = false;
	bPlayerCooldown = false;
	bLevelToReplay = false;
	bGameToRestart = false;
	bPlayerDeath = false;
}

