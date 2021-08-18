#pragma once

#include "GameStateBase.h"

#include "Primitives/Mesh.h"
#include "../Scene2D/BackgroundEntity.h"

#include <string>

// Include IMGUI
// Important: GLEW and GLFW must be included before IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif

class CSoundState : public CGameStateBase
{
public:
	// Constructor
	CSoundState(void);
	// Destructor
	~CSoundState(void);

	// Init the class
	virtual bool Init(void);
	// Update the class
	virtual bool Update(const double dElapsedTime);
	// Render the class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);

protected:
	struct ButtonData
	{
		std::string fileName;
		unsigned textureID;
	};

	ButtonData VolumeIncreaseButtonData;
	ButtonData VolumeDecreaseButtonData;
};