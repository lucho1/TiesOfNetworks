#pragma once

class Module
{
public:

	// Constructor and destructor
	Module() {}
	virtual ~Module() {}

	// Virtual functions of Modules
	virtual bool Init()			{ return true; }
	virtual bool PreUpdate()	{ return true; }
	virtual bool Update()		{ return true; }
	virtual bool GUI()			{ return true; }
	virtual bool PostUpdate()	{ return true; }
	virtual bool CleanUp()		{ return true;  }

	// For tasks
	virtual void onTaskFinished(Task *) { }
};