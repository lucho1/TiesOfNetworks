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

		switch (it.action)
		{
			case REPLICATION_ACTION::CREATE:
			{
				GameObject* go = App->modLinkingContext->GetNetworkGameObject(it.net_id);

				if (go) {
					packet << it.action;
					// GO Info
					packet << go->position << go->size << go->angle << go->tag;
					// GO Behaviour
					if (go->behaviour)
						packet << go->behaviour->Type();
					else
						packet << BehaviourType::NONE;
					// GO Tex
					packet << std::string(go->sprite->texture->filename) << go->sprite->color << go->sprite->order << go->sprite->pivot;
				}
				else
					packet << REPLICATION_ACTION::NONE;	//Sergi: If we don't have a GO ourselves we don't want to create one on clients		
				break;
			}

			case REPLICATION_ACTION::UPDATE:
			{
				GameObject* go = App->modLinkingContext->GetNetworkGameObject(it.net_id);
				
				if (go) {
					packet << it.action;
					packet << go->position << go->size << go->angle << go->tag;
					if (go->behaviour)
						go->behaviour->Write(packet);
				}
				else
					packet << REPLICATION_ACTION::NONE;

				break;
			}
			default:
			{
				packet << it.action;
				break;
			}
		}
	}

	m_ReplicationCommands.clear();
}