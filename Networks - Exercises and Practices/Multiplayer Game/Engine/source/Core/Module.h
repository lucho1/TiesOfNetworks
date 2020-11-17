#ifndef _MODULE_H_
#define _MODULE_H_

class Module
{
private:

	bool m_Enabled;
	bool m_NextEnabled;


public:

	// Constructor and destructor
	Module(bool startEnabled = true) : m_Enabled(false), m_NextEnabled(startEnabled) {}
	virtual ~Module() {}


	// Virtual functions
	virtual bool Init()			{ return true; }
	virtual bool Start()		{ return true; }
	
	virtual bool PreUpdate()	{ return true; }
	virtual bool Update()		{ return true; }
	virtual bool PostUpdate()	{ return true; }
	
	virtual bool Stop()			{ return true; }
	virtual bool CleanUp()		{ return true;  }

	virtual bool DrawGUI()			{ return true; }


	// Enable disable modules
	void Enable(bool pEnabled)	{ m_NextEnabled = pEnabled; }
	void UpdateEnabledState()	{ m_Enabled = m_NextEnabled; }
	bool IsEnabled() const		{ return m_Enabled; }
	
	bool NeedsStart() const		{ return !m_Enabled && m_NextEnabled; }
	bool NeedsStop() const		{ return m_Enabled && !m_NextEnabled; }


	// For tasks
	virtual void OnTaskFinished(Task *) {}
};

#endif //_MODULE_H_