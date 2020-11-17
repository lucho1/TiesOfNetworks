#include "Core/Core.h"
#include "ModuleBehaviour.h"

bool ModuleBehaviour::Update()
{
	for (Spaceship &behaviour : m_Spaceships)
		HandleBehaviourLifeCycle(&behaviour);
	
	for (Laser &behaviour : m_Lasers)
		HandleBehaviourLifeCycle(&behaviour);

	return true;
}

Behaviour *ModuleBehaviour::AddBehaviour(BehaviourType behaviourType, GameObject *parentGameObject)
{
	switch (behaviourType)
	{
		case BehaviourType::SPACESHIP:	return AddSpaceship(parentGameObject);
		case BehaviourType::LASER:		return AddLaser(parentGameObject);
		default:						return nullptr;
	}
}

Spaceship *ModuleBehaviour::AddSpaceship(GameObject *parentGameObject)
{
	for (Spaceship &behaviour : m_Spaceships)
	{
		if (behaviour.gameObject == nullptr)
		{
			behaviour = {};
			behaviour.gameObject = parentGameObject;
			parentGameObject->behaviour = &behaviour;

			return &behaviour;
		}
	}

	ASSERT(false, "Couldn't find any place in the Pool to push a new Spaceship");
	return nullptr;
}

Laser *ModuleBehaviour::AddLaser(GameObject *parentGameObject)
{
	for (Laser &behaviour : m_Lasers)
	{
		if (behaviour.gameObject == nullptr)
		{
			behaviour = {};
			behaviour.gameObject = parentGameObject;
			parentGameObject->behaviour = &behaviour;
			return &behaviour;
		}
	}

	ASSERT(false);
	return nullptr;
}

void ModuleBehaviour::HandleBehaviourLifeCycle(Behaviour *behaviour)
{
	GameObject *gameObject = behaviour->gameObject;
	if (gameObject != nullptr)
	{
		switch (gameObject->state)
		{
			case GameObject::STARTING:
				behaviour->Start();
				break;

			case GameObject::UPDATING:
				behaviour->Update();
				break;

			case GameObject::DESTROYING:
				behaviour->Destroy();
				gameObject->behaviour = nullptr;
				behaviour->gameObject = nullptr;
				break;

			default:
				break;
		}
	}
}
