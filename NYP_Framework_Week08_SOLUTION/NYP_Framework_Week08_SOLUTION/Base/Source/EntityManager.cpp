#include "EntityManager.h"
#include "EntityBase.h"
#include "Collider/Collider.h"
#include <vector>
#include "SpatialPartition\SpatialPartition.h"
#include "SceneGraph\SceneGraph.h"
#include "Projectile/Laser.h"

#include <iostream>
using namespace std;

// Update all entities
void EntityManager::Update(double _dt)
{
	// Update all entities
	std::list<EntityBase*>::iterator it, end;
	end = entityList.end();
	for (it = entityList.begin(); it != end; ++it)
	{
		(*it)->Update(_dt);
	}

	// Update all projectiles
	std::list<EntityBase*>::iterator item_projectile = projectileList.begin();
	while (item_projectile != projectileList.end())
	{
		(*item_projectile)->Update(_dt);
		++item_projectile;
	}


	// Check for Collision amongst entities with collider properties
	CheckForCollision();

	// Clean up entities that are done
	it = entityList.begin();
	while (it != end)
	{
		if ((*it)->IsDone())
		{
			// Delete if done
			delete *it;
			it = entityList.erase(it);
		}
		else
		{
			// Move on otherwise
			++it;
		}
	}

	// Clean up projectiles that are done
	item_projectile = projectileList.begin();
	while (item_projectile != projectileList.end())
	{
		if ((*item_projectile)->IsDone())
		{
			cout << "Removing a projectile..." << endl;
			// Delete if done
			delete *item_projectile;
			projectileList.erase(item_projectile++);
		}
		else
		{
			// Move on otherwise
			++item_projectile;
		}
	}
}

// Render all entities
void EntityManager::Render()
{
	// Render all entities
	std::list<EntityBase*>::iterator it, end;
	end = entityList.end();
	for (it = entityList.begin(); it != end; ++it)
	{
		(*it)->Render();
	}

	// Render the projectiles
	std::list<EntityBase*>::iterator item_projectile = projectileList.begin();
	while (item_projectile != projectileList.end())
	{
		(*item_projectile)->Render();
		++item_projectile;
	}
}

// Render the UI entities
void EntityManager::RenderUI()
{
	// Render all entities UI
	std::list<EntityBase*>::iterator it, end;
	end = entityList.end();
	for (it = entityList.begin(); it != end; ++it)
	{
		(*it)->RenderUI();
	}
}

// Add an entity to this EntityManager
void EntityManager::AddEntity(EntityBase* _newEntity, ENTITY_TYPE sEntityType)
{
	if (sEntityType == FIXED)
		entityList.push_back(_newEntity);
	else if (sEntityType == PROJECTILE)
		projectileList.push_back(_newEntity);
}

// Remove an entity from this EntityManager
bool EntityManager::RemoveEntity(EntityBase* _existingEntity)
{
	// Find the entity's iterator
	std::list<EntityBase*>::iterator findIter = std::find(entityList.begin(), entityList.end(), _existingEntity);

	// Delete the entity if found
	if (findIter != entityList.end())
	{
		delete *findIter;
		findIter = entityList.erase(findIter);
		return true;	
	}
	// Return false if not found
	return false;
}

// Destroy this EntityManager and delete all entities and projectiles
void EntityManager::Destroy(void)
{
	// Delete all entities
	std::list<EntityBase*>::iterator it, end;
	it = entityList.begin();
	end = entityList.end();
	while (it != end)
	{
		// Delete if done
		delete *it;
		it = entityList.erase(it);
	}

	// Delete all projectiles
	std::list<EntityBase*>::iterator item_projectile = projectileList.begin();
	while (item_projectile != projectileList.end())
	{
		delete *item_projectile;
		projectileList.erase(item_projectile++);
	}

	// Destroy the Entity Manager's Singleton instance
	Singleton<EntityManager>::Destroy();
}

// Constructor
EntityManager::EntityManager()
{
}

// Destructor
EntityManager::~EntityManager()
{
}

