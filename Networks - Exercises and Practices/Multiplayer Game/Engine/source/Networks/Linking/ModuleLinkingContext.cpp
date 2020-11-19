#include "Core/Core.h"
#include "ModuleLinkingContext.h"


void ModuleLinkingContext::RegisterNetworkGameObject(GameObject *gameObject)
{
	for (uint16 i = 0; i < MAX_NETWORK_OBJECTS; ++i)
	{
		if (m_NetworkGameObjects[i] == nullptr)
		{
			gameObject->networkId = MakeNetworkId(i);
			m_NetworkGameObjects[i] = gameObject;
			m_NetworkGameObjectsCount++;

			return;
		}
	}

	ASSERT(false, "MAX Networked Objects reached. Increase MAX_NETWORKED_OBJECTS");
}

void ModuleLinkingContext::RegisterNetworkGameObjectWithNetworkId(GameObject * gameObject, uint32 networkId)
{
	ASSERT(networkId != 0);

	uint16 arrayIndex = ArrayIndexFromNetworkId(networkId);
	ASSERT(arrayIndex < MAX_NETWORK_OBJECTS);
	ASSERT(m_NetworkGameObjects[arrayIndex] == nullptr);
	
	m_NetworkGameObjects[arrayIndex] = gameObject;
	gameObject->networkId = networkId;
	m_NetworkGameObjectsCount++;
}

GameObject * ModuleLinkingContext::GetNetworkGameObject(uint32 networkId, bool safeNetworkIdCheck)
{
	ASSERT(networkId != 0);
	uint16 arrayIndex = ArrayIndexFromNetworkId(networkId);
	ASSERT(arrayIndex < MAX_NETWORK_OBJECTS);

	GameObject *gameObject = m_NetworkGameObjects[arrayIndex];
	if (safeNetworkIdCheck)
	{
		if (gameObject != nullptr && gameObject->networkId == networkId)
			return gameObject;
		else
			return nullptr;
	}
	else
		return gameObject;
}

void ModuleLinkingContext::GetNetworkGameObjects(GameObject * gameObjects[MAX_NETWORK_OBJECTS], uint16 * count)
{
	uint16 insertIndex = 0;
	for (uint16 i = 0; i < MAX_NETWORK_OBJECTS && insertIndex < m_NetworkGameObjectsCount; ++i)
	{
		if (m_NetworkGameObjects[i] != nullptr)
		{
			ASSERT(m_NetworkGameObjects[i]->networkId != 0);
			gameObjects[insertIndex++] = m_NetworkGameObjects[i];
		}
	}

	*count = insertIndex;
}


void ModuleLinkingContext::UnregisterNetworkGameObject(GameObject *gameObject)
{
	ASSERT(gameObject != nullptr);
	
	uint16 arrayIndex = ArrayIndexFromNetworkId(gameObject->networkId);
	ASSERT(arrayIndex < MAX_NETWORK_OBJECTS);
	ASSERT(m_NetworkGameObjects[arrayIndex] == gameObject);

	m_NetworkGameObjects[arrayIndex] = nullptr;
	gameObject->networkId = 0;
	m_NetworkGameObjectsCount--;
}

void ModuleLinkingContext::Clear()
{
	for (uint16 i = 0; i < MAX_NETWORK_OBJECTS; ++i)
	{
		if (m_NetworkGameObjects[i] != nullptr)
		{
			m_NetworkGameObjects[i]->networkId = 0;
			m_NetworkGameObjects[i] = nullptr;
		}
	}

	m_NetworkGameObjectsCount = 0;
}

uint32 ModuleLinkingContext::MakeNetworkId(uint16 arrayIndex)
{
	ASSERT(arrayIndex < MAX_NETWORK_OBJECTS);

	uint32 magicNumber = m_NextMagicNumber++;
	uint32 networkId = (magicNumber << 16) | arrayIndex;
	return networkId;
}

uint16 ModuleLinkingContext::ArrayIndexFromNetworkId(uint32 networkId)
{
	uint16 arrayIndex = networkId & 0xffff;
	return arrayIndex;
}