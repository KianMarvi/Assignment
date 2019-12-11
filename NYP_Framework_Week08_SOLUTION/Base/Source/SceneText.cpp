#include "SceneText.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "EntityManager.h"
#include "RenderHelper.h"
#include "FPSCounter.h"

#include "GenericEntity.h"
#include "GroundEntity.h"
#include "TextEntity.h"
#include "SpriteEntity.h"
#include "Light.h"
#include "SkyBox/SkyBoxEntity.h"
#include "Minimap\Minimap.h"
#include "SceneGraph\SceneGraph.h"
#include "SceneGraph\UpdateTransformation.h"
#include "SpatialPartition\SpatialPartition.h"
#include "FrustumCulling\FrustumCulling.h"

#include <iostream>
using namespace std;

SceneText* SceneText::sInstance = new SceneText(SceneManager::GetInstance());

SceneText::SceneText()
	: theMinimap(NULL)
	, theCameraEffects(NULL)
	, theMouse(NULL)
	, theKeyboard(NULL)
{
}

SceneText::SceneText(SceneManager* _sceneMgr)
	: theMinimap(NULL)
	, theCameraEffects(NULL)
	, theMouse(NULL)
	, theKeyboard(NULL)
{
	_sceneMgr->AddScene("Start", this);
}

SceneText::~SceneText()
{
	if (theCameraEffects)
	{
		delete theCameraEffects;
		theCameraEffects = NULL;
	}
	if (theMinimap)
	{
		delete theMinimap;
		theMinimap = NULL;
	}
	if (theMouse)
	{
		delete theMouse;
		theMouse = NULL;
	}
	if (theKeyboard)
	{
		delete theKeyboard;
		theKeyboard = NULL;
	}

	// Delete the scene graph
	CSceneGraph::GetInstance()->Destroy();
	// Delete the Spatial Partition
	CSpatialPartition::GetInstance()->Destroy();
	// Delete the EntityManager
	EntityManager::GetInstance()->Destroy();
}

