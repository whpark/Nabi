#include "pch.h"
#include "SplitImageDlg.h"

using namespace gtl::qt;

xSplitImageDlg::xSplitImageDlg(cv::Mat img, QWidget* parent) : m_img(img), base_t(parent) {
	ui.setupUi(this);

	UpdateData(false);

	connect(ui.spinWidth, &QSpinBox::valueChanged, this, &this_t::OnWidthChanged);
	connect(ui.spinHeight, &QSpinBox::valueChanged, this, &this_t::OnHeightChanged);
	connect(ui.btnBrowse, &QPushButton::clicked, this, &this_t::OnBrowse);
	connect(ui.edtPath, &QLineEdit::textChanged, this, &this_t::OnPathChanged);

	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, [this]{ UpdateData(true); accept(); });
	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, [this]{ reject(); });
}

xSplitImageDlg::~xSplitImageDlg() {
}

void xSplitImageDlg::UpdateData(bool bSave) {

	UpdateWidgetValue(bSave, ui.edtPath, m_path);
	UpdateWidgetValue(bSave, ui.spinWidth, m_size.cx);
	UpdateWidgetValue(bSave, ui.spinHeight, m_size.cy);

	UpdateWidgetValue(bSave, ui.cmbBPP, m_option.bpp);
	UpdateWidgetValue(bSave, ui.spinDPIx, m_option.dpi.cx);
	UpdateWidgetValue(bSave, ui.spinDPIy, m_option.dpi.cy);
	UpdateWidgetValue(bSave, ui.chkTopToBottom, m_option.bTopToBottom);

}

void xSplitImageDlg::OnWidthChanged(int val) {
	if (val < 0)
		return;

	if (val == 0)
		ui.edtInfoH->setText("1 page");
	else {
		int n = m_img.cols / val;
		if (m_img.cols % val)
			n++;
		ui.edtInfoH->setText(ToQString(std::format("{} pages", n)));
	}
}

void xSplitImageDlg::OnHeightChanged(int val) {
	if (val < 0)
		return;

	if (val == 0)
		ui.edtInfoV->setText("1 page");
	else {
		int n = m_img.rows / val;
		if (m_img.rows % val)
			n++;
		ui.edtInfoV->setText(ToQString(std::format("{} pages", n)));
	}
}

void xSplitImageDlg::OnBrowse() {
	QString strPath = QFileDialog::getSaveFileName(this, "Save Image", ui.edtPath->text(), "Image Files (*.bmp *.jpg *.jpeg *.png *.tif *.tiff);;All Files(*.*)");
	if (strPath.isEmpty())
		return;
	ui.edtPath->setText(strPath);
}

void xSplitImageDlg::OnPathChanged() {
	bool bEnable{};
	if (m_img.channels() != 1)
		bEnable = false;
	else {
		QString str = ui.edtPath->text();
		str = str.trimmed();
		if (str.size() >= 4) {
			bEnable = str.right(4).toLower() == ".bmp";
		}
	}

	if (ui.groupBitmapSaveOption->isEnabled() != bEnable)
		ui.groupBitmapSaveOption->setEnabled(bEnable);

}
