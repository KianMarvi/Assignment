 #include "SpatialPartition.h"
#include "stdio.h"
#include "Collider\Collider.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "../FrustumCulling/FrustumCulling.h"
#include "KeyboardController.h"

template <typename T> vector<T> concat(vector<T> &a, vector<T> &b) {
	vector<T> ret = vector<T>();
	copy(a.begin(), a.end(), back_inserter(ret));
	copy(b.begin(), b.end(), back_inserter(ret));
	return ret;
}

/********************************************************************************
 Constructor
 ********************************************************************************/
CSpatialPartition::CSpatialPartition(void)
	: theGrid(NULL)
	, xSize(0)
	, zSize(0)
	, xGridSize(0)
	, zGridSize(0)
	, xNumOfGrid(0)
	, zNumOfGrid(0)
	, yOffset(0.0f)
	, _meshName("")
	, theCamera(NULL)
	, halfWindowWidth(Application::GetInstance().GetWindowWidth() * 0.5f)
	, halfWindowHeight(Application::GetInstance().GetWindowHeight() * 0.5f)
	, fontSize(25.f)
	, halfFontSize(fontSize * 0.5f)
{
	textObject[0] = Create::Text2DObject("text", Vector3(-halfWindowWidth / 2.0f, -halfWindowHeight + fontSize + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 1.0f, 0.0f));
	textObject[1] = Create::Text2DObject("text", Vector3(-halfWindowWidth / 2.0f, -halfWindowHeight + fontSize + halfFontSize + 30.f, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 1.0f, 0.0f));
	EnableFrustumCulling();
}

/********************************************************************************
 Destructor
 ********************************************************************************/
CSpatialPartition::~CSpatialPartition(void)
{
	theCamera = NULL;
}

/********************************************************************************
Initialise the spatial partition
********************************************************************************/
void CSpatialPartition::Destroy()
{
	if (theGrid)
	{
		delete[] theGrid;
	}
	Singleton<CSpatialPartition>::Destroy();
}

/********************************************************************************
 Initialise the spatial partition
 ********************************************************************************/
bool CSpatialPartition::Init(	const int xGridSize, const int zGridSize, 
								const int xNumOfGrid, const int zNumOfGrid, 
								const float yOffset)
{
	if ((xGridSize>0) && (zGridSize>0)
		&& (xNumOfGrid>0) && (zNumOfGrid>0))
	{
		this->xNumOfGrid = xNumOfGrid;
		this->zNumOfGrid = zNumOfGrid;
		this->xGridSize = xGridSize;
		this->zGridSize = zGridSize;
		this->xSize = xGridSize * xNumOfGrid;
		this->zSize = zGridSize * zNumOfGrid;
		this->yOffset = yOffset;

		// Create an array of grids
		theGrid = new CGrid[ xNumOfGrid*zNumOfGrid ];

		// Initialise the array of grids
		for (int i=0; i<xNumOfGrid; i++)
		{
			for (int j=0; j<zNumOfGrid; j++)
			{
				theGrid[i*zNumOfGrid + j].Init(i, j, xGridSize, zGridSize, (float)(xSize >> 1), (float)(zSize >> 1));
			}
		}

		

		// Assign a Mesh to each Grid if available.
		ApplyMesh();

		// Create a migration list vector
		MigrationList.clear();

		return true;
	}
	return false;
}

/********************************************************************************
 Set Mesh's Render Mode
 ********************************************************************************/
void CSpatialPartition::SetMeshRenderMode(CGrid::SMeshRenderMode meshRenderMode)
{
	this->meshRenderMode = meshRenderMode;

	// Initialise the array of grids
	for (int i = 0; i<xNumOfGrid; i++)
	{
		for (int j = 0; j<zNumOfGrid; j++)
		{
			theGrid[i*zNumOfGrid + j].SetMeshRenderMode(meshRenderMode);
		}
	}
}

/********************************************************************************
 Get Mesh's Render Mode
 ********************************************************************************/
CGrid::SMeshRenderMode CSpatialPartition::GetMeshRenderMode(void) const
{
	if (xNumOfGrid*zNumOfGrid > 0)
		return theGrid[0].GetMeshRenderMode();
	else
		return CGrid::FILL;
}

/********************************************************************************
 Set a particular grid's Mesh
 ********************************************************************************/
