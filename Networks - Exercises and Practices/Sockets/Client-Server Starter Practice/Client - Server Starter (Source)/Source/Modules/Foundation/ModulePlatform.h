#pragma once

class ModulePlatform : public Module
{
public:

	// Virtual functions of Modules
	bool init() override;
	bool preUpdate() override;
	bool postUpdate() override;
	bool cleanUp() override;
};