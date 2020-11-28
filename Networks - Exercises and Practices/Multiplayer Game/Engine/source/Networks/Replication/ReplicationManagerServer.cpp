#include "Core/Core.h"
#include "ReplicationManagerServer.h"

// TODO(you): World state replication lab session
void ReplicationManagerServer::SetAction(uint32 networkID, REPLICATION_ACTION action) {
	ReplicationCommand cmd;
	cmd.net_id = networkID;
	cmd.action = action;
	m_ReplicationCommands.push_back(cmd);
}

void ReplicationManagerServer::Write(OutputMemoryStream& packet)
{
	for (auto& it : m_ReplicationCommands)
	{
		packet << it.net_id;
		packet << it.action;

		switch (it.action)
		{
			case REPLICATION_ACTION::CREATE:
			{
				GameObject* go = App->modLinkingContext->GetNetworkGameObject(it.net_id);
				packet << go->position << go->size << go->angle << go->tag;
				
				break;
			}

			case REPLICATION_ACTION::UPDATE:
			{
				GameObject* go = App->modLinkingContext->GetNetworkGameObject(it.net_id);
				packet << go->position << go->size << go->angle << go->tag;

				break;
			}
		}
	}

	m_ReplicationCommands.clear();
}