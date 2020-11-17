#ifndef _MODULE_BEHAVIOUR_H_
#define _MODULE_BEHAVIOUR_H_

#include "Behaviours.h"

class ModuleBehaviour : public Module
{
public:

	virtual bool Update() override;

	Behaviour*	AddBehaviour(BehaviourType behaviourType, GameObject *parentGameObject);
	Spaceship*	AddSpaceship(GameObject *parentGameObject);
	Laser*		AddLaser(GameObject *parentGameObject);


private:

	void HandleBehaviourLifeCycle(Behaviour * behaviour);

	Spaceship m_Spaceships[MAX_CLIENTS];
	Laser m_Lasers[MAX_GAME_OBJECTS];
};

#endif //_MODULE_BEHAVIOUR_H_