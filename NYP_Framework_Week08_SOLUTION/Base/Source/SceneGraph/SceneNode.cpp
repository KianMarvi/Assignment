#include "SceneNode.h"
#include "../EntityManager.h"
#include <algorithm>

#include "SceneGraph.h"
#include "GraphicsManager.h"
#include "../GenericEntity.h"

CSceneNode::CSceneNode(void)
	: ID(-1)
	, theEntity(NULL)
	, theParent(NULL)
{
}

CSceneNode::~CSceneNode()
{
}

// Release all memory for this node and its children
void CSceneNode::Destroy(void)
{
	// Destroy the children first
	std::vector<CSceneNode*>::iterator it;
	for (it = theChildren.begin(); it != theChildren.end(); ++it)
	{
		(*it)->Destroy();
		delete *it;
		theChildren.erase(it);
	}

	// Reset the pointers for this node
	theEntity = NULL;
	theParent = NULL;
}

// Set the ID for this node
void CSceneNode::SetID(const int ID)
{
	this->ID = ID;
}

// Get the ID for this node
int CSceneNode::GetID(void) const
{
	return ID;
}

// Set the entity for this node
bool CSceneNode::SetEntity(EntityBase* theEntity)
{
	if (theEntity)
	{
		this->theEntity = theEntity;
		return true;
	}
	return false;
}
// Get the ID for this node
EntityBase* CSceneNode::GetEntity(void) const
{
	return theEntity;
}

// Set a parent to this node
void CSceneNode::SetParent(CSceneNode* theParent)
{
	this->theParent = theParent;
}

// Get parent of this node
CSceneNode* CSceneNode::GetParent(void) const
{
	return theParent;
}

// Add a child to this node
CSceneNode* CSceneNode::AddChild(EntityBase* theEntity)
{
	if (theEntity)
	{
		// Create a new Scene Node
		CSceneNode* aNewNode = new CSceneNode();
		// Set the entity to this new scene node
		aNewNode->SetEntity(theEntity);
		// Store the pointer to the parent
		aNewNode->SetParent(this);
		// Assign an ID to this node
		aNewNode->SetID(CSceneGraph::GetInstance()->GenerateID());
		// Add to vector list
		this->theChildren.push_back(aNewNode);
		// Return this new scene node
		return aNewNode;
	}

	cout << "CSceneNode::AddChild: Unable to add to this scene node, " << this->GetID() << endl;
	return NULL;
}
// Delete a child from this node using the pointer to the entity
bool CSceneNode::DeleteChild(EntityBase* theEntity)
{
	// if this node contains theEntity, then we proceed to delete all its children
	if (this->theEntity == theEntity)
	{
		// If this node has children, then we proceed to delete them.
		if (theChildren.size() != 0)
		{
			// Iterate through all the children and delete all of their children and grand children etc
			vector <CSceneNode*>::iterator it = theChildren.begin();
			while (it != theChildren.end())
			{
				if ((*it)->DeleteAllChildren())
				{
					cout << "CSceneNode::DeleteChild: Deleted child nodes for theEntity." << endl;
				}
				(*it)->GetEntity()->SetIsDone(true);
				delete *it;
				it = theChildren.erase(it);
			}
		}
		return true;	// return true to say that this Node contains theEntity
	}
	else
	{
		// Search the children for this particular theEntity
		if (theChildren.size() != 0)
		{
			std::vector<CSceneNode*>::iterator it;
			for (it = theChildren.begin(); it != theChildren.end(); ++it)
			{
				// Check if this child is the one containing theEntity
				if ((*it)->DeleteChild(theEntity))
				{
					// If DeleteChild method call above DID remove theEntity
					// Then we should proceed to removed this child from our vector of children
					(*it)->GetEntity()->SetIsDone(true);
					delete *it;
					theChildren.erase(it);
					//break;	// Stop deleting since we have already found and deleted theEntity
					return true;
				}
			}
		}
	}

	return false;
}

