#ifndef _MODULE_NETWORKINGSERVER_
#define _MODULE_NETWORKINGSERVER_

#include "ModuleNetworking.h"


class ModuleNetworkingServer : public ModuleNetworking
{
public:

	// ModuleNetworkingServer public methods
	void SetListenPort(int port) { m_ListenPort = port; }

	// Spawning network objects
	GameObject* SpawnPlayer(uint8 spaceshipType, vec2 initialPosition, float initialAngle);


private:

	// ModuleNetworking virtual methods
	bool IsServer() const override { return true; }

	virtual void OnStart() override;
	virtual void OnUpdate() override;
	virtual void OnGUI() override;
	virtual void OnDisconnect() override;
	
	virtual void OnPacketReceived(const InputMemoryStream& packet, const sockaddr_in& fromAddress) override;
	virtual void OnConnectionReset(const sockaddr_in& fromAddress) override;


private:

	// Updating / destroying network objects
	GameObject* InstantiateNetworkObject();
	friend GameObject* (NetworkInstantiate)();

	void UpdateNetworkObject(GameObject* gameObject);
	friend void (NetworkUpdate)(GameObject*);

	void DestroyNetworkObject(GameObject* gameObject);
	void DestroyNetworkObject(GameObject* gameObject, float delaySeconds);
	friend void (NetworkDestroy)(GameObject*);
	friend void (NetworkDestroy)(GameObject*, float delaySeconds);


private:

	struct DelayedDestroyEntry
	{
		float delaySeconds = 0.0f;
		GameObject* object = nullptr;
	};

	DelayedDestroyEntry m_NetGameObjectsToDestroyWithDelay[MAX_GAME_OBJECTS] = {};

	// State
	enum class ServerState
	{
		STOPPED,
		LISTENING
	};

	ServerState m_State = ServerState::STOPPED;
	uint16 m_ListenPort = 0;

	// TODO(you): UDP virtual connection lab session

private:

	// Client proxies
	uint32 m_NextClientId = 0;

	struct ClientProxy
	{
		bool connected = false;
		sockaddr_in address;
		uint32 clientId;
		std::string name;
		GameObject *gameObject = nullptr;

		// TODO(you): UDP virtual connection lab session
		float lastPing = 0.0f;
		// TODO(you): World state replication lab session
		ReplicationManagerServer repServer;
		// TODO(you): Reliability on top of UDP lab session
		uint32 m_LastSequenceNumProcessed = 0;
		DeliveryManager delManager;
		ServerDelegate* serverDelegate = nullptr;

		uint32 nextExpectedInputSequenceNumber = 0;
		InputController gamepad;
	};


	ClientProxy m_ClientProxies[MAX_CLIENTS];
	float m_LastPingSent = 0.0f;
	float m_LastRepSent = 0.0f;

private:

	ClientProxy* CreateClientProxy();
	ClientProxy* GetClientProxy(const sockaddr_in& clientAddress);
    void DestroyClientProxy(ClientProxy* clientProxy);
};



// NOTE(jesus): It creates a game object into the network. Use
// this method instead of Instantiate() to create network objects.
// It makes sure the object creation is replicated over the network.
GameObject * NetworkInstantiate();

// NOTE(jesus): It marks an object for replication update.
void NetworkUpdate(GameObject* gameObject);

// NOTE(jesus): For network objects, use this version instead of
// the default Destroy(GameObject *gameObject) one. This one makes
// sure to notify the destruction of the object to all connected
// machines.
void NetworkDestroy(GameObject* gameObject);
void NetworkDestroy(GameObject* gameObject, float delaySeconds);

#endif //_MODULE_NETWORKINGSERVER_