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

			//Behaviour
			BehaviourType behaviour_type;
			packet >> behaviour_type;
			go->behaviour = App->modBehaviour->AddBehaviour(behaviour_type, go);

			//Tex
			if (!go->sprite)
				go->sprite = App->modRender->AddSprite(go);
			
			// GO Tex
			std::string tex_name;
			packet >> tex_name;

			if (tex_name == "spacecraft1.png")
				go->sprite->texture = App->modResources->spacecraft1;

			else if (tex_name == "spacecraft2.png")
				go->sprite->texture = App->modResources->spacecraft2;

			else if (tex_name == "spacecraft3.png")
				go->sprite->texture = App->modResources->spacecraft3;

			else if (tex_name == "laser.png")
				go->sprite->texture = App->modResources->laser;

			else if (tex_name == "explosion1.png") {
				go->sprite->texture = App->modResources->explosion1;
				go->animation = App->modRender->AddAnimation(go);
				go->animation->clip = App->modResources->explosionClip;
				App->modSound->PlayAudioClip(App->modResources->audioClipExplosion);
			}

			packet >> go->sprite->color >> go->sprite->order >> go->sprite->pivot;

			break;
		} //REPLICATION_ACTION::CREATE
		case REPLICATION_ACTION::UPDATE:
		{
			GameObject* go = App->modLinkingContext->GetNetworkGameObject(net_id);
			go->prev_position = go->position;
			go->prev_angle = go->angle;
			
			if(App->modNetClient->enable_interpolation)
				packet >> go->next_position >> go->size >> go->next_angle >> go->tag;
			else
				packet >> go->position >> go->size >> go->angle >> go->tag;

			if (net_id != App->modNetClient->GetNetID() && App->modNetClient->enable_interpolation)
			{
				go->InterpolationTime = 0.0f;
				go->position = go->prev_position;
				go->angle = go->prev_angle;
			}

			if (go->behaviour)
				go->behaviour->Read(packet);
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