// Delete a child from this node using its ID
bool CSceneNode::DeleteChild(const int ID)
{
	// if this node contains theEntity, then we proceed to delete all its children
	if (this->ID == ID)
	{
		// If this node has children, then we proceed to delete them.
		if (theChildren.size() != 0)
		{
			// Iterate through all the children and delete all of their children and grand children etc
			vector <CSceneNode*>::iterator it = theChildren.begin();
			while (it != theChildren.end())
			{
				if ((*it)->DeleteAllChildren())
				{
					cout << "CSceneNode::DeleteChild: Deleted child nodes for ID=" << ID << endl;
				}
				(*it)->GetEntity()->SetIsDone(true);
				delete *it;
				it = theChildren.erase(it);
			}
		}
		return true;	// return true to say that this Node contains theEntity
	}
	else
	{
		// Search the children for this particular theEntity
		if (theChildren.size() != 0)
		{
			std::vector<CSceneNode*>::iterator it;
			for (it = theChildren.begin(); it != theChildren.end(); ++it)
			{
				// Check if this child is the one containing theEntity
				if ((*it)->DeleteChild(theEntity))
				{
					// If DeleteChild method call above DID remove theEntity
					// Then we should proceed to removed this child from our vector of children
					(*it)->GetEntity()->SetIsDone(true);
					delete *it;
					theChildren.erase(it);
					//break;	// Stop deleting since we have already found and deleted theEntity
					return true;
				}
			}
		}
	}

	return false;
}
// Delete all children from this node using its ID
bool CSceneNode::DeleteAllChildren(void)
{
	bool bResult = false;

	vector <CSceneNode*>::iterator it = theChildren.begin();
	while (it != theChildren.end())
	{
		if ((*it)->DeleteAllChildren())
		{
			cout << "CSceneNode::DeleteChild: Delete child nodes." << endl;
		}
		(*it)->GetEntity()->SetIsDone(true);
		delete *it;
		it = theChildren.erase(it);
		bResult = true;
	}
	return bResult;
}
// Detach a child from this node using the pointer to the node
CSceneNode* CSceneNode::DetachChild(EntityBase* theEntity)
{
	if (theChildren.size() != 0)
	{
		vector <CSceneNode*>::iterator it = theChildren.begin();
		while (it != theChildren.end())
		{
			if ((*it)->GetEntity() == theEntity)
			{
				CSceneNode* theNode = (*it)->DetachChild(theEntity);
				if (theNode)
				{
					// Remove this node from the children
					theChildren.erase(it);
					return theNode;
				}
			}
			it++;
		}
	}
	return NULL;
}
// Detach a child from this node using its ID
CSceneNode* CSceneNode::DetachChild(const int ID)
{
	//// if it is inside this node, then return this node
	//if (this->ID == ID)
	//	return this;

	if (theChildren.size() != 0)
	{
		vector <CSceneNode*>::iterator it = theChildren.begin();
		while (it != theChildren.end())
		{
			if ((*it)->GetID() == ID)
			{
				CSceneNode* theNode = (*it)->DetachChild(ID);
				if (theNode)
				{
					// Remove this node from the children
					theChildren.erase(it);
					return theNode;
				}
			}
			it++;
		}
	}
	return NULL;
}
// Get the entity inside this Scene Graph
CSceneNode* CSceneNode::GetEntity(EntityBase* theEntity)
{
	// if it is inside this node, then return this node
	if (this->theEntity == theEntity)
		return this;

	if (theChildren.size() != 0)
	{
		std::vector<CSceneNode*>::iterator it;
		for (it = theChildren.begin(); it != theChildren.end(); ++it)
		{
			CSceneNode* theNode = (*it)->GetEntity(theEntity);
			if (theNode)
			{
				return theNode;
			}
		}
	}
	return NULL;
}
// Get a child from this node using its ID
CSceneNode* CSceneNode::GetEntity(const int ID)
{
	// if it is inside this node, then return this node
	if (this->ID == ID)
		return this;

	// Check the children
	if (theChildren.size() != 0)
	{
		std::vector<CSceneNode*>::iterator it;
		for (it = theChildren.begin(); it != theChildren.end(); ++it)
		{
			CSceneNode* theNode = (*it)->GetEntity(ID);
			if (theNode)
			{
				return theNode;
			}
		}
	}
	return NULL;
}
// Return the number of children in this group
int CSceneNode::GetNumOfChild(void)
{
	// Start with this node's children
	int NumOfChild = theChildren.size();

	// Ask the children to feedback how many children they have
	std::vector<CSceneNode*>::iterator it;
	for (it = theChildren.begin(); it != theChildren.end(); ++it)
	{
		NumOfChild += (*it)->GetNumOfChild();
	}

	return NumOfChild;
}