// Check for overlap
bool EntityManager::CheckOverlap(Vector3 thisMinAABB, Vector3 thisMaxAABB, Vector3 thatMinAABB, Vector3 thatMaxAABB)
{	
	// Check if this object is overlapping that object
	if (((thatMinAABB >= thisMinAABB) && (thatMinAABB <= thisMaxAABB))
		||
		((thatMaxAABB >= thisMinAABB) && (thatMaxAABB <= thisMaxAABB)))
	{
		return true;
	}

	// Check if that object is overlapping this object
	if (((thisMinAABB >= thatMinAABB) && (thisMinAABB <= thatMaxAABB))
		||
		((thisMaxAABB >= thatMinAABB) && (thisMaxAABB <= thatMaxAABB)))
	{
		return true;
	}

	// Check if this object is within that object
	if (((thisMinAABB >= thatMinAABB) && (thisMaxAABB <= thatMaxAABB))
		&&
		((thisMaxAABB >= thatMinAABB) && (thisMaxAABB <= thatMaxAABB)))
		return true;

	// Check if that object is within this object
	if (((thatMinAABB >= thisMinAABB) && (thatMinAABB <= thisMaxAABB))
		&&
		((thatMaxAABB >= thisMinAABB) && (thatMaxAABB <= thisMaxAABB)))
		return true;

	return false;
}

// Check if this entity's bounding sphere collided with that entity's bounding sphere 
bool EntityManager::CheckSphereCollision(EntityBase *ThisEntity, EntityBase *ThatEntity)
{
	// Get the colliders for the 2 entities
	CCollider *thisCollider = dynamic_cast<CCollider*>(ThisEntity);
	CCollider *thatCollider = dynamic_cast<CCollider*>(ThatEntity);

	// Get the minAABB and maxAABB for each entity
	Vector3 thisMinAABB = ThisEntity->GetPosition() + thisCollider->GetMinAABB();
	Vector3 thisMaxAABB = ThisEntity->GetPosition() + thisCollider->GetMaxAABB();
	Vector3 thatMinAABB = ThatEntity->GetPosition() + thatCollider->GetMinAABB();
	Vector3 thatMaxAABB = ThatEntity->GetPosition() + thatCollider->GetMaxAABB();

	// if Radius of bounding sphere of ThisEntity plus Radius of bounding sphere of ThatEntity is 
	// greater than the distance squared between the 2 reference points of the 2 entities,
	// then it could mean that they are colliding with each other.
	if (DistanceSquaredBetween(thisMinAABB, thisMaxAABB) + DistanceSquaredBetween(thatMinAABB, thatMaxAABB) >
		DistanceSquaredBetween(ThisEntity->GetPosition(), ThatEntity->GetPosition()) * 2.0)
	{
		return true;
	}

	return false;
}

// Check if this entity collided with another entity, but both must have collider
bool EntityManager::CheckAABBCollision(EntityBase *ThisEntity, EntityBase *ThatEntity)
{
	// Get the colliders for the 2 entities
	CCollider *thisCollider = dynamic_cast<CCollider*>(ThisEntity);
	CCollider *thatCollider = dynamic_cast<CCollider*>(ThatEntity);

	// Get the minAABB and maxAABB for each entity
	Vector3 thisMinAABB = ThisEntity->GetPosition() + thisCollider->GetMinAABB();
	Vector3 thisMaxAABB = ThisEntity->GetPosition() + thisCollider->GetMaxAABB();
	Vector3 thatMinAABB = ThatEntity->GetPosition() + thatCollider->GetMinAABB();
	Vector3 thatMaxAABB = ThatEntity->GetPosition() + thatCollider->GetMaxAABB();

	// Check for overlap
	if (CheckOverlap(thisMinAABB, thisMaxAABB, thatMinAABB, thatMaxAABB))
		return true;

	// if AABB collision check fails, then we need to check the other corners of the bounding boxes to 
	// Do more collision checks with other points on each bounding box
	Vector3 altThisMinAABB = Vector3(thisMinAABB.x, thisMinAABB.y, thisMaxAABB.z);
	Vector3 altThisMaxAABB = Vector3(thisMaxAABB.x, thisMaxAABB.y, thisMinAABB.z);
	//	Vector3 altThatMinAABB = Vector3(thatMinAABB.x, thatMinAABB.y, thatMaxAABB.z);
	//	Vector3 altThatMaxAABB = Vector3(thatMaxAABB.x, thatMaxAABB.y, thatMinAABB.z);

	// Check for overlap
	if (CheckOverlap(altThisMinAABB, altThisMaxAABB, thatMinAABB, thatMaxAABB))
		return true;

	return false;
}


