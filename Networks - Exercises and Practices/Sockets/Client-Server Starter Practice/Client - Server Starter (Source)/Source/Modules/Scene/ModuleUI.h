#pragma once

class ModuleUI : public Module
{
public:

	// Virtual functions of Module
	virtual bool Init() override;
	virtual bool PreUpdate() override;
	virtual bool GUI() override;
	virtual bool PostUpdate() override;
	virtual bool CleanUp() override;

	// Text Color Green by default
	inline void PrintMessageInConsole(const std::string& msg, const Color& col = { 0.2f, 1.0f, 0.2f })	{ m_ChatMessages.push_back({ msg, col }); }
	inline void ClearConsoleMessages()																	{ m_ChatMessages.clear(); }

	// Public methods
	LRESULT HandleWindowsEvents(UINT msg, WPARAM wParam, LPARAM lParam);

private:

	std::vector<std::pair<std::string, const Color>> m_ChatMessages;
};