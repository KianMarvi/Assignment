#ifndef SCENE_TEXT_H
#define SCENE_TEXT_H

#include "Scene.h"
#include "Mtx44.h"
#include "PlayerInfo/PlayerInfo.h"
#include "GroundEntity.h"
#include "FPSCamera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "GenericEntity.h"
#include "Enemy/Enemy3D.h"
#include "HardwareAbstraction\Keyboard.h"
#include "Minimap\Minimap.h"
#include "CameraEffects\CameraEffects.h"
#include "HardwareAbstraction\Mouse.h"
#include "SpatialPartition/Grid.h"

class ShaderProgram;
class SceneManager;
class TextEntity;
class Light;
class SceneText : public Scene
{	
public:
	SceneText();
	~SceneText();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

protected:
	// Create Entities to display in this game
	virtual void CreateEntities(void);

private:
	SceneText(SceneManager* _sceneMgr); // This is used to register to SceneManager

	ShaderProgram* currProg;
	CPlayerInfo* playerInfo;
	GroundEntity* groundEntity;
	FPSCamera camera;
	FPSCamera camera2;
	ostringstream DisplayText;
	TextEntity* textObj[4];
	Light* lights[2];

	CKeyboard* theKeyboard;
	CMouse* theMouse;
	CMinimap* theMinimap;
	CCameraEffects* theCameraEffects;

	CGrid* theGrid;

	GenericEntity* theCube;
//	CEnemy3D* anEnemy3D;	// This is the CEnemy class for 3D use.

	static SceneText* sInstance; // The pointer to the object that gets registered
};

#endif