// Recalculate the AABB (to be called by Scene Graph only)
void CSceneNode::ReCalc_AABB(void)
{
	if (!theEntity)
	{
		cout << "ReCalc_AABB: theRoot start" << endl;
		// If this Scene Node is the root, then just iterate through all the children
		vector <CSceneNode*>::iterator it = theChildren.begin();
		while (it != theChildren.end())
		{
			(*it)->ReCalc_AABB();
			it++;
		}
		cout << "ReCalc_AABB: theRoot end" << endl;
	}
	else
	{
		cout << "\tReCalc_AABB: first tier scene node START" << endl;
		Vector3 theEntityPosition, theMaxAABB, theMinAABB;

		// Get the AABB of the entity in this Scene Node
		GenericEntity* theGenericEntity = dynamic_cast<GenericEntity*>(theEntity);
		theGenericEntity->GetAABB(theMaxAABB, theMinAABB);
		theEntityPosition = theGenericEntity->GetPosition();
		cout << "\t\tBEFORE" << theEntityPosition + theMaxAABB << ", " << theEntityPosition + theMinAABB << endl;

		// Compare the children's AABB against this
		//Vector3 theChildPosition, theChildMaxAABB, theChildMinAABB;
		vector <CSceneNode*>::iterator it = theChildren.begin();
		while (it != theChildren.end())
		{
			// Get the AABB for this child
			(*it)->ReCalc_AABB(theEntityPosition, theMaxAABB, theMinAABB);

			// Go to the next child
			it++;
		}

		// Update the entity's AABB to include the children's AABB
		theGenericEntity->SetAABB(theMaxAABB, theMinAABB);
		cout << "\t\tAFTER" << theEntityPosition + theMaxAABB << ", " << theEntityPosition + theMinAABB << endl;
		cout << "\tReCalc_AABB: first tier scene node END" << endl;
	}
}

// Recalculate the AABB (for Scene Nodes with Entity in it only)
void CSceneNode::ReCalc_AABB(Vector3 theParentPosition, Vector3& MaxAABB, Vector3& MinAABB)
{
	// If this Scene Node has an entity
	// This should not be an issue as there should not be any root node at this point
	if (theEntity)
	{
		cout << "\t\tReCalc_AABB: child scene node START" << endl;
		Vector3 theEntityPosition, theMaxAABB, theMinAABB;

		// Get the AABB of the entity in this Scene Node
		GenericEntity* theGenericEntity = dynamic_cast<GenericEntity*>(theEntity);
		theGenericEntity->GetAABB(theMaxAABB, theMinAABB);
		Vector3 theOffsetFromParent;
		GetTranslate(theOffsetFromParent.x, theOffsetFromParent.y, theOffsetFromParent.z);
		theEntityPosition = theParentPosition + theOffsetFromParent;// theGenericEntity->GetPosition();
		cout << "\t\t\tThis was passed in " << theParentPosition  << ", " << MaxAABB << ", " << MinAABB << endl;
		cout << "\t\t\tThis is the parent's AABB: " << theParentPosition + MaxAABB << ", " << theParentPosition + MinAABB << endl;
		cout << "\t\t\tThis is the child's AABB: " << theEntityPosition + theMaxAABB << ", " << theEntityPosition + theMinAABB << endl;

		// Check if the entity's AABB is larger than the parent
		// If yes, then set the entity's AABB to the parent'sAABB
		if (theEntityPosition.x + theMaxAABB.x > theParentPosition.x + MaxAABB.x)
			MaxAABB.x = (theEntityPosition + theMaxAABB).x - theParentPosition.x;
		if (theEntityPosition.y + theMaxAABB.y > theParentPosition.y + MaxAABB.y)
			MaxAABB.y = (theEntityPosition + theMaxAABB).y - theParentPosition.y;
		if (theEntityPosition.z + theMaxAABB.z > theParentPosition.z + MaxAABB.z)
			MaxAABB.z = (theEntityPosition + theMaxAABB).z - theParentPosition.z;
		if (theEntityPosition.x + theMinAABB.x < theParentPosition.x + MinAABB.x)
			MinAABB.x = (theEntityPosition + theMinAABB).x - theParentPosition.x;
		if (theEntityPosition.y + theMinAABB.y < theParentPosition.y + MinAABB.y)
			MinAABB.y = (theEntityPosition + theMinAABB).x - theParentPosition.y;
		if (theEntityPosition.z + theMinAABB.z < theParentPosition.z + MinAABB.z)
			MinAABB.z = (theEntityPosition + theMinAABB).z - theParentPosition.z;

		cout << "\t\t\tThis was the parent's AABB after adding in the child's" << theParentPosition + MaxAABB << ", " << theParentPosition + MinAABB << endl;

		// Check the children of this Scene Node
		//Vector3 theChildPosition, theChildMaxAABB, theChildMinAABB;
		vector <CSceneNode*>::iterator it = theChildren.begin();
		while (it != theChildren.end())
		{
			// Get the AABB for this child
			(*it)->ReCalc_AABB(theEntityPosition, theMaxAABB, theMinAABB);

			// Go to the next child
			it++;
		}

		// Check if the entity's AABB is larger than the parent
		// If yes, then set the entity's AABB to the parent'sAABB
		if (theEntityPosition.x + theMaxAABB.x > theParentPosition.x + MaxAABB.x)
			MaxAABB.x = (theEntityPosition + theMaxAABB).x - theParentPosition.x;
		if (theEntityPosition.y + theMaxAABB.y > theParentPosition.y + MaxAABB.y)
			MaxAABB.y = (theEntityPosition + theMaxAABB).y - theParentPosition.y;
		if (theEntityPosition.z + theMaxAABB.z > theParentPosition.z + MaxAABB.z)
			MaxAABB.z = (theEntityPosition + theMaxAABB).z - theParentPosition.z;
		if (theEntityPosition.x + theMinAABB.x < theParentPosition.x + MinAABB.x)
			MinAABB.x = (theEntityPosition + theMinAABB).x - theParentPosition.x;
		if (theEntityPosition.y + theMinAABB.y < theParentPosition.y + MinAABB.y)
			MinAABB.y = (theEntityPosition + theMinAABB).y - theParentPosition.y;
		if (theEntityPosition.z + theMinAABB.z < theParentPosition.z + MinAABB.z)
			MinAABB.z = (theEntityPosition + theMinAABB).z - theParentPosition.z;

		// Update the entity's AABB to include the children's AABB
		theGenericEntity->SetAABB(theMaxAABB, theMinAABB);
		cout << "\t\t\tAFTER" << theEntityPosition + theMaxAABB << ", " << theEntityPosition + theMinAABB << endl;
		cout << "\t\tReCalc_AABB: child scene node END" << endl;
	}
}

