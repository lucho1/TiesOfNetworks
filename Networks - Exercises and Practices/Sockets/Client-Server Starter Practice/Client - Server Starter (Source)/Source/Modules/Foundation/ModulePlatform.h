#pragma once

class ModulePlatform : public Module
{
public:

	// Virtual functions of Modules
	virtual bool Init() override;
	virtual bool PreUpdate() override;
	virtual bool PostUpdate() override;
	virtual bool CleanUp() override;
};