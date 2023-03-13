#include "pch.h"
#include "MainWnd.h"
#include "MatView/MatView.h"

xMainWnd::xMainWnd( wxWindow* parent ) : ui::IMainWnd( parent ) {
}

void xMainWnd::OnButtonClick_Go(wxCommandEvent& event) {
	auto strPath = base_t::m_dirPicker->GetPath();
	base_t::m_dir->SetPath(strPath);
}

void xMainWnd::OnDirctrlSelectionChanged(wxCommandEvent& event) {
	wxBusyCursor wc;
	auto str = base_t::m_dir->GetPath().ToStdWstring();
	std::filesystem::path path(str);
	auto img = gtl::LoadImageMat(path);
	base_t::m_view->SetImage(img);
}
