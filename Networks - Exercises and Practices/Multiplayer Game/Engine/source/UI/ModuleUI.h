#ifndef _MODULE_UI_H_
#define _MODULE_UI_H_

class ModuleUI : public Module
{
public:

	// Virtual functions
	virtual bool Init() override;
	virtual bool PreUpdate() override;
	virtual bool DrawGUI() override;
	virtual bool PostUpdate() override;
	virtual bool CleanUp() override;


	// Public methods
	LRESULT HandleWindowsEvents(UINT msg, WPARAM wParam, LPARAM lParam);
	
	void SetInputsEnabled(bool enabled)	{ m_InputsEnabled = enabled; }
	bool GetInputsEnables() const		{ return m_InputsEnabled; }


private:

	bool m_InputsEnabled = true;
};


#endif //_MODULE_UI_H_