void CSpatialPartition::SetMesh(const std::string& _meshName)
{
	this->_meshName = _meshName;

	// Assign a Mesh to each Grid if available.
	ApplyMesh();
}

/********************************************************************************
  ApplyMesh
 ********************************************************************************/
void CSpatialPartition::ApplyMesh(void)
{
	if (_meshName.size() != 0)
	{
		for (int i = 0; i<xNumOfGrid; i++)
		{
			for (int j = 0; j<zNumOfGrid; j++)
			{
				theGrid[i*zNumOfGrid + j].SetMesh(_meshName);
			}
		}
	}
}

void CSpatialPartition::EnableFrustumCulling()
{
	for (int i = 0; i < xNumOfGrid; i++)
	{
		for (int j = 0; j < zNumOfGrid; j++)
		{
			// Update the grid
			theGrid[i*zNumOfGrid + j].Update(&MigrationList);

			float gridXPos = (float)(xGridSize* i + (xGridSize >> 1) - (xSize >> 1));
			float gridZPos = (float)(zGridSize* j + (zGridSize >> 1) - (zSize >> 1));
			Vector3 gridPos(gridXPos, 0.f, gridZPos);

			// Do Frustum Culling. We only render the grid if it is in the Frustum
			if (CFrustumCulling::GetInstance()->isBoxInFrustum(gridPos, xGridSize, zGridSize))
			{
				if (theGrid[i*zNumOfGrid + j].GetNumOfObject() > 0)
				{
					float distance = CalculateDistanceSquare(&(theCamera->GetCameraPos()), i, j);
					if (distance < LevelOfDetails_Distances[0])
					{
						theGrid[i*zNumOfGrid + j].SetDetailLevel(CLevelOfDetails::HIGH_DETAILS);
					}
					else if (distance < LevelOfDetails_Distances[1])
					{
						theGrid[i*zNumOfGrid + j].SetDetailLevel(CLevelOfDetails::MID_DETAILS);
					}
					else
					{
						theGrid[i*zNumOfGrid + j].SetDetailLevel(CLevelOfDetails::LOW_DETAILS);
					}
				}
			}
			else
			{
				theGrid[i*zNumOfGrid + j].SetDetailLevel(CLevelOfDetails::NO_DETAILS);
			}
		}
		//cout << endl;
	}
}

void CSpatialPartition::DisableFrustumCulling()
{
	for (int i = 0; i < xNumOfGrid; i++)
	{
		for (int j = 0; j < zNumOfGrid; j++)
		{
			// Update the grid
			theGrid[i*zNumOfGrid + j].Update(&MigrationList);

				float distance = CalculateDistanceSquare(&(theCamera->GetCameraPos()), i, j);
				if (distance < LevelOfDetails_Distances[0])
				{
					theGrid[i*zNumOfGrid + j].SetDetailLevel(CLevelOfDetails::HIGH_DETAILS);
				}
				else if (distance < LevelOfDetails_Distances[1])
				{
					theGrid[i*zNumOfGrid + j].SetDetailLevel(CLevelOfDetails::MID_DETAILS);
				}
				else
				{
					theGrid[i*zNumOfGrid + j].SetDetailLevel(CLevelOfDetails::LOW_DETAILS);
				}

			}

		//cout << endl;
	}
}

/********************************************************************************
Update the spatial partition
********************************************************************************/
void CSpatialPartition::Update(void)
{
	//CFrustumCulling::GetInstance()->Update(theCamera->GetCameraPos(), theCamera->GetCameraTarget(), theCamera->GetCameraUp());
	//cout << "Rendering these grids:" << endl;

	if (KeyboardController::GetInstance()->IsKeyDown('1'))
	{
		EnableFrustumCulling();
	}

	if (KeyboardController::GetInstance()->IsKeyDown('2'))
	{
		DisableFrustumCulling();
	}
	

	// If there are objects due for migration, then process them
	if (MigrationList.empty() == false)
	{
		// Check each object to see if they are no longer in this grid
		for (int i = 0; i < (int)MigrationList.size(); ++i)
		{
			Add(MigrationList[i]);
		}

		MigrationList.clear();
	}
}

