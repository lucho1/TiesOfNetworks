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

	// For console messages (text color Green by default)
	inline void PrintMessageInConsole(const std::string& msg, const Color& col = { 0.2f, 1.0f, 0.2f }) { m_ChatMessages.push_back({ msg, col }); }
	inline void ClearConsoleMessages() { m_ChatMessages.clear(); }


	// Public methods
	LRESULT HandleWindowsEvents(UINT msg, WPARAM wParam, LPARAM lParam);
	
	void SetInputsEnabled(bool enabled)	{ m_InputsEnabled = enabled; }
	bool GetInputsEnables() const		{ return m_InputsEnabled; }


private:

	std::vector<std::pair<std::string, const Color>> m_ChatMessages;
	bool m_InputsEnabled = true;
};


#endif //_MODULE_UI_H_