#include "pch.h"

// wxDesktopApp.cpp : Defines the entry point for the application.
//

#include "wxDesktopApp.h"

wxIMPLEMENT_APP(wxDesktopApp);

bool wxDesktopApp::OnInit() {
	m_reg.Create();

	// Startup Folder
	gtl::SetCurrentPath_ProjectFolder(stdfs::path(L"."));

	if (!base_t::OnInit())
		return false;

	LoadSettings();

	m_wndMain = std::make_unique<xMainWnd>(nullptr);
	m_wndMain->Show();

#ifdef _DEBUG
	m_wndLog = std::make_unique<wxLogWindow>(m_wndMain.get(), wxString(L"Log"));
	m_wndLog->Show(true);
#endif
	return true;
}

bool wxDesktopApp::LoadSettings() try {
	json j;
	if (stdfs::exists(m_pathSettings)) {
		std::ifstream f(m_pathSettings, std::ios_base::binary);
		f >> j;
	}
	else {
		j = R"xxx(
{
}
)xxx"_json;
		std::ofstream f(m_pathSettings, std::ios_base::binary);
		f << std::setw(4) << j;
	}

	//from_json(j, m_settings);

	return true;
} catch (...) {
	wxMessageBox(L"CANNOT Load settings.json", L"ERROR");
	return false;
}

bool wxDesktopApp::SaveSettings() try {
	json j;

	//to_json(j, m_settings);

	std::ofstream f(m_pathSettings, std::ios_base::binary);
	f << std::setw(4) << j;
	return true;
} catch (...) {
	wxMessageBox(L"CANNOT Save settings.json", L"ERROR");
	return false;
}
