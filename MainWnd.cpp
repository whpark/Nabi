#include "pch.h"
#include"wxDesktopApp.h"
#include "MainWnd.h"
#include "MatView/MatView.h"

#include "opencv2/cudaimgproc.hpp"

xMainWnd::xMainWnd( wxWindow* parent ) : ui::IMainWnd( parent ) {
	auto& app = wxGetApp();
	wxString str;
	app.m_reg.QueryValue(L"LastImage", str);
	if (!str.empty()) {
		std::filesystem::path path(str.ToStdWstring());
		if (std::filesystem::is_regular_file(path))
			path = path.parent_path();
		base_t::m_dir->SetPath(path.wstring());
	}

	m_view->m_fnSyncSetting = [this](bool bStore, std::string_view cookie, xMatView::S_OPTION& option) -> bool {
		auto& app = wxGetApp();
		std::filesystem::path path = app.m_paths.GetConfigDir().ToStdWstring();
		path /= u8"MatView.json";
		try {
			if (bStore) {
				nlohmann::json j;
				option.ToJson(j);
				std::ofstream f(path, std::ios_base::binary);
				f << std::setw(4) << j;
			} else {
				nlohmann::json j;
				std::ifstream f(path, std::ios_base::binary);
				f >> j;
				option.FromJson(j);
			}
		} catch (...) {
			return false;
		}
		return true;
	};
	m_view->LoadOption();
}

void xMainWnd::OnButtonClick_Go(wxCommandEvent& event) {
	auto strPath = base_t::m_dirPicker->GetPath();
	base_t::m_dir->SetPath(strPath);
}

void xMainWnd::OnDirctrlSelectionChanged(wxCommandEvent& event) {
	wxBusyCursor wc;
	auto str = base_t::m_dir->GetPath().ToStdWstring();
	std::filesystem::path path(str);
	if (!std::filesystem::is_regular_file(path))
		return;
	auto img = gtl::LoadImageMat(path);
	if (img.empty())
		return;
	if (img.channels() == 3) {
		cv::cvtColor(img, img, cv::ColorConversionCodes::COLOR_BGR2RGB);
	} else if (img.channels() == 4) {
		cv::cvtColor(img, img, cv::ColorConversionCodes::COLOR_BGRA2RGBA);
	}
	wxGetApp().m_reg.SetValue(L"LastImage", path.wstring());
	base_t::m_view->SetImage(img);
}
