#include "Core/Core.h"


GameObject::GameObject()
{
	// NOTE(jesus): Game objects are automatically insterted into ModuleGameObject
	bool inserted = false;
	for (GameObject* &gameObject : App->modGameObject->gameObjects)
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


bool ModuleGameObject::init()
{
	return true;
}

bool ModuleGameObject::preUpdate()
{
	return true;
}

bool ModuleGameObject::update()
{
	return true;
}

bool ModuleGameObject::postUpdate()
{
	for (GameObject* &gameObject : gameObjects)
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

bool ModuleGameObject::cleanUp()
{
	for (GameObject* gameObject : gameObjects)
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