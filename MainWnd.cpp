#include "pch.h"
#include"wxDesktopApp.h"
#include "MainWnd.h"
#include "MatView/MatView.h"

xMainWnd::xMainWnd( wxWindow* parent ) : ui::IMainWnd( parent ) {
	wxString str;
	wxGetApp().m_reg.QueryValue(L"LastImage", str);
	if (!str.empty()) {
		std::filesystem::path path(str.ToStdWstring());
		if (std::filesystem::is_regular_file(path))
			path = path.parent_path();
		base_t::m_dir->SetPath(path.wstring());
	}
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
	wxGetApp().m_reg.SetValue(L"LastImage", path.wstring());
	base_t::m_view->SetImage(img);
}