/********************************************************************************
Render the spatial partition
********************************************************************************/
void CSpatialPartition::Render(Vector3 theCameraPosition, Vector3 theCameraTarget, Vector3 theCameraUp)
{
	//Update frustum culling values
	CFrustumCulling::GetInstance()->Update(theCameraPosition, theCameraTarget, theCameraUp);

	// Render the Spatial Partitions
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();

	modelStack.PushMatrix();
	modelStack.Translate(0.0f, yOffset, 0.0f);

	for (int i = 0; i < xNumOfGrid; i++)
	{
		for (int j = 0; j < zNumOfGrid; j++)
		{
			// Set up the variables for the grid coordinates
			float gridXPos = (float)(xGridSize* i + (xGridSize >> 1) - (xSize >> 1));
			float gridZPos = (float)(zGridSize* j + (zGridSize >> 1) - (zSize >> 1));
			Vector3 gridPos(gridXPos, 0.f, gridZPos);
  
			// Do Frustum Culling. We only render the grid if it is in the Frustum
			if (CFrustumCulling::GetInstance()->isBoxInFrustum(gridPos, xGridSize, zGridSize))
			{
				if (theGrid[i*zNumOfGrid + j].GetNumOfObject() > 0)
				{
					// Demonstrate that frustum culling is working
				    //	cout << "Frustum Culling works!" << endl;
					
					modelStack.PushMatrix();
					modelStack.Translate((float)(xGridSize*i - (xSize >> 1) + (xGridSize >> 1)), 0.0f, (float)(zGridSize*j - (zSize >> 1) + (zGridSize >> 1)));
					modelStack.PushMatrix();
					modelStack.Scale((float)xGridSize, 1.0f, (float)zGridSize);
					modelStack.Rotate(-90, 1, 0, 0);
					theGrid[i*zNumOfGrid + j].Render();
					modelStack.PopMatrix();
					modelStack.PopMatrix();
				}
			}
			
		}
	}

	modelStack.PopMatrix();
}

/********************************************************************************
 Get xSize of the entire spatial partition
********************************************************************************/
int CSpatialPartition::GetxSize(void) const
{
	return xSize;
}
/********************************************************************************
 Get zSize of the entire spatial partition
********************************************************************************/
int CSpatialPartition::GetzSize(void) const
{
	return zSize;
}
/********************************************************************************
 Get xSize
********************************************************************************/
int CSpatialPartition::GetxGridSize(void) const
{
	return xGridSize;
}
/********************************************************************************
 Get zNumOfGrid
********************************************************************************/
int CSpatialPartition::GetzGridSize(void) const
{
	return zGridSize;
}
/********************************************************************************
Get xNumOfGrid
********************************************************************************/
int CSpatialPartition::GetxNumOfGrid(void) const
{
	return xNumOfGrid;
}
/********************************************************************************
Get zNumOfGrid
********************************************************************************/
int CSpatialPartition::GetzNumOfGrid(void) const
{
	return zNumOfGrid;
}

/********************************************************************************
 Get a particular grid
 ********************************************************************************/
CGrid CSpatialPartition::GetGrid(const int xIndex, const int yIndex) const
{
	return theGrid[ xIndex*zNumOfGrid + yIndex ];
}

/********************************************************************************
 Get vector of objects from this Spatial Partition
 ********************************************************************************/
vector<EntityBase*> CSpatialPartition::GetObjects(Vector3 position, const float radius)
{
	// Get the indices of the object's position
	int xIndex = (((int)position.x - (-xSize >> 1)) / (xSize / xNumOfGrid));
	int zIndex = (((int)position.z - (-zSize >> 1)) / (zSize / zNumOfGrid));

	if (((xIndex < 0) || (zIndex < 0)) || ((xIndex >= xNumOfGrid) || (zIndex >= zNumOfGrid)))
	{
		// Return an empty list
		vector<EntityBase*> ListOfObjects;
		return ListOfObjects;
	}

	return theGrid[xIndex*zNumOfGrid + zIndex].GetListOfObject(position, radius);
}

/********************************************************************************
 Add a new object model
 ********************************************************************************/
