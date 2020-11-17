#ifndef _MODULE_RENDER_H_
#define _MODULE_RENDER_H_

struct Texture;

struct Sprite
{
	GameObject *gameObject = nullptr;
	vec2 pivot = vec2{ 0.5f, 0.5f };             // NOTE(jesus): 0.5 means centered
	vec4 color = vec4{ 1.0f, 1.0f, 1.0f, 1.0f }; // NOTE(jesus): Color to tint the sprite
	Texture * texture = nullptr;                 // NOTE(jesus): Texture with the actual image
	int  order = 0;                              // NOTE(jesus): determines the drawing order
};

const uint8 MAX_ANIMATION_CLIP_FRAMES = 25;
struct AnimationClip
{
	vec4 frameRect[MAX_ANIMATION_CLIP_FRAMES];
	float frameTime = 0.1f;
	uint16 id = 0;
	uint8 frameCount = 0;
	bool loop = false;

	void addFrameRect(vec4 rect)
	{
		frameRect[frameCount++] = rect;
	}
};

struct Animation
{
	GameObject *gameObject = nullptr;
	AnimationClip *clip = nullptr;
	float elapsedTime = 0.0f;
	uint8 currentFrame = 0;

	vec4 CurrentFrameRect() const { return clip->frameRect[currentFrame]; }
	bool Finished() const { return !clip->loop && elapsedTime > clip->frameCount* clip->frameTime; }

	void Update(float deltaTime)
	{
		elapsedTime = elapsedTime + deltaTime;
		currentFrame = (uint8)min(clip->frameCount - 1, elapsedTime / clip->frameTime);

		float duration = clip->frameCount * clip->frameTime;
		while (clip->loop && elapsedTime > duration)
		{
			elapsedTime -= duration;
			currentFrame = 0;
		}
	}

	void Rewind()
	{
		elapsedTime = 0.0f;
		currentFrame = 0;
	}	
};

class ModuleRender : public Module
{
public:

	// Virtual functions
	bool Init() override;
	bool Update() override;
	bool PostUpdate() override;
	bool CleanUp() override;


	// Public methods
	void ResizeBuffers(unsigned int width, unsigned int height);
	void Present();

	Sprite *AddSprite(GameObject *parent);
	void RemoveSprite(GameObject * parent);
	
	Animation *AddAnimation(GameObject *parent);
	AnimationClip *AddAnimationClip();
	AnimationClip *GetAnimationClip(uint16 animationClipId);
	void RemoveAnimation(GameObject *parent);
	void RemoveAnimationClip(AnimationClip *clip);


	// Attributes
	vec2 cameraPosition = {};
	bool mustRenderColliders = false;


private:

	void RenderScene();

	// D3D Methods
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();


private:

	// Game Objects
	GameObject* m_OrderedGameObjects[MAX_GAME_OBJECTS] = {};
	
	// Textures & Shaders
	Texture * m_WhitePixel = nullptr;
	Texture * m_BlackPixel = nullptr;
	uint8 m_ShaderSource[Kilobytes(128)];

	// Sprites
	uint32 m_SpriteCount = 0;
	Sprite m_Sprites[MAX_GAME_OBJECTS] = {};

	// Animations
	uint32 m_AnimationCount = 0;
	Animation m_Animations[MAX_GAME_OBJECTS] = {};
	AnimationClip m_AnimationClips[MAX_ANIMATION_CLIPS] = {};
};

#endif //_MODULE_RENDER_H_