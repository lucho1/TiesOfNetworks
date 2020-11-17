#include "Core/Core.h"


// --- Virtual functions ---
bool ModuleGameObject::PreUpdate()
{
	BEGIN_TIMED_BLOCK(GOPreUpdate);

	static const GameObject::State gNextState[] =
	{
		GameObject::NON_EXISTING, // After NON_EXISTING
		GameObject::STARTING,     // After INSTANTIATE
		GameObject::UPDATING,     // After STARTING
		GameObject::UPDATING,     // After UPDATING
		GameObject::DESTROYING,   // After DESTROY
		GameObject::NON_EXISTING  // After DESTROYING
	};

	for (GameObject &gameObject : GameObjects)
		gameObject.state = gNextState[gameObject.state];

	END_TIMED_BLOCK(GOPreUpdate);
	return true;
}

bool ModuleGameObject::Update()
{
	// Delayed destructions
	for (DelayedDestroyEntry &destroyEntry : m_GameObjectsWithDelayedDestruction)
	{
		if (destroyEntry.object != nullptr)
		{
			destroyEntry.delaySeconds -= Time.deltaTime;
			if (destroyEntry.delaySeconds <= 0.0f)
			{
				Destroy(destroyEntry.object);
				destroyEntry.object = nullptr;
			}
		}
	}

	return true;
}


// --- Game Object Methods ---
GameObject* ModuleGameObject::Instantiate()
{
	for (uint32 i = 0; i < MAX_GAME_OBJECTS; ++i)
	{
		GameObject &gameObject = App->modGameObject->GameObjects[i];

		if (gameObject.state == GameObject::NON_EXISTING)
		{
			gameObject = GameObject();
			gameObject.id = i;
			gameObject.state = GameObject::INSTANTIATE;

			return &gameObject;
		}
	}

	ASSERT(0, "You need to increase MAX_GAME_OBJECTS in case this assert crashes");
	return nullptr;
}

void ModuleGameObject::Destroy(GameObject * gameObject)
{
	ASSERT(gameObject->networkId == 0); // NOTE(jesus): If it has a network identity, it must be destroyed by the Networking module first

	static const GameObject::State gNextState[] =
	{
		GameObject::NON_EXISTING, // After NON_EXISTING
		GameObject::DESTROY,      // After INSTANTIATE
		GameObject::DESTROY,      // After STARTING
		GameObject::DESTROY,      // After UPDATING
		GameObject::DESTROY,      // After DESTROY
		GameObject::DESTROYING    // After DESTROYING
	};

	ASSERT(gameObject->state < GameObject::STATE_COUNT);
	gameObject->state = gNextState[gameObject->state];
}

void ModuleGameObject::Destroy(GameObject * gameObject, float delaySeconds)
{
	for (uint32 i = 0; i < MAX_GAME_OBJECTS; ++i)
	{
		if (App->modGameObject->m_GameObjectsWithDelayedDestruction[i].object == nullptr)
		{
			App->modGameObject->m_GameObjectsWithDelayedDestruction[i].object = gameObject;
			App->modGameObject->m_GameObjectsWithDelayedDestruction[i].delaySeconds = delaySeconds;
			break;
		}
	}
}

GameObject * Instantiate()
{
	GameObject *result = ModuleGameObject::Instantiate();
	return result;
}

void Destroy(GameObject * gameObject)
{
	ModuleGameObject::Destroy(gameObject);
}

void Destroy(GameObject * gameObject, float delaySeconds)
{
	ModuleGameObject::Destroy(gameObject, delaySeconds);
}
