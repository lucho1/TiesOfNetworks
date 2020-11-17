#include "ModuleCollision.h"


static bool CollisionTestOverSeparatingAxis(
	vec2 a1, vec2 a2, vec2 a3, vec2 a4, // Points in box a
	vec2 b1, vec2 b2, vec2 b3, vec2 b4, // Points in box b
	vec2 axis)                          // Separating axis test
{
	float pa1 = Dot(a1, axis);
	float pa2 = Dot(a2, axis);
	float pa3 = Dot(a3, axis);
	float pa4 = Dot(a4, axis);
	float pb1 = Dot(b1, axis);
	float pb2 = Dot(b2, axis);
	float pb3 = Dot(b3, axis);
	float pb4 = Dot(b4, axis);
	float maxa = max(pa1, max(pa2, max(pa3, pa4)));
	float mina = min(pa1, min(pa2, min(pa3, pa4)));
	float maxb = max(pb1, max(pb2, max(pb3, pb4)));
	float minb = min(pb1, min(pb2, min(pb3, pb4)));
	bool separated = maxa < minb || mina > maxb;

	return !separated;
}

static bool CollisionTest(CollisionData &c1, CollisionData &c2)
{
	BEGIN_TIMED_BLOCK(CollisionTestProfile);

	bool areColliding = false;
	vec2 axes[] = { c1.p1 - c1.p2, c1.p2 - c1.p3, c2.p1 - c2.p2, c2.p2 - c2.p3 };

	for (vec2 axis : axes)
	{
		areColliding = CollisionTestOverSeparatingAxis(c1.p1, c1.p2, c1.p3, c1.p4, c2.p1, c2.p2, c2.p3, c2.p4, axis);
		if (!areColliding)
			break;
	}

	END_TIMED_BLOCK(CollisionTestProfile);
	return areColliding;
}


// --- ModuleCollision public methods ---
Collider * ModuleCollision::AddCollider(ColliderType type, GameObject * parent)
{
	ASSERT(type != ColliderType::NONE);
	ASSERT(parent->sprite != nullptr);

	for (Collider &collider : m_Colliders)
	{
		if (collider.type == ColliderType::NONE)
		{
			collider.type = type;
			collider.gameObject = parent;
			m_CollidersCount++;

			return &collider;
		}
	}

	ASSERT(0, "No space for more colliders, increase MAX_COLLIDERS");
	return nullptr;
}

void ModuleCollision::RemoveCollider(Collider *collider)
{
	if (collider->type != ColliderType::NONE)
	{
		collider->type = ColliderType::NONE;
		collider->gameObject->collider = nullptr;
		collider->gameObject = nullptr;

		m_CollidersCount--;
	}
}


// --- Module virtual methods ---
bool ModuleCollision::Update()
{
	BEGIN_TIMED_BLOCK(Collisions);
	BEGIN_TIMED_BLOCK(Collisions1);

	// Pack colliders in activeColliders contiguously
	uint32 activeCollidersCount = 0;
	for (unsigned int i = 0; i < MAX_COLLIDERS && activeCollidersCount < m_CollidersCount; ++i)
	{
		Collider *collider = &m_Colliders[i];
		if (collider->type != ColliderType::NONE)
		{
			// Handle game object destruction
			GameObject *go = collider->gameObject;
			ASSERT(go != nullptr);

			if (go->state == GameObject::DESTROYING)
			{
				App->modCollision->RemoveCollider(collider);
				continue;
			}

			if (go->state == GameObject::UPDATING)
			{
				// Precompute collision data and store it into activeColliders
				Sprite *sprite = go->sprite;
				ASSERT(sprite != nullptr, "Sprite was Nullptr!");
				vec2 size = IsZero(go->size) ? (sprite->texture ? sprite->texture->size : vec2{ 100.0f, 100.0f }) : go->size;

				mat4 aWorldMatrix =
					translation(go->position) *
					rotationZ(DegToRad(go->angle)) *
					scaling(size) *
					translation(vec2{ 0.5f, 0.5f } -sprite->pivot);

				m_ActiveColliders[activeCollidersCount].collider = collider;
				m_ActiveColliders[activeCollidersCount].p1 = vec2_cast(aWorldMatrix * vec4{ -0.5f, -0.5f, 0.0f, 1.0f });
				m_ActiveColliders[activeCollidersCount].p2 = vec2_cast(aWorldMatrix * vec4{ 0.5f, -0.5f, 0.0f, 1.0f });
				m_ActiveColliders[activeCollidersCount].p3 = vec2_cast(aWorldMatrix * vec4{ 0.5f,  0.5f, 0.0f, 1.0f });
				m_ActiveColliders[activeCollidersCount].p4 = vec2_cast(aWorldMatrix * vec4{ -0.5f,  0.5f, 0.0f, 1.0f });
				m_ActiveColliders[activeCollidersCount].behaviour = (collider->isTrigger) ? collider->gameObject->behaviour : nullptr;
				activeCollidersCount++;
			}
		}
	}

	END_TIMED_BLOCK(Collisions1);
	BEGIN_TIMED_BLOCK(Collisions2);

	// Traverse all active colliders
	for (uint32 i = 0; i < activeCollidersCount; ++i)
	{
		CollisionData &c1 = m_ActiveColliders[i];
		for (uint32 j = i + 1; j < activeCollidersCount; ++j)
		{
			CollisionData &c2 = m_ActiveColliders[j];
			if ((c1.behaviour != nullptr) || (c2.behaviour != nullptr))
			{
				if (CollisionTest(c1, c2))
				{
					if (c1.behaviour)
						c1.behaviour->OnCollisionTriggered(*c1.collider, *c2.collider);
					if (c2.behaviour)
						c2.behaviour->OnCollisionTriggered(*c2.collider, *c1.collider);
				}
			}
		}
	}

	END_TIMED_BLOCK(Collisions2);
	END_TIMED_BLOCK(Collisions);
	return true;
}