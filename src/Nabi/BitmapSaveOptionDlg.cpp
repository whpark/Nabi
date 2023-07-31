#include "pch.h"
#include "BitmapSaveOptionDlg.h"

using namespace gtl::qt;

xBitmapSaveOptionDlg::xBitmapSaveOptionDlg(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);

	UpdateData(false);

	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, [this]{ UpdateData(true); accept(); });
	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, [this]{ reject(); });
}

xBitmapSaveOptionDlg::~xBitmapSaveOptionDlg() {
}

bool xBitmapSaveOptionDlg::UpdateData(bool bSave) {
	UpdateWidgetValue(bSave, ui.cmbBPP, m_option.bpp);
	UpdateWidgetValue(bSave, ui.spinDPIx, m_option.dpi.cx);
	UpdateWidgetValue(bSave, ui.spinDPIy, m_option.dpi.cy);
	UpdateWidgetValue(bSave, ui.chkTopToBottom, m_option.bTopToBottom);
	return false;
}
