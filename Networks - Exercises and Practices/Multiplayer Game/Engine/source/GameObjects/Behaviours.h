#ifndef _BEHAVIOURS_H_
#define _BEHAVIOURS_H_

enum class BehaviourType : uint8;

struct Behaviour
{
	// Object Variables
	GameObject *gameObject = nullptr;
	bool isServer = false;
	bool isLocalPlayer = false;

	virtual BehaviourType Type() const = 0;

	// Object Methods
	virtual void Start()	{}
	virtual void Update()	{}
	virtual void Destroy()	{}

	virtual void OnInput(const InputController &input)	{}
	virtual void OnCollisionTriggered(Collider &c1, Collider &c2) {}

	// Networks Methods
	virtual void Write(OutputMemoryStream &packet) { }
	virtual void Read(const InputMemoryStream &packet) { }
};


enum class BehaviourType : uint8
{
	NONE,
	SPACESHIP,
	LASER,
};


struct Laser : public Behaviour
{
	// Variables
	float secondsSinceCreation = 0.0f;

	BehaviourType Type() const override { return BehaviourType::LASER; }

	// Object Methods
	virtual void Start() override;
	virtual void Update() override;
};


struct Spaceship : public Behaviour
{
	// Variables
	static const uint8 MAX_HIT_POINTS = 5;
	uint8 hitPoints = MAX_HIT_POINTS;
	GameObject *lifebar = nullptr;

	BehaviourType Type() const override { return BehaviourType::SPACESHIP; }

	// Object Methods
	virtual void Start() override;
	virtual void Update() override;
	virtual void Destroy() override;
	
	virtual void OnInput(const InputController &input) override;
	virtual void OnCollisionTriggered(Collider &c1, Collider &c2) override;
	
	// Networks Methods
	virtual void Write(OutputMemoryStream &packet) override;
	virtual void Read(const InputMemoryStream &packet) override;
};

#endif //_BEHAVIOURS_H_