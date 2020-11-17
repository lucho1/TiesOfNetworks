#ifndef _MODULE_PLATFORM_H_
#define _MODULE_PLATFORM_H_

class ModulePlatform : public Module
{
public:

	// Virtual functions
	virtual bool Init() override;
	virtual bool PreUpdate() override;
	virtual bool PostUpdate() override;
	virtual bool CleanUp() override;
};

#endif //_MODULE_PLATFORM_H_