void SceneText::Init()
{
	currProg = GraphicsManager::GetInstance()->LoadShader("default", "Shader//comg.vertexshader", "Shader//comg.fragmentshader");
	
	// Tell the shader program to store these uniform locations
	currProg->AddUniform("MVP");
	currProg->AddUniform("MV");
	currProg->AddUniform("MV_inverse_transpose");
	currProg->AddUniform("material.kAmbient");
	currProg->AddUniform("material.kDiffuse");
	currProg->AddUniform("material.kSpecular");
	currProg->AddUniform("material.kShininess");
	currProg->AddUniform("lightEnabled");
	currProg->AddUniform("numLights");
	currProg->AddUniform("lights[0].type");
	currProg->AddUniform("lights[0].position_cameraspace");
	currProg->AddUniform("lights[0].color");
	currProg->AddUniform("lights[0].power");
	currProg->AddUniform("lights[0].kC");
	currProg->AddUniform("lights[0].kL");
	currProg->AddUniform("lights[0].kQ");
	currProg->AddUniform("lights[0].spotDirection");
	currProg->AddUniform("lights[0].cosCutoff");
	currProg->AddUniform("lights[0].cosInner");
	currProg->AddUniform("lights[0].exponent");
	currProg->AddUniform("lights[1].type");
	currProg->AddUniform("lights[1].position_cameraspace");
	currProg->AddUniform("lights[1].color");
	currProg->AddUniform("lights[1].power");
	currProg->AddUniform("lights[1].kC");
	currProg->AddUniform("lights[1].kL");
	currProg->AddUniform("lights[1].kQ");
	currProg->AddUniform("lights[1].spotDirection");
	currProg->AddUniform("lights[1].cosCutoff");
	currProg->AddUniform("lights[1].cosInner");
	currProg->AddUniform("lights[1].exponent");
	currProg->AddUniform("colorTextureEnabled");
	currProg->AddUniform("colorTexture");
	currProg->AddUniform("textEnabled");
	currProg->AddUniform("textColor");
	
	// Tell the graphics manager to use the shader we just loaded
	GraphicsManager::GetInstance()->SetActiveShader("default");

	lights[0] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[0]", lights[0]);
	lights[0]->type = Light::LIGHT_DIRECTIONAL;
	lights[0]->position.Set(0, 20, 0);
	lights[0]->color.Set(1, 1, 1);
	lights[0]->power = 1;
	lights[0]->kC = 1.f;
	lights[0]->kL = 0.01f;
	lights[0]->kQ = 0.001f;
	lights[0]->cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0]->cosInner = cos(Math::DegreeToRadian(30));
	lights[0]->exponent = 3.f;
	lights[0]->spotDirection.Set(0.f, 1.f, 0.f);
	lights[0]->name = "lights[0]";

	lights[1] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[1]", lights[1]);
	lights[1]->type = Light::LIGHT_DIRECTIONAL;
	lights[1]->position.Set(1, 1, 0);
	lights[1]->color.Set(1, 1, 0.5f);
	lights[1]->power = 0.4f;
	lights[1]->name = "lights[1]";

	currProg->UpdateInt("numLights", 1);
	currProg->UpdateInt("textEnabled", 0);
	
	// Create the playerinfo instance, which manages all information about the player
	playerInfo = CPlayerInfo::GetInstance();
	playerInfo->Init();

	// Create and attach the camera to the scene
	//camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	camera.Init(playerInfo->GetPos(), playerInfo->GetTarget(), playerInfo->GetUp());
	playerInfo->AttachCamera(&camera);
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	// Second camera
	camera2.Init(Vector3(0, 700, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));

	// Load all the meshes
	MeshBuilder::GetInstance()->GenerateAxes("reference");
	MeshBuilder::GetInstance()->GenerateCrossHair("crosshair");
	MeshBuilder::GetInstance()->GenerateQuad("quad", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("quad")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GetMesh("text")->material.kAmbient.Set(1, 0, 0);
	MeshBuilder::GetInstance()->GenerateRing("ring", Color(1, 0, 1), 36, 1, 0.5f);
	MeshBuilder::GetInstance()->GenerateSphere("lightball", Color(1, 1, 1), 18, 36, 1.f);
	MeshBuilder::GetInstance()->GenerateSphere("sphere", Color(1, 0, 0), 18, 36, 1.f);
	MeshBuilder::GetInstance()->GenerateSphere("greenSphere", Color(0, 1, 0), 18, 36, 1.f);
	MeshBuilder::GetInstance()->GenerateCone("cone", Color(0.5f, 1, 0.3f), 36, 10.f, 10.f);
	MeshBuilder::GetInstance()->GenerateCube("cube", Color(1.0f, 1.0f, 0.0f), 1.0f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kDiffuse.Set(0.99f, 0.99f, 0.99f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kSpecular.Set(0.f, 0.f, 0.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRASS_DARKGREEN", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("GRASS_DARKGREEN")->textureID = LoadTGA("Image//soil_ground.tga");

	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_FRONT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BACK", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_LEFT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_RIGHT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_TOP", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BOTTOM", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_FRONT")->textureID = LoadTGA("Image//sist_ft.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BACK")->textureID = LoadTGA("Image//sist_bk.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_LEFT")->textureID = LoadTGA("Image//sist_lf.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_RIGHT")->textureID = LoadTGA("Image//sist_rt.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_TOP")->textureID = LoadTGA("Image//sist_up.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BOTTOM")->textureID = LoadTGA("Image//sist_dn.tga");

	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH", Color(1, 1, 0), 1.f);
	MeshBuilder::GetInstance()->GenerateRay("laser", 1.0f);

	MeshBuilder::GetInstance()->GenerateOBJ("VASE_HIGH", "Image//Vase_High.obj");
	MeshBuilder::GetInstance()->GetMesh("VASE_HIGH")->textureID = LoadTGA("Image//chair.tga");

	MeshBuilder::GetInstance()->GenerateOBJ("VASE_MID", "Image//Vase_Mid.obj");
	MeshBuilder::GetInstance()->GetMesh("VASE_MID")->textureID = LoadTGA("Image//toilet.tga");

	MeshBuilder::GetInstance()->GenerateOBJ("VASE_LOW", "Image//Vase_Low.obj");
	MeshBuilder::GetInstance()->GetMesh("VASE_LOW")->textureID = LoadTGA("Image//bed.tga");

	// Set up the Spatial Partition and pass it to the EntityManager to manage
	CSpatialPartition::GetInstance()->Init(100, 100, 10, 10);
	CSpatialPartition::GetInstance()->SetMeshRenderMode(CGrid::FILL);
	CSpatialPartition::GetInstance()->SetMesh("GRIDMESH");
	CSpatialPartition::GetInstance()->SetCamera(&camera);
	CSpatialPartition::GetInstance()->SetLevelOfDetails(10000.0f, 160000.0f);

	// Initialise the Frustum Culling
	CFrustumCulling::GetInstance()->Init(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);

	// Create entities into the scene
	Create::Entity("reference", Vector3(0.0f, 0.0f, 0.0f)); // Reference
	Create::Entity("lightball", Vector3(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z)); // Lightball
	//GenericEntity* aCube = Create::Entity("cube", Vector3(-20.0f, 0.0f, -20.0f));
	Create::Entity("ring", Vector3(0.0f, 0.0f, 0.0f)); // Reference


	
	//groundEntity->Ini
//	Create::Text3DObject("text", Vector3(0.0f, 0.0f, 0.0f), "DM2210", Vector3(10.0f, 10.0f, 10.0f), Color(0, 1, 1));
	Create::Sprite2DObject("crosshair", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

	SkyBoxEntity* theSkyBox = Create::SkyBox("SKYBOX_FRONT", "SKYBOX_BACK",
											 "SKYBOX_LEFT", "SKYBOX_RIGHT",
											 "SKYBOX_TOP", "SKYBOX_BOTTOM");

	// Customise the ground entity
	//groundEntity->InitLOD("GEO_GRASS_LIGHTGREEN", "GEO_GRASS_LIGHTGREEN", "GEO_GRASS_LIGHTGREEN");
	groundEntity = Create::Ground("GRASS_DARKGREEN", "GRASS_DARKGREEN");
	groundEntity->SetPosition(Vector3(0, -10, 0));
	groundEntity->SetScale(Vector3(100.0f, 100.0f, 100.0f));
	groundEntity->SetGrids(Vector3(10.0f, 1.0f, 10.0f));
	playerInfo->SetTerrain(groundEntity);

	// Create entities such as NPC etc
	this->CreateEntities();

	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
	for (int i = 0; i < 3; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f,1.0f,0.0f));
	}
	textObj[0]->SetText("HELLO WORLD");

	// Hardware Abstraction
	theKeyboard = new CKeyboard();
	theKeyboard->Create(playerInfo);

	// Activate the Blend Function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Minimap
	theMinimap = Create::Minimap(false);
	theMinimap->SetBackground(MeshBuilder::GetInstance()->GenerateQuad("MINIMAP", Color(1, 1, 1), 1.f));
	theMinimap->GetBackground()->textureID = LoadTGA("Image//minimap_square.tga");
	theMinimap->SetBorder(MeshBuilder::GetInstance()->GenerateCircle("MINIMAPBORDER", Color(1, 1, 1), 1.05f));
	theMinimap->SetAvatar(MeshBuilder::GetInstance()->GenerateQuad("MINIMAPAVATAR", Color(1, 1, 0), 0.25f));
	theMinimap->GetAvatar()->textureID = LoadTGA("Image//Avatar.tga");
	theMinimap->SetStencil(MeshBuilder::GetInstance()->GenerateCircle("MINIMAP_STENCIL", Color(1, 1, 1), 1.0f));

	// CameraEffects
	theCameraEffects = Create::CameraEffects(false);
	theCameraEffects->SetBloodScreen(MeshBuilder::GetInstance()->GenerateQuad("CAMERAEFFECTS_BLOODSCREEN", Color(1, 1, 1), 1.f));
	theCameraEffects->GetBloodScreen()->textureID = LoadTGA("Image//CameraEffects_Blood.tga");
	theCameraEffects->SetStatus_BloodScreen(false);

	// Hardware Abstraction
	theKeyboard = new CKeyboard();
	theKeyboard->Create(playerInfo);

	theMouse = new CMouse();
	theMouse->Create(playerInfo);
}

void SceneText::Update(double dt)
{
	// Update our entities
	EntityManager::GetInstance()->Update(dt);

	// THIS WHOLE CHUNK TILL <THERE> CAN REMOVE INTO ENTITIES LOGIC! Or maybe into a scene function to keep the update clean
	/*if(KeyboardController::GetInstance()->IsKeyDown('1'))
		glEnable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('2'))
		glDisable(GL_CULL_FACE);*/
	//if(KeyboardController::GetInstance()->IsKeyDown('3'))
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//if(KeyboardController::GetInstance()->IsKeyDown('4'))
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//
	if(KeyboardController::GetInstance()->IsKeyDown('5'))
	{
		lights[0]->type = Light::LIGHT_POINT;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('6'))
	{
		lights[0]->type = Light::LIGHT_DIRECTIONAL;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('7'))
	{
		lights[0]->type = Light::LIGHT_SPOT;
	}

	if(KeyboardController::GetInstance()->IsKeyDown('I'))
		lights[0]->position.z -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('K'))
		lights[0]->position.z += (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('J'))
		lights[0]->position.x -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('L'))
		lights[0]->position.x += (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('O'))
		lights[0]->position.y -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('P'))
		lights[0]->position.y += (float)(10.f * dt);

	// if the left mouse button was released
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::LMB))
	{
		cout << "Left Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::RMB))
	{
		cout << "Right Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::MMB))
	{
		cout << "Middle Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) != 0.0)
	{
		//cout << "Mouse Wheel has offset in X-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) << endl;
	}
	if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) != 0.0)
	{
		//cout << "Mouse Wheel has offset in Y-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) << endl;
	}
	// <THERE>

	// Hardware Abstraction
	theKeyboard->Read((float)dt);
	theMouse->Read((float)dt);

	// Update the player position and other details based on keyboard and mouse inputs
	playerInfo->Update(dt);

	//camera.Update(dt); // Can put the camera into an entity rather than here (Then we don't have to write this)

	// Update the Scene Graph
	CSceneGraph::GetInstance()->Update((float)dt);

	// Update the Spatial Partition
	CSpatialPartition::GetInstance()->Update();

	// Update NPC
	//enemyInfo->Update(dt);

	GraphicsManager::GetInstance()->UpdateLights(dt);

	// Update the 2 text object values. NOTE: Can do this in their own class but i'm lazy to do it now :P
	// Eg. FPSRenderEntity or inside RenderUI for LightEntity
	DisplayText.str("");
	DisplayText.clear();
	DisplayText << "FPS: " << CFPSCounter::GetInstance()->GetFrameRate() << endl;
	textObj[1]->SetText(DisplayText.str());

	DisplayText.str("");
	DisplayText.clear();
	DisplayText << "Sway:" << playerInfo->m_fCameraSwayAngle;
	textObj[2]->SetText(DisplayText.str());



	// Update camera effects
	theCameraEffects->Update((float)dt);
}

