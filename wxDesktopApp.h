#pragma once

#include "resource.h"
#include "MainWnd.h"

class wxDesktopApp : public wxApp {
public:
    using this_t = wxDesktopApp;
    using base_t = wxApp;

public:
    wxStandardPaths m_paths{wxStandardPaths::Get()};
    stdfs::path m_pathSettings;
    wxRegKey m_reg{wxRegKey::HKCU, L"Software\\Biscuit-lab\\Nabi"};

    std::unique_ptr<xMainWnd> m_wndMain;
#ifdef _DEBUG
    std::unique_ptr<wxLogWindow> m_wndLog;
#endif

public:
    virtual ~wxDesktopApp() {
    #ifdef _DEBUG
        m_wndLog.release();
    #endif
        m_wndMain.release();
    }

    virtual bool OnInit() override;

    bool LoadSettings();
    bool SaveSettings();
};

wxDECLARE_APP(wxDesktopApp);
