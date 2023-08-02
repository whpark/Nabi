#include "pch.h"
#include "SplitImageDlg.h"
#include "fmt/std.h"
#include "fmt/ranges.h"
#include "fmt/os.h"
#include "fmt/ostream.h"

using namespace gtl::qt;

xSplitImageDlg::xSplitImageDlg(cv::Mat img, QWidget* parent) : m_img(img), base_t(parent) {
	ui.setupUi(this);

	UpdateData(false);

	connect(ui.spinWidth, &QSpinBox::valueChanged, this, &this_t::OnWidthChanged);
	connect(ui.spinHeight, &QSpinBox::valueChanged, this, &this_t::OnHeightChanged);
	connect(ui.btnBrowse, &QPushButton::clicked, this, &this_t::OnBrowse);
	connect(ui.edtPath, &QLineEdit::textChanged, this, &this_t::OnPathChanged);

	connect(ui.spinInterleave_FieldsCountX, &QSpinBox::valueChanged, this, &this_t::OnInterleaveFieldsXChanged);
	connect(ui.spinInterleave_FieldsCountY, &QSpinBox::valueChanged, this, &this_t::OnInterleaveFieldsYChanged);
	connect(ui.spinInterleave_PixelGroupingX, &QSpinBox::valueChanged, this, &this_t::OnInterleavePixelGroupingXChanged);
	connect(ui.spinInterleave_PixelGroupingY, &QSpinBox::valueChanged, this, &this_t::OnInterleavePixelGroupingYChanged);

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

	UpdateWidgetValue(bSave, ui.groupInterleave, m_interleave.bUse);
	UpdateWidgetValue(bSave, ui.spinInterleave_FieldsCountX, m_interleave.sizeFields.cx);
	UpdateWidgetValue(bSave, ui.spinInterleave_FieldsCountY, m_interleave.sizeFields.cy);
	UpdateWidgetValue(bSave, ui.spinInterleave_PixelGroupingX, m_interleave.sizePixelGroup.cx);
	UpdateWidgetValue(bSave, ui.spinInterleave_PixelGroupingY, m_interleave.sizePixelGroup.cy);
}

auto GetDivisors(int n) {
	std::vector<int> divisors;
	divisors.push_back(1);
	for (int i{2}, n2 = (n >= 2 ? n/2 : n); i < n2; i++) {
		;if (n % i)
			continue;
		divisors.push_back(i);
	}
	divisors.push_back(n);
	return divisors;
}

void xSplitImageDlg::OnWidthChanged(int val) {
	if (val < 0)
		return;

	int nPage = GetPageX(val);
	ui.edtInfoX->setText(ToQString(std::format("{} pages", nPage)));
	auto str = nPage <= 1 ? "No Interleaving"s : fmt::format("{}", GetDivisors(nPage));
	ui.txtInterleave_FieldsCountX->setText(ToQString(str));

	OnInterleaveFieldsXChanged(ui.spinInterleave_FieldsCountX->value());
	OnInterleavePixelGroupingXChanged(ui.spinInterleave_PixelGroupingX->value());
}

void xSplitImageDlg::OnHeightChanged(int val) {
	if (val < 0)
		return;

	int nPage = GetPageY(val);
	ui.edtInfoY->setText(ToQString(std::format("{} pages", nPage)));
	auto str = nPage <= 1 ? "No Interleaving"s : fmt::format("{}", GetDivisors(nPage));
	ui.txtInterleave_FieldsCountY->setText(ToQString(str));

	OnInterleaveFieldsYChanged(ui.spinInterleave_FieldsCountY->value());
	OnInterleavePixelGroupingYChanged(ui.spinInterleave_PixelGroupingY->value());
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

void xSplitImageDlg::OnInterleaveFieldsXChanged(int val) {
	if (val <= 0)
		val = 1;
	int nPage = GetPageX();
	auto strColor = (nPage % val) ? "red" : "green";
	ui.spinInterleave_FieldsCountX->setStyleSheet(std::format("background-color: {}", strColor).c_str());
}

void xSplitImageDlg::OnInterleaveFieldsYChanged(int val) {
	if (val <= 0)
		val = 1;
	int nPage = GetPageY();
	auto strColor = (nPage % val) ? "red" : "green";
	ui.spinInterleave_FieldsCountY->setStyleSheet(std::format("background-color: {}", strColor).c_str());
}

void xSplitImageDlg::OnInterleavePixelGroupingXChanged(int val) {
}

void xSplitImageDlg::OnInterleavePixelGroupingYChanged(int val) {
}