void SceneText::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	// Setup 3D pipeline then render 3D
	GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	if (KeyboardController::GetInstance()->IsKeyDown(VK_RETURN))
	{
		GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
		GraphicsManager::GetInstance()->AttachCamera(&camera2);
	}

	// PreRenderMesh
	RenderHelper::PreRenderMesh();
		EntityManager::GetInstance()->Render();
		CSceneGraph::GetInstance()->Render();
		CSpatialPartition::GetInstance()->Render(playerInfo->GetPos(), playerInfo->GetTarget(), playerInfo->GetUp());
	// PostRenderMesh
	RenderHelper::PostRenderMesh();

	// Enable blend mode
	glEnable(GL_BLEND);

		// Setup 2D pipeline then render 2D
		int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
		int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;
		GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
		GraphicsManager::GetInstance()->DetachCamera();

		// PreRenderText
		RenderHelper::PreRenderText();

			EntityManager::GetInstance()->RenderUI();

			if (KeyboardController::GetInstance()->IsKeyDown('9'))
				theCameraEffects->SetStatus_BloodScreen(true);
			// Render Camera Effects
			theCameraEffects->RenderUI();

			// Render Minimap
			theMinimap->RenderUI();

		// PostRenderText
		RenderHelper::PostRenderText();

	// Disable blend mode
	glDisable(GL_BLEND);
}

