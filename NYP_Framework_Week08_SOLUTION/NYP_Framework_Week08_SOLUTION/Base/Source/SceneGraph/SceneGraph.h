#pragma once

#include "SingletonTemplate.h"
#include "EntityBase.h"
#include "Vector3.h"
#include <string>
#include "SceneNode.h"

class Mesh;

class CSceneGraph : public Singleton<CSceneGraph>
{
	friend Singleton<CSceneGraph>;
public:
	CSceneGraph(void);
	virtual ~CSceneGraph();

	void Destroy();

	// Get the root of this Scene Graph
	CSceneNode* GetRoot() const;

	// Add a Node to this Scene Graph
	CSceneNode* AddNode(EntityBase* theEntity = NULL);
	// Delete a Node from this Scene Graph using the pointer to the node
	bool DeleteNode(EntityBase* theEntity);
	// Delete a Node from this Scene Graph using its ID
	bool DeleteNode(const int ID);
	// Detach a Node from this Scene Graph using the pointer to the entity
	CSceneNode* DetachNode(EntityBase* theEntity);
	// Detach a Node from this Scene Graph using its ID
	CSceneNode* DetachNode(const int ID);
	// Get a Node using the pointer to the node
	CSceneNode* GetNode(EntityBase* theEntity) const;
	// Get a Node using its ID
	CSceneNode* GetNode(const int ID) const;
	// Return the number of nodes in this Scene Graph
	int GetNumOfNode(void) const;
	// Recalculate the AABB
	void ReCalc_AABB(void);

	// Generate an ID for a Scene Node
	int GenerateID(void);

	// Update the Scene Graph
	void Update(const float dt);
	// Render the Scene Graph
	void Render(void) const;

	// PrintSelf for debug purposes
	void PrintSelf(void) const;

protected:
	// The root of the scene graph. 
	// It usually does not have a mesh, and is the starting point for all scene graph operations
	CSceneNode* theRoot;

	// The next ID to be assigned to a scene node.
	int ID;
};