// Update the Scene Graph
void CSceneNode::Update(const float dt)
{
	// Update the Transformation between this node and its children
	if (theUpdateTransformation)
	{
		ApplyTransform(GetUpdateTransform());
	}

	if (theEntity)
	{
		// Update this Scene Node
		theEntity->Update(dt);
	}

	// Count the number of children in this node
	int NumOfChild = theChildren.size();

	// Update the children
	std::vector<CSceneNode*>::iterator it;
	for (it = theChildren.begin(); it != theChildren.end(); ++it)
	{
		(*it)->Update(dt);
	}
}

// Render the Scene Graph
void CSceneNode::Render(void)
{
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();

		if (theEntity)
		{
			// Update the information of this Scene Node from the Entity
			modelStack.Translate(	theEntity->GetPosition().x, 
									theEntity->GetPosition().y, 
									theEntity->GetPosition().z);
			modelStack.MultMatrix(GetTransform());

			// Render the entity
			theEntity->Render();
		}

		// Render the children
		std::vector<CSceneNode*>::iterator it;
		for (it = theChildren.begin(); it != theChildren.end(); ++it)
		{
			(*it)->Render();
		}

	modelStack.PopMatrix();
}

// PrintSelf for debug purposes
void CSceneNode::PrintSelf(const int numTabs)
{
	if (numTabs == 0)
	{
		cout << "============================" << endl << "Start of theRoot::PrintSelf()" << endl;
		GetTransform().PrintSelf();
		cout << "CSceneNode::PrintSelf: ID=" << ID << "/Children=" << theChildren.size() << endl;
		cout << "Printing out the children:" << endl;
		vector <CSceneNode*>::iterator it = theChildren.begin();
		while (it != theChildren.end())
		{
			(*it)->PrintSelf((numTabs + 1));
			it++;
		}
		cout << "End of theRoot::PrintSelf()" << endl << "============================" << endl;
	}
	else
	{
		for (int i = 0; i < numTabs; i++)
			cout << "\t";
		cout << "CSceneNode::PrintSelf: ID=" << ID << "/theEntity=" << theEntity << "[" << theEntity->GetPosition() << "]" <<
				"/Children=" << theChildren.size() << endl;

		GenericEntity* aGenericEntity = (GenericEntity*)theEntity;
		cout << aGenericEntity->GetLODStatus() << endl;
		if (aGenericEntity->GetLODStatus() == true)
		{
			cout << "\t\t\t\t*** LOD Level: " << aGenericEntity->GetDetailLevel() << endl;
		}


		for (int i = 0; i < numTabs; i++)
			cout << "\t";
		cout << "Printing out the children:" << endl;
		vector <CSceneNode*>::iterator it = theChildren.begin();
		while (it != theChildren.end())
		{
			(*it)->PrintSelf((numTabs + 1));
			it++;
		}
	}
}