void CSpatialPartition::Add(EntityBase* theObject)
{
	// Get the indices of the object's position
	int xIndex = (((int)theObject->GetPosition().x - (-xSize >> 1)) / (xSize / xNumOfGrid));
	int zIndex = (((int)theObject->GetPosition().z - (-zSize >> 1)) / (zSize / zNumOfGrid));

	// Add them to each grid
	if (((xIndex >= 0) && (xIndex<xNumOfGrid)) && ((zIndex >= 0) && (zIndex<zNumOfGrid)))
	{
		theGrid[xIndex*zNumOfGrid + zIndex].Add(theObject);
	}
}

// Remove but not delete object from this grid
void CSpatialPartition::Remove(EntityBase* theObject)
{
	// Get the indices of the object's position
	int xIndex = (((int)theObject->GetPosition().x - (-xSize >> 1)) / (xSize / xNumOfGrid));
	int zIndex = (((int)theObject->GetPosition().z - (-zSize >> 1)) / (zSize / zNumOfGrid));

	// Add them to each grid
	if (((xIndex >= 0) && (xIndex<xNumOfGrid)) && ((zIndex >= 0) && (zIndex<zNumOfGrid)))
	{
		theGrid[xIndex*zNumOfGrid + zIndex].Remove(theObject);
	}
}

/********************************************************************************
 Calculate the squared distance from camera to a grid's centrepoint
 ********************************************************************************/
float CSpatialPartition::CalculateDistanceSquare(Vector3* theCameraPosition, const int xIndex, const int zIndex)
{
	float xDistance = (xGridSize*xIndex - (xSize >> 1) + (xGridSize >> 1)) - theCameraPosition->x;
	float yDistance = (zGridSize*zIndex - (zSize >> 1) + (zGridSize >> 1)) - theCameraPosition->z;

	return (float) ( xDistance*xDistance + yDistance*yDistance );
}

/********************************************************************************
Store a camera pointer into this class
********************************************************************************/
void CSpatialPartition::SetCamera(FPSCamera* _cameraPtr)
{
	theCamera = _cameraPtr;
}

/********************************************************************************
Remove the camera pointer from this class
********************************************************************************/
void CSpatialPartition::RemoveCamera(void)
{
	theCamera = nullptr;
}

/********************************************************************************
Set LOD distances
********************************************************************************/
void CSpatialPartition::SetLevelOfDetails(const float distance_High2Mid, const float distance_Mid2Low)
{
	LevelOfDetails_Distances[0] = distance_High2Mid;
	LevelOfDetails_Distances[1] = distance_Mid2Low;
}

/********************************************************************************
Check if a CGrid is visible to the camera
********************************************************************************/
bool CSpatialPartition::IsVisible(Vector3 theCameraPosition,
	Vector3 theCameraDirection,
	const int xIndex, const int zIndex)
{
	float xDistance = (xGridSize*xIndex + (xGridSize >> 1) 
						- (xSize >> 1)) - theCameraPosition.x;
	float zDistance = (zGridSize*zIndex + (zGridSize >> 1) 
						- (zSize >> 1)) - theCameraPosition.z;
	// If the camera is within the CGrid, then display by default
	// Otherwise, the entities in the grid are not displayed.
	if (xDistance*xDistance + zDistance*zDistance < (xGridSize*xGridSize + zGridSize*zGridSize))
		return true;
	Vector3 gridCentre(xDistance, 0, zDistance);
	if (theCameraDirection.Dot(gridCentre) < 0)
	{
		return false;
	}
	return true;
}

/********************************************************************************
 PrintSelf
 ********************************************************************************/
void CSpatialPartition::PrintSelf() const
{
	cout << "******* Start of CSpatialPartition::PrintSelf() **********************************" << endl;
	cout << "xSize\t:\t" << xSize << "\tzSize\t:\t" << zSize << endl;
	cout << "xNumOfGrid\t:\t" << xNumOfGrid << "\tzNumOfGrid\t:\t" << zNumOfGrid << endl;
	if (theGrid)
	{
		cout << "theGrid : OK" << endl;
		cout << "Printing out theGrid below: " << endl;
		for (int i=0; i<xNumOfGrid; i++)
		{
			for (int j=0; j<zNumOfGrid; j++)
			{
				theGrid[ i*zNumOfGrid + j ].PrintSelf();
			}
		}
	}
	else
		cout << "theGrid : NULL" << endl;
	cout << "******* End of CSpatialPartition::PrintSelf() **********************************" << endl;
}
