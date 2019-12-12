#include "Grid.h"
#include "stdio.h"
#include "MeshBuilder.h"
#include "RenderHelper.h"
#include "../GenericEntity.h"
#include "../SceneGraph/SceneGraph.h"

/********************************************************************************
Constructor
********************************************************************************/
CGrid::CGrid(void)
	: index(Vector3(-1, -1, -1))
	, size(Vector3(-1, -1, -1))
	, offset(Vector3(-1, -1, -1))
	, min(Vector3(-1, -1, -1))
	, max(Vector3(-1, -1, -1))
	, theMesh(NULL)
	, ListOfObjects(NULL)
	, meshRenderMode(FILL)
	, theDetailLevel(CLevelOfDetails::NO_DETAILS)
{
}

/********************************************************************************
Destructor
********************************************************************************/
CGrid::~CGrid(void)
{
	if (theMesh)
	{
		// Do not delete the Mesh here as MeshBuilder will take care of them.
		//delete theMesh;
		theMesh = NULL;
	}
	Remove();
}

/********************************************************************************
Initialise this grid
********************************************************************************/
void CGrid::Init(	const int xIndex, const int zIndex,
					const int xGridSize, const int zGridSize,
					const float xOffset, const float zOffset)
{
	index.Set((float)xIndex, 0.0f, (float)zIndex);
	size.Set((float)xGridSize, 0.0f, (float)zGridSize);
	offset.Set(xOffset, 0.0f, zOffset);
	min.Set(index.x * size.x - offset.x, 0.0f, index.z * size.z - offset.z);
	max.Set(index.x * size.x - offset.x + xGridSize, 0.0f, index.z * size.z - offset.z + zGridSize);
}

/********************************************************************************
 Set a particular grid's Mesh
********************************************************************************/
void CGrid::SetMesh(const std::string& _meshName)
{
	Mesh* modelMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	if (modelMesh != nullptr)
	{
		theMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	}
}

/********************************************************************************
 Set Mesh's Render Mode
********************************************************************************/
void CGrid::SetMeshRenderMode(SMeshRenderMode meshRenderMode)
{
	this->meshRenderMode = meshRenderMode;
}

/********************************************************************************
 Get Mesh's Render Mode
********************************************************************************/
CGrid::SMeshRenderMode CGrid::GetMeshRenderMode(void) const
{
	return meshRenderMode;
}


/********************************************************************************
Update the grid
********************************************************************************/
void CGrid::Update(vector<EntityBase*>* migrationList)
{
	// Check each object to see if they are no longer in this grid
	std::vector<EntityBase*>::iterator it;
	it = ListOfObjects.begin();
	while (it != ListOfObjects.end())
	{
		Vector3 position = (*it)->GetPosition();
		if (((min.x <= position.x) && (position.x < max.x)) &&
			((min.z <= position.z) && (position.z < max.z)))
		{
			// Move on otherwise
			++it;
		}
		else
		{
			migrationList->push_back(*it);

			// Remove from this Grid
			it = ListOfObjects.erase(it);
		}
	}
}

