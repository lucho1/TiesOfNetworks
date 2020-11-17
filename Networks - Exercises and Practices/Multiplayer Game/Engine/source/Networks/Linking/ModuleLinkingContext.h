#ifndef _MODULE_LINKINGCONTEXT_H_
#define _MODULE_LINKINGCONTEXT_H_

class ModuleLinkingContext : public Module
{
public:
	
	void RegisterNetworkGameObject(GameObject *gameObject);
	void RegisterNetworkGameObjectWithNetworkId(GameObject *gameObject, uint32 networkId);
	void UnregisterNetworkGameObject(GameObject * gameObject);

	GameObject *GetNetworkGameObject(uint32 networkId, bool safeNetworkIdCheck = true);
	void GetNetworkGameObjects(GameObject *gameObjects[MAX_NETWORK_OBJECTS], uint16 *count);
	
	uint16 GetNetworkGameObjectsCount() const { return m_NetworkGameObjectsCount; }

	void Clear();


private:

	// NOTE(jesus): The networkId of a gameObject is the combination of
	// two 2-byte words: magicNumber (higher bytes) and arrayIndex
	// (lower bytes)
	// The lower bytes contain the index within the array of network
	// game objects.
	// The higher bytes contain an ever increasing number.
	// So: networkId = (0xffff0000 & (magicNumber << 16)) | (0x0000ffff & arrayIndex)
	// With this combination, with a networkId we always know the
	// position of a certain object in the array of networkdObjects,
	// and can uniquely identify objects that started existing later
	// but take the same position in the array
	uint32 MakeNetworkId(uint16 arrayIndex);
	uint16 ArrayIndexFromNetworkId(uint32 networkId);


private:

	uint16 m_NetworkGameObjectsCount = 0;
	uint32 m_NextMagicNumber = 1;
	GameObject* m_NetworkGameObjects[MAX_NETWORK_OBJECTS] = {};
};

#endif //_MODULE_LINKINGCONTEXT_H_