void SceneText::Exit()
{
	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();
	playerInfo->DetachCamera();

	if (playerInfo->DropInstance() == false)
	{
#if _DEBUGMODE==1
		cout << "Unable to drop PlayerInfo class" << endl;
#endif
	}

	// Delete the lights
	delete lights[0];
	delete lights[1];
}

/********************************************************************************
Create Entities to display in this game
********************************************************************************/
void SceneText::CreateEntities(void)
{
	// Add a cube to act as the torso of a NPC



	GenericEntity* pNPCTorso = Create::Entity("VASE_HIGH", Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), false);

	pNPCTorso->InitLOD("VASE_HIGH", "VASE_MID", "VASE_LOW");
	pNPCTorso->SetCollider(true);
	pNPCTorso->SetAABB(Vector3(0.45f, 0.45f, 0.45f), Vector3(-0.45f, -0.45f, -0.45f));

	// Add the pointer to the root of the Scene Graph
	CSceneNode* pNPCSceneNode = CSceneGraph::GetInstance()->AddNode(pNPCTorso);
	pNPCSceneNode->SetTranslate(Vector3(0.0f, 1.0f, -10.0f));
	CUpdateTransformation* aRotateMtx = new CUpdateTransformation();
	aRotateMtx->ApplyUpdate(1.0f, 0, 1, 0);
	aRotateMtx->SetSteps(-60, 60);
	pNPCSceneNode->SetUpdateTransformation(aRotateMtx);

	// Add the entity into the Spatial Partition
	CSpatialPartition::GetInstance()->Add(pNPCTorso);

	
	CSceneGraph::GetInstance()->PrintSelf();

	
	srand(NULL);
	for (int i = 0; i < 100; i++)
	{
		CEnemy3D* anEnemy3D = Create::Enemy3D("VASE_HIGH",
									Vector3(rand() % 1000 - 500.0f, 0.0f, rand() % 1000 - 500.0f), 
									Vector3(1.0f, 1.0f, 1.0f), 
									false);
		anEnemy3D->InitLOD("VASE_HIGH", "VASE_MID", "VASE_LOW");
		anEnemy3D->Init();
		//anEnemy3D->SetPos(Vector3(0, 0, 0));
		anEnemy3D->SetSpeed(10.0f);
		anEnemy3D->SetCollider(true);
		anEnemy3D->SetAABB(Vector3(0.5, 0.5, 0.5), Vector3(-0.5, -0.5, -0.5));

		anEnemy3D->SetTerrain(groundEntity);

		// Add the entity into the Spatial Partition
		CSpatialPartition::GetInstance()->Add(anEnemy3D);

		pNPCSceneNode = CSceneGraph::GetInstance()->AddNode(anEnemy3D);
		pNPCSceneNode->SetTranslate(Vector3(0.0f, 0.0f, 0.0f));
	}

	CSceneGraph::GetInstance()->ReCalc_AABB();
}