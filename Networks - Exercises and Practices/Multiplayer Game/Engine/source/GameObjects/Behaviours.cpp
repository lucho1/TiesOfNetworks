#include "Core/Core.h"
#include "Behaviours.h"


// --- Laser ---
void Laser::Start()
{
	gameObject->networkInterpolationEnabled = false;
	App->modSound->PlayAudioClip(App->modResources->audioClipLaser);
}

void Laser::Update()
{
	secondsSinceCreation += Time.deltaTime;
	const float pixelsPerSecond = 1000.0f;
	gameObject->position += DegreesToVec2(gameObject->angle) * pixelsPerSecond * Time.deltaTime;

	if (isServer)
	{
		const float neutralTimeSeconds = 0.1f;
		if (secondsSinceCreation > neutralTimeSeconds && gameObject->collider == nullptr)
			gameObject->collider = App->modCollision->AddCollider(ColliderType::LASER, gameObject);

		const float lifetimeSeconds = 2.0f;
		if (secondsSinceCreation >= lifetimeSeconds)
			NetworkDestroy(gameObject);
	}
}


// --- Spaceship ---
void Spaceship::Start()
{
	gameObject->tag = (uint32)(Random.next() * UINT_MAX);

	lifebar = Instantiate();
	lifebar->sprite = App->modRender->AddSprite(lifebar);
	lifebar->sprite->pivot = vec2{ 0.0f, 0.5f };
	lifebar->sprite->order = 5;
}

void Spaceship::OnInput(const InputController &input)
{
	if (std::abs(input.horizontalAxis) > std::numeric_limits<float>::epsilon())
	{
		const float rotateSpeed = 180.0f;
		gameObject->angle += input.horizontalAxis * rotateSpeed * Time.deltaTime;

		if (isServer)
			NetworkUpdate(gameObject);
	}

	if (input.actionDown == ButtonState::PRESSED)
	{
		const float advanceSpeed = 200.0f;
		gameObject->position += DegreesToVec2(gameObject->angle) * advanceSpeed * Time.deltaTime;

		if (isServer)
			NetworkUpdate(gameObject);
	}

	if (input.actionLeft == ButtonState::PRESS)
	{
		if (isServer)
		{
			GameObject *laser = NetworkInstantiate();

			laser->position = gameObject->position;
			laser->angle = gameObject->angle;
			laser->size = { 20, 60 };

			laser->sprite = App->modRender->AddSprite(laser);
			laser->sprite->order = 3;
			laser->sprite->texture = App->modResources->laser;

			Laser *laserBehaviour = App->modBehaviour->AddLaser(laser);
			laserBehaviour->isServer = isServer;

			laser->tag = gameObject->tag;
		}
	}
}

void Spaceship::Update()
{
	static const vec4 colorAlive = vec4{ 0.2f, 1.0f, 0.1f, 0.5f };
	static const vec4 colorDead = vec4{ 1.0f, 0.2f, 0.1f, 0.5f };
	const float lifeRatio = max(0.01f, (float)(hitPoints) / (MAX_HIT_POINTS));

	lifebar->position = gameObject->position + vec2{ -50.0f, -50.0f };
	lifebar->size = vec2{ lifeRatio * 80.0f, 5.0f };
	lifebar->sprite->color = Lerp(colorDead, colorAlive, lifeRatio);
}

void Spaceship::Destroy()
{
	App->modGameObject->Destroy(lifebar);
}

void Spaceship::OnCollisionTriggered(Collider &c1, Collider &c2)
{
	if (c2.type == ColliderType::LASER && c2.gameObject->tag != gameObject->tag)
	{
		if (isServer)
		{
			uint32 laserTag = c2.gameObject->tag; // Just in case the tag is destroyed with the laser on next line
			NetworkDestroy(c2.gameObject); // Destroy the laser
		
			if (hitPoints > 0)
			{
				hitPoints--;
				NetworkUpdate(gameObject);
			}

			float size = 30 + 50.0f * Random.next();
			vec2 position = gameObject->position + 50.0f * vec2{Random.next() - 0.5f, Random.next() - 0.5f};
			if (hitPoints <= 0)
			{
				App->modNetServer->AddScorePlayer(laserTag);

				// Centered big explosion
				size = 250.0f + 100.0f * Random.next();
				position = gameObject->position;
				App->modNetServer->RespawnPlayer(gameObject, { 0.0f, 0.0f }, 0.0f);
				NetworkDestroy(gameObject);
			}

			GameObject *explosion = NetworkInstantiate();
			explosion->position = position;
			explosion->size = vec2{ size, size };
			explosion->angle = 365.0f * Random.next();

			explosion->sprite = App->modRender->AddSprite(explosion);
			explosion->sprite->texture = App->modResources->explosion1;
			explosion->sprite->order = 100;

			explosion->animation = App->modRender->AddAnimation(explosion);
			explosion->animation->clip = App->modResources->explosionClip;

			NetworkDestroy(explosion, 2.0f);

			// NOTE(jesus): Only played in the server right now...
			// You need to somehow make this happen in clients
			App->modSound->PlayAudioClip(App->modResources->audioClipExplosion);
		}
	}
}

void Spaceship::Write(OutputMemoryStream & packet)
{
	packet << hitPoints;
}

void Spaceship::Read(const InputMemoryStream & packet)
{
	packet >> hitPoints;
}
