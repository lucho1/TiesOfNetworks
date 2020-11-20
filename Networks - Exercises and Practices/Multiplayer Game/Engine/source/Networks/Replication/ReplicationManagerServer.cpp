#include "Core/Core.h"
#include "ReplicationManagerServer.h"

// TODO(you): World state replication lab session
void ReplicationManagerServer::Create(uint32 networkID)
{
}

void ReplicationManagerServer::Update(uint32 networkID)
{
}

void ReplicationManagerServer::Destroy(uint32 networkID)
{
}

void ReplicationManagerServer::Write(OutputMemoryStream& packet)
{
	for (auto& it : m_ReplicationMap)
	{
		GameObject* GObj = App->modLinkingContext->GetNetworkGameObject(it.first);

		if (GObj != nullptr)
		{
			packet << it.first;
			packet << (int)it.second.action;

			switch (it.second.action)
			{
				case ReplicationAction::CREATE:
				{
					// Serialize GObj fields -- WTF?
				}
				case ReplicationAction::UPDATE:
				{
					// Serialize GObj fields -- WTF?
				}
				case ReplicationAction::DESTROY:
				{

				}
			}
		}
	}
}