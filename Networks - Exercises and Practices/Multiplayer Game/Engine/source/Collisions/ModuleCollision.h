#ifndef _MODULE_COLLISION_H_
#define _MODULE_COLLISION_H_

struct Collider
{
	ColliderType type = ColliderType::NONE;
	GameObject* gameObject = nullptr;
	bool isTrigger = false;
};

struct CollisionData
{
	Collider* collider;   // The collider component itself
	Behaviour* behaviour; // The callbacks
	vec2 p1, p2, p3, p4;  // Transformed bounding box points
};

class ModuleCollision : public Module
{
	friend class ModuleRender;
public:

	// ModuleCollision public methods
	Collider* AddCollider(ColliderType type, GameObject *parent);
	void RemoveCollider(Collider * collider);


private:

	// Module virtual methods
	virtual bool Update() override;

	uint32    m_CollidersCount = 0;
	Collider  m_Colliders[MAX_COLLIDERS];
	CollisionData m_ActiveColliders[MAX_COLLIDERS];
};

#endif //_MODULE_COLLISION_H_