#include "Core/Core.h"
#include "ReplicationManagerClient.h"

// TODO(you): World state replication lab session

void ReplicationManagerClient::Read(const InputMemoryStream& packet) {

	while (packet.RemainingByteCount() != 0) {

		uint net_id;
		REPLICATION_ACTION rep_action;
		packet >> net_id >> rep_action;

		switch (rep_action) {
		case REPLICATION_ACTION::CREATE:
		{
			GameObject* go = App->modLinkingContext->GetNetworkGameObject(net_id, false);
			if (go) {
				App->modLinkingContext->UnregisterNetworkGameObject(go);
				Destroy(go);
				go = nullptr;
			}
			go = Instantiate();
			App->modLinkingContext->RegisterNetworkGameObjectWithNetworkId(go, net_id);
			packet >> go->position >> go->size >> go->angle >> go->tag;

			break;
		} //REPLICATION_ACTION::CREATE
		case REPLICATION_ACTION::UPDATE:
		{
			GameObject* go = App->modLinkingContext->GetNetworkGameObject(net_id);
			packet >> go->position >> go->size >> go->angle >> go->tag;

			break;
		} //REPLICATION_ACTION::UPDATE
		case REPLICATION_ACTION::DESTROY:
		{
			GameObject* go = App->modLinkingContext->GetNetworkGameObject(net_id, false);
			if (go) {
				App->modLinkingContext->UnregisterNetworkGameObject(go);
				Destroy(go);
			}

			break;
		} //REPLICATION_ACTION::DESTROY

		} //switch (rep_action)
	}
}
