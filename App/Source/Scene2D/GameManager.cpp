#include "GameManager.h"

/**
@brief Constructor
*/
CGameManager::CGameManager(void) 
	: bPlayerHome(false)
	, bPlayerMedieval(false)
	, bPlayerCave(false)
	, bPlayerSky(false)
	, bPlayerWon(false)
	, bPlayerLost(false)
	, bLevelCompleted(false)
	, bLevelPaused(false)
	, bPlayerCooldown(false)
	, bLevelToReplay(false)
	, bGameToRestart(false) {
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
	bPlayerHome = true;
	bPlayerMedieval = false;
	bPlayerCave = false;
	bPlayerSky = false;
	bPlayerWon = false;
	bPlayerLost = false;
	bLevelCompleted = false;
	bLevelPaused = false;
	bPlayerCooldown = false;
	bLevelToReplay = false;
	bGameToRestart = false;
}

