#include "GameManager.h"

using namespace std;

/**
@brief Constructor
*/
CGameManager::CGameManager(void)
	: currDimem(0)
	, bPlayerWon(false)
	, bPlayerLost(false)
	, bLevelCompleted(false)
	, bLevelPaused(false)
	, bLevelToReplay(false)
	, bGameToRestart(false)
	, bPlayerTouched(false)
	, bPlayerStabbed(false)
	, bPlayerDeath(false){
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
	currDimem = 0;
	bPlayerWon = false;
	bPlayerLost = false;
	bLevelCompleted = false;
	bLevelPaused = false;
	bLevelToReplay = false;
	bGameToRestart = false;
	bPlayerDeath = false;
	bPlayerTouched = false;
	bPlayerStabbed = false;
}

