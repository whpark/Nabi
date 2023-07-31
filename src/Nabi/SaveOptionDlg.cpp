#include "pch.h"
#include "SaveOptionDlg.h"

using namespace gtl::qt;

xSaveOptionDlg::xSaveOptionDlg(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);

	UpdateData(false);

	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, [this]{ UpdateData(true); accept(); });
	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, [this]{ reject(); });
}

xSaveOptionDlg::~xSaveOptionDlg() {
}

bool xSaveOptionDlg::UpdateData(bool bSave) {
	UpdateWidgetValue(bSave, ui.cmbBPP, m_eBPP);
	UpdateWidgetValue(bSave, ui.spinDPIx, m_dpi.cx);
	UpdateWidgetValue(bSave, ui.spinDPIy, m_dpi.cy);
	UpdateWidgetValue(bSave, ui.chkBottomToTop, m_bBottomToTop);
	return false;
}
