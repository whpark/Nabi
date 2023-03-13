#include "pch.h"
#include "MatView.h"
#include "DlgMatViewOption.h"

xDlgMatViewOption::xDlgMatViewOption(xMatView::S_OPTION const& option, wxWindow* parent) : ui::IDlgMatViewOption(parent), m_option{option} {
#if defined(SK_GL)
	m_chkGLonly->Set3StateValue(m_option.bGLonly ? wxCHK_CHECKED : wxCHK_UNCHECKED);
	m_chkSkia->Set3StateValue(m_option.bSkia ? wxCHK_CHECKED : wxCHK_UNCHECKED);
#else
	m_chkGLonly->Enable(false);
	m_chkSkia->Enable(false);
#endif
	m_chkZoomLock->Set3StateValue(m_option.bZoomLock ? wxCHK_CHECKED : wxCHK_UNCHECKED);
	m_chkPanningLock->Set3StateValue(m_option.bPanningLock ? wxCHK_CHECKED : wxCHK_UNCHECKED);
	m_chkExtendedPanning->Set3StateValue(m_option.bExtendedPanning ? wxCHK_CHECKED : wxCHK_UNCHECKED);
	m_chkKeyboardNavigation->Set3StateValue(m_option.bKeyboardNavigation ? wxCHK_CHECKED : wxCHK_UNCHECKED);
	m_chkDrawPixelValue->Set3StateValue(m_option.bDrawPixelValue ? wxCHK_CHECKED : wxCHK_UNCHECKED);
	m_chkPyrImageDown->Set3StateValue(m_option.bPyrImageDown ? wxCHK_CHECKED : wxCHK_UNCHECKED);
	m_spinMouseSpeed->SetValue(m_option.dMouseSpeed);
	m_spinScrollMargin->SetValue(m_option.nScrollMargin);
	m_radioZoomInMethod->SetSelection(std::to_underlying(m_option.eZoomIn));
	m_radioZoomOutMethod->SetSelection(std::to_underlying(m_option.eZoomOut));
	auto cr = m_option.crBackground;
	m_crBackground->SetColour(wxColour(cr[0], cr[1], cr[2]));
}

void xDlgMatViewOption::OnCheckBox_Skia(wxCommandEvent& event) {
#if defined(SK_GL)
	if (m_chkSkia->Get3StateValue() != wxCHK_CHECKED)
		m_chkGLonly->Set3StateValue(wxCHK_CHECKED);
#endif
}

void xDlgMatViewOption::OnButtonClick_RestoreBackgroundColor(wxCommandEvent& event) {
	auto cr = xMatView::S_OPTION{}.crBackground;
	m_crBackground->SetColour(wxColour(cr[0], cr[1], cr[2]));
}

void xDlgMatViewOption::OnButtonClick_OK(wxCommandEvent& event) {
	// ui to m_option
#if defined(SK_GL)
	m_option.bGLonly = (m_chkGLonly->Get3StateValue() == wxCHK_CHECKED);
	m_option.bSkia = (m_chkSkia->Get3StateValue() == wxCHK_CHECKED);
#endif
	m_option.bZoomLock = (m_chkZoomLock->Get3StateValue() == wxCHK_CHECKED);
	m_option.bPanningLock = (m_chkPanningLock->Get3StateValue() == wxCHK_CHECKED);
	m_option.bExtendedPanning = (m_chkExtendedPanning->Get3StateValue() == wxCHK_CHECKED);
	m_option.bKeyboardNavigation = (m_chkKeyboardNavigation->Get3StateValue() == wxCHK_CHECKED);
	m_option.bDrawPixelValue = (m_chkDrawPixelValue->Get3StateValue() == wxCHK_CHECKED);
	m_option.bPyrImageDown = (m_chkPyrImageDown->Get3StateValue() == wxCHK_CHECKED);
	m_option.dMouseSpeed = m_spinMouseSpeed->GetValue();
	m_option.nScrollMargin = m_spinScrollMargin->GetValue();
	m_option.eZoomIn = (xMatView::eZOOM_IN)m_radioZoomInMethod->GetSelection();
	m_option.eZoomOut = (xMatView::eZOOM_OUT)m_radioZoomOutMethod->GetSelection();
	auto cr = m_crBackground->GetColour();
	m_option.crBackground[0] = cr.Red();
	m_option.crBackground[1] = cr.Green();
	m_option.crBackground[2] = cr.Blue();

	EndDialog(wxStandardID::wxID_OK);
}

void xDlgMatViewOption::OnButtonClick_Cancel(wxCommandEvent& event) {
	EndDialog(wxStandardID::wxID_CANCEL);
}