// Check where a line segment between two positions intersects a plane
bool EntityManager::GetIntersection(const float fDst1, const float fDst2, Vector3 P1, Vector3 P2, Vector3 &Hit)
{
	if ((fDst1 * fDst2) >= 0.0f)
		return false;
	if (fDst1 == fDst2)
		return false;
	Hit = P1 + (P2 - P1) * (-fDst1 / (fDst2 - fDst1));
	return true;
}

// Check two positions are within a box region
bool EntityManager::InBox(Vector3 Hit, Vector3 B1, Vector3 B2, const int Axis)
{
	if (Axis == 1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return true;
	if (Axis == 2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return true;
	if (Axis == 3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return true;
	return false;
}

// Check for intersection between a line segment and a plane
bool EntityManager::CheckLineSegmentPlane(Vector3 line_start, Vector3 line_end,
										  Vector3 minAABB, Vector3 maxAABB,
										  Vector3 &Hit)
{
	if ((GetIntersection(line_start.x - minAABB.x, line_end.x - minAABB.x, line_start, line_end, Hit) &&
		InBox(Hit, minAABB, maxAABB, 1))
		|| (GetIntersection(line_start.y - minAABB.y, line_end.y - minAABB.y, line_start, line_end, Hit) &&
		InBox(Hit, minAABB, maxAABB, 2))
		|| (GetIntersection(line_start.z - minAABB.z, line_end.z - minAABB.z, line_start, line_end, Hit) &&
		InBox(Hit, minAABB, maxAABB, 3))
		|| (GetIntersection(line_start.x - maxAABB.x, line_end.x - maxAABB.x, line_start, line_end, Hit) &&
		InBox(Hit, minAABB, maxAABB, 1))
		|| (GetIntersection(line_start.y - maxAABB.y, line_end.y - maxAABB.y, line_start, line_end, Hit) &&
		InBox(Hit, minAABB, maxAABB, 2))
		|| (GetIntersection(line_start.z - maxAABB.z, line_end.z - maxAABB.z, line_start, line_end, Hit) &&
		InBox(Hit, minAABB, maxAABB, 3)))
	{
		cout << "*** Laser has collided with an entity! ***" << endl;
		return true;
	}

	return false;
}

// Check if any Collider is colliding with another Collider
bool EntityManager::CheckForCollision(void)
{
	std::list<EntityBase*>::iterator projectileCollider;

	// Check the projectiles for collision with other moving entities in Spatial Partition
	projectileCollider = projectileList.begin();
	while (projectileCollider != projectileList.end())
	{
		// This object was derived from a CCollider class, then it will have Collision Detection methods
		EntityBase *aProjectile = dynamic_cast<EntityBase*>(*projectileCollider);

		// if this projectile does not have a collider, then skip
		if (!aProjectile->HasCollider())
		{
			// Go to the next projectile
			++projectileCollider;
			continue;
		}

		// Check if this entity is a CLaser type
		if (aProjectile->GetIsLaser())
		{
			// Dynamic cast it to a CLaser class
			CLaser* theLaser = dynamic_cast<CLaser*>(aProjectile);

			// Get the list of entities near this projectile's position from the SpatialPartition
			vector<EntityBase*> ExportList = CSpatialPartition::GetInstance()->GetObjects(aProjectile->GetPosition(),
				20.0f);
			for (int i = 0; i < (int)ExportList.size(); ++i)
			{
				if (ExportList[i]->HasCollider())
				{
					Vector3 hitPosition = Vector3(0, 0, 0);

					// Get the minAABB and maxAABB for (*colliderThat)
					CCollider *thatCollider = dynamic_cast<CCollider*>(ExportList[i]);
					Vector3 thatMinAABB = (ExportList[i])->GetPosition() + thatCollider->GetMinAABB();
					Vector3 thatMaxAABB = (ExportList[i])->GetPosition() + thatCollider->GetMaxAABB();

					if (CheckLineSegmentPlane(theLaser->GetPosition() + theLaser->GetDirection() * theLaser->GetLength(),
											  theLaser->GetPosition(),
											  thatMinAABB, thatMaxAABB,
											  hitPosition) == true)
					{
						aProjectile->SetIsDone(true);
						ExportList[i]->SetIsDone(true);
						// Remove from Spatial Partition first
						CSpatialPartition::GetInstance()->Remove(ExportList[i]);
						// Do the actual removal from Scene Graph
						if (CSceneGraph::GetInstance()->DeleteNode(ExportList[i]) == false)
						{
							cout << "*** EntityManager::CheckForCollision(void): ***" << endl;
							cout << "*** Unable to remove a mobile object from the Scene Graph ***" << endl;
						}
						cout << "*** Laser beam! ***" << endl;
					}
				}
			}
		}
		else
		{
			// Get the list of entities near this projectile's position from the SpatialPartition
			vector<EntityBase*> ExportList = CSpatialPartition::GetInstance()->GetObjects(aProjectile->GetPosition(),
				20.0f);
			for (int i = 0; i < (int)ExportList.size(); ++i)
			{
				if (ExportList[i]->HasCollider())
				{
					if (CheckSphereCollision(aProjectile, ExportList[i]))
					{
						if (CheckAABBCollision(aProjectile, ExportList[i]))
						{
							// Mark the projectiles for removal in EntityManager::Update()
							aProjectile->SetIsDone(true);
							// Mark the projectiles for removal but this is not really necessary
							ExportList[i]->SetIsDone(true);
							// Remove from Spatial Partition first
							CSpatialPartition::GetInstance()->Remove(ExportList[i]);
							// Do the actual removal from Scene Graph
							if (CSceneGraph::GetInstance()->DeleteNode(ExportList[i]) == false)
							{
								cout << "*** EntityManager::CheckForCollision(void): ***" << endl;
								cout << "*** Unable to remove a mobile object from the Scene Graph ***" << endl;
							}
						}
					}
				}
			}
		}

		// Go to the next projectile
		++projectileCollider;
	}

	/*
	// Check for Collision
	std::list<EntityBase*>::iterator colliderThis, colliderThisEnd;
	std::list<EntityBase*>::iterator colliderThat, colliderThatEnd;

	colliderThisEnd = entityList.end();
	for (colliderThis = entityList.begin(); colliderThis != colliderThisEnd; ++colliderThis)
	{
		// This object was derived from a CCollider class, then it will have Collision Detection methods
		//CCollider *thisCollider = dynamic_cast<CCollider*>(*colliderThis);
		EntityBase *thisEntity = dynamic_cast<EntityBase*>(*colliderThis);

		if (thisEntity->HasCollider())
		{
			// Check for collision with another collider class
			colliderThatEnd = entityList.end();
			for (colliderThat = entityList.begin(); colliderThat != colliderThatEnd; ++colliderThat)
			{
				if (colliderThat == colliderThis)
					continue;

				if ((*colliderThat)->HasCollider())
				{
					EntityBase *thatEntity = dynamic_cast<EntityBase*>(*colliderThat);

					if (CheckSphereCollision(thisEntity, thatEntity))
					{
						if (CheckAABBCollision(thisEntity, thatEntity))
						{
							thisEntity->SetIsDone(true);
							thatEntity->SetIsDone(true);
						}
					}
				}
			}
		}
	}
	*/
	return false;
}
