#include "Core/Core.h"


GameObject::GameObject()
{
	// NOTE(jesus): Game objects are automatically insterted into ModuleGameObject
	bool inserted = false;
	for (GameObject*& gameObject : App->modGameObject->m_GameObjects)
	{
		if (gameObject == nullptr)
		{
			gameObject = this;
			inserted = true;
			break;
		}
	}

	ASSERT(inserted, "Couldn't Insert GameObject in GameObjectsList, it was full!");
}


bool ModuleGameObject::Init()
{
	return true;
}

bool ModuleGameObject::PreUpdate()
{
	return true;
}

bool ModuleGameObject::Update()
{
	return true;
}

bool ModuleGameObject::PostUpdate()
{
	for (GameObject* &gameObject : m_GameObjects)
	{
		if (gameObject == nullptr) continue;

		if (gameObject->deleteFlag)
		{
			delete gameObject;
			gameObject = nullptr;
		}
	}

	return true;
}

bool ModuleGameObject::CleanUp()
{
	for (GameObject* gameObject : m_GameObjects)
		delete gameObject;

	return true;
}

//void ModuleGameObject::deleteGameObjectsInScene(Screen *scene)
//{
//	for (GameObject* &gameObject : gameObjects)
//	{
//		if (gameObject == nullptr) continue;
//
//		if (gameObject->scene == scene)
//		{
//			gameObject->deleteFlag = true;
//		}
//	}
//}