/********************************************************************************
Render
********************************************************************************/
void CGrid::Render(void)
{
	if (theMesh)
	{
		if (ListOfObjects.size() > 0)
		{
			// Set to wire render mode if wire render mode is required
			if (meshRenderMode == WIRE)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			RenderHelper::RenderMesh(theMesh);
			// Set back to fill render mode if wire render mode is required
			if (meshRenderMode == WIRE)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

/********************************************************************************
RenderObjects
********************************************************************************/
void CGrid::RenderObjects(const int RESOLUTION)
{
	/*
	glPushAttrib(GL_ENABLE_BIT);
	// Draw the Grid and its list of objects
	for (int i=0; i<(int)ListOfObjects.size(); i++)
	{
	ListOfObjects[i]->Render(RESOLUTION);
	}
	glPopAttrib();
	*/
}

/********************************************************************************
Add a new object to this grid
********************************************************************************/
void CGrid::Add(EntityBase* theObject)
{
	for (int i = 0; i < (int)ListOfObjects.size(); ++i)
	{
		if (ListOfObjects[i] == theObject)
			return;
	}
	ListOfObjects.push_back( theObject );
	ChangeGridColor();
}

/********************************************************************************
 Remove but not delete object from this grid
********************************************************************************/
void CGrid::Remove(void)
{
	for (int i = 0; i < (int)ListOfObjects.size(); i++)
	{
		// We can delete the entities here.
		// If you delete them here, then do not delete in Scene Graph or SceneText
		delete ListOfObjects[i];
		ListOfObjects[i] = NULL;
		ChangeGridColor();
	}
	ListOfObjects.clear();
}

/********************************************************************************
 Remove but not delete an object from this grid
********************************************************************************/
bool CGrid::Remove(EntityBase* theObject)
{
	// Clean up entities that are done
	std::vector<EntityBase*>::iterator it, end;
	it = ListOfObjects.begin();
	end = ListOfObjects.end();
	while (it != end)
	{
		if ((*it) == theObject)
		{
			it = ListOfObjects.erase(it);
			ChangeGridColor();
			return true;
		}
		else
		{
			// Move on otherwise
			++it;
		}
	}
	return false;
}

/********************************************************************************
 Check if an object is in this grid
********************************************************************************/
bool CGrid::IsHere(EntityBase* theObject) const
{
	for (int i = 0; i < (int)ListOfObjects.size(); ++i)
	{
		if (ListOfObjects[i] == theObject)
			return true;
	}
	return false;
}

/********************************************************************************
Get list of objects in this grid
********************************************************************************/
vector<EntityBase*> CGrid::GetListOfObject(Vector3 position, const float radius)
{
	// if the radius of inclusion is not specified, or illegal value specified
	// then return all entities in this grid
	if (radius <= 0.0f)
	{
		return ListOfObjects;
	}

	vector<EntityBase*> theListOfObjects;
	for (int i = 0; i < (int)ListOfObjects.size(); ++i)
	{
		// Calculate the distance between the object and the supplied position
		// And check if it is within the radius
		if ((ListOfObjects[i]->GetPosition() - position).LengthSquared() < radius * radius)
		{
			theListOfObjects.push_back(ListOfObjects[i]);
		}
	}
	return theListOfObjects;
}

/********************************************************************************
Set the Level of Detail for objects in this CGrid
********************************************************************************/
void CGrid::SetDetailLevel(const CLevelOfDetails::DETAIL_LEVEL theDetailLevel)
{
	this->theDetailLevel = theDetailLevel;

	// Update the objects in this grid to the specified level of detail
	GenericEntity* aGenericEntity = NULL;
	for (int i = 0; i < (int)ListOfObjects.size(); ++i)
	{
		aGenericEntity = (GenericEntity*)ListOfObjects[i];
		if (aGenericEntity->GetLODStatus() == true)
		{
			aGenericEntity->SetDetailLevel(theDetailLevel);
		}
	}
}

// Get number of objects in this grid
int CGrid::GetNumOfObject(void) const
{
	return ListOfObjects.size();
}

/********************************************************************************
 PrintSelf
 ********************************************************************************/
void CGrid::PrintSelf(void)
{
	if (ListOfObjects.size() > 0)
	{
		cout << "********************************************************************************" << endl;
		cout << "CGrid::PrintSelf()" << endl;
		cout << "\tIndex\t:\t" << index << "\t\tOffset\t:\t" << offset << endl;
		cout << "\tMin\t:\t" << min << "\tMax\t:\t" << max << endl;
		cout << "\tList of objects in this grid: (LOD:" << this->theDetailLevel << ")" << endl;
		cout << "\t------------------------------------------------------------------------" << endl;

		for (int i = 0; i < (int)ListOfObjects.size(); ++i)
		{
			cout << "\t" << i << "\t:\t" << ListOfObjects[i]->GetPosition() << endl;
			GenericEntity* aGenericEntity = (GenericEntity*)ListOfObjects[i];
			cout << aGenericEntity->GetLODStatus() << endl;
			if (aGenericEntity->GetLODStatus() == true)
			{
				cout << "\t\t\t\t*** LOD Level: " << ((GenericEntity*)ListOfObjects[i])->GetDetailLevel() << endl;
			}
		}
		cout << "\t------------------------------------------------------------------------" << endl;
		cout << "********************************************************************************" << endl;
	}
	else
	{
		//cout << "\tThis grid has no entities." << endl;
	}
}

 void CGrid::ChangeGridColor()
 {
	 if (ListOfObjects.size() >= 1)
	 {
		 SetMesh("GRID_YELLOW");
	 }
	 if (ListOfObjects.size() >= 3)
	 {
		 SetMesh("GRID_ORANGE");
	 }
	 if (ListOfObjects.size() >= 5)
	 {
		 SetMesh("GRID_RED");
	 }
 }
