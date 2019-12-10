#pragma once

#include "Vector3.h"
#include "Grid.h"
#include "EntityBase.h"
#include "SingletonTemplate.h"
#include "../FPSCamera.h"
#include "../TextEntity.h"
#include "../Application.h"
#include <iostream>
#include <string>
#include <sstream>

class CSpatialPartition : public Singleton<CSpatialPartition>
{
	friend Singleton<CSpatialPartition>;
public:
	// Destructor
	virtual ~CSpatialPartition();

	// Destroy the Singleton instance
	void Destroy();

	// Initialise the spatial partition
	bool Init(	const int xGridSize, const int zGridSize, 
				const int xNumOfGrid, const int zNumOfGrid, 
				const float yOffset = -9.9f);

	// Set Mesh's Render Mode
	void SetMeshRenderMode(CGrid::SMeshRenderMode meshRenderMode);
	// Get Mesh's Render Mode
	CGrid::SMeshRenderMode GetMeshRenderMode(void) const;

	// Set a particular grid's Mesh
	void SetMesh(const std::string& _meshName);

	// ApplyMesh
	void ApplyMesh(void);

	// Update the spatial partition
	void Update(void);
	// Render the spatial partition
	void Render(Vector3 theCameraPosition, Vector3 theCameraTarget, Vector3 theCameraUp);

	// Get xSize of the entire spatial partition
	int GetxSize(void) const;
	// Get zSize of the entire spatial partition
	int GetzSize(void) const;
	// Get xSize
	int GetxGridSize(void) const;
	// Get zNumOfGrid
	int GetzGridSize(void) const;
	// Get xNumOfGrid
	int GetxNumOfGrid(void) const;
	// Get zNumOfGrid
	int GetzNumOfGrid(void) const;

	// Get a particular grid
	CGrid GetGrid(const int xIndex, const int zIndex) const;

	// Get vector of objects from this Spatial Partition
	vector<EntityBase*> GetObjects(Vector3 position, const float radius);

	// Add a new object
	void Add(EntityBase* theObject);
	// Remove but not delete object from this grid
	void Remove(EntityBase* theObject);

	// Calculate the squared distance from camera to a grid's centrepoint
	float CalculateDistanceSquare(Vector3* theCameraPosition, const int xIndex, const int zIndex);

	// Set the camera pointer to this class instance
	void SetCamera(FPSCamera* _cameraPtr);
	// Get the camera pointer stored in this class instance
	void RemoveCamera(void);

	// Set LOD distances
	void SetLevelOfDetails(const float distance_High2Mid, const float distance_Mid2Low);
	// Check if a CGrid is visible to the camera
	bool IsVisible(Vector3 theCameraPosition, Vector3 theCameraDirection, const int xIndex, const int zIndex);

	//PrintSelf
	void PrintSelf() const;

	// The vector of objects due for migration to another grid
	vector<EntityBase*> MigrationList;

protected:
	// Constructor
	CSpatialPartition(void);

	// Variables
	CGrid* theGrid;
	int xSize;
	int zSize;
	int xGridSize;
	int zGridSize;
	int xNumOfGrid;
	int zNumOfGrid;
	float yOffset;
	std::string _meshName; // Name of the mesh

	float halfWindowWidth;
	float halfWindowHeight;
	float fontSize = 25.0f;
	float halfFontSize;

	TextEntity* textObject[2];

	std::ostringstream displayText;

//	bool EnableVisibilityCheck;

	// Define the mesh render mode
	CGrid::SMeshRenderMode meshRenderMode;

	// We store the pointer to the Camera so we can get it's position and direction to calculate LOD and visibility
	FPSCamera* theCamera;

	// LOD distances
	float LevelOfDetails_Distances[2];
};
