#include "pch.h"

#include "gtl/qt/QCollapsibleGroupBox.h"
#include "SplitImageDlg.h"
#include "fmt/std.h"
#include "fmt/ranges.h"
#include "fmt/os.h"
#include "fmt/ostream.h"

using namespace gtl::qt;

xSplitImageDlg::xSplitImageDlg(cv::Mat img, QWidget* parent) : m_img(img), base_t(parent) {
	ui.setupUi(this);
	if constexpr (std::is_convertible_v<std::decay_t<decltype(*ui.groupInterleave)>, gtl::qt::QCollapsibleGroupBox>) {
		ui.groupInterleave->PrepareAnimation();
		if (!ui.groupInterleave->isChecked())
			ui.groupInterleave->Collapse(true);
	}

	UpdateData(false);

	connect(ui.spinWidth, &QSpinBox::valueChanged, this, &this_t::OnWidthChanged);
	connect(ui.spinHeight, &QSpinBox::valueChanged, this, &this_t::OnHeightChanged);
	connect(ui.btnBrowse, &QPushButton::clicked, this, &this_t::OnBrowse);
	connect(ui.edtPath, &QLineEdit::textChanged, this, &this_t::OnPathChanged);

	connect(ui.spinInterleave_FieldsCountX, &QSpinBox::valueChanged, this, &this_t::OnInterleaveFieldsXChanged);
	connect(ui.spinInterleave_FieldsCountY, &QSpinBox::valueChanged, this, &this_t::OnInterleaveFieldsYChanged);
	connect(ui.spinInterleave_PixelGroupingX, &QSpinBox::valueChanged, this, &this_t::OnInterleavePixelGroupingXChanged);
	connect(ui.spinInterleave_PixelGroupingY, &QSpinBox::valueChanged, this, &this_t::OnInterleavePixelGroupingYChanged);

	connect(ui.btnOK, &QPushButton::clicked, this, [this]{ 
		if (UpdateData(true)) 
			accept(); });
	connect(ui.btnCancel, &QPushButton::clicked, this, [this]{ reject(); });
}

xSplitImageDlg::~xSplitImageDlg() {
}

bool xSplitImageDlg::UpdateData(bool bSave) {
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

	if (bSave) {
		if (m_interleave.bUse) {
			if (!VerifyData(true))
				return false;
		}
	}

	return true;
}

bool xSplitImageDlg::VerifyData(bool bUpdateUI) {
	struct sSpinItem {
		std::string cr;
		QSpinBox* spin{};
	};
	enum class eITEM { FX, FY, PX, PY};
	std::map<eITEM, sSpinItem> items {
		{eITEM::FX, {"", ui.spinInterleave_FieldsCountX}},
		{eITEM::FY, {"", ui.spinInterleave_FieldsCountY}},
		{eITEM::PX, {"", ui.spinInterleave_PixelGroupingX}},
		{eITEM::PY, {"", ui.spinInterleave_PixelGroupingY}},
	};

	bool bOK{true};

	int nPageX = GetPageX();
	int nPageY = GetPageY();
	if (m_interleave.sizeFields.cx <= 0)
		m_interleave.sizeFields.cx = 1;
	if (m_interleave.sizeFields.cy <= 0)
		m_interleave.sizeFields.cy = 1;

	items[eITEM::PX].cr = items[eITEM::FX].cr = (nPageX <= 1) ? "" : "green";
	items[eITEM::PY].cr = items[eITEM::FY].cr = (nPageY <= 1) ? "" : "green";

	if (nPageX % m_interleave.sizeFields.cx) {
		items[eITEM::FX].cr = "red";
		bOK = false;
	}
	if (nPageY % m_interleave.sizeFields.cy) {
		items[eITEM::FY].cr = "red";
		bOK = false;
	}

	if (m_interleave.sizePixelGroup.cx <= 0)
		m_interleave.sizePixelGroup.cx = 1;
	if (m_interleave.sizePixelGroup.cy <= 0)
		m_interleave.sizePixelGroup.cy = 1;

	if (m_size.cx <= 0)
		m_size.cx = m_img.cols;
	if (m_size.cy <= 0)
		m_size.cy = m_img.rows;

	if ( (nPageX == 1 and m_interleave.sizePixelGroup.cx > 1)
		or (m_size.cx % m_interleave.sizePixelGroup.cx)
		)
	{
		items[eITEM::PX].cr = "red";
		bOK = false;
	}
	if ((nPageY == 1 and m_interleave.sizePixelGroup.cy > 1)
		or (m_size.cy % m_interleave.sizePixelGroup.cy)
		)
	{
		items[eITEM::PY].cr = "red";
		bOK = false;
	}

	for (auto& [key, item] : items) {
		auto& [strColor, spin] = item;
		if (strColor.empty())
			spin->setStyleSheet("");
		else
			spin->setStyleSheet(std::format("background-color: {}", strColor).c_str());
	}

	return bOK;
}

auto GetDivisors(int n) {
	std::vector<int> divisors;
	divisors.push_back(1);
	for (int i{2}, n2 = (n >= 2 ? n/2 : n); i <= n2; i++) {
		if (n % i)
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

	auto str2 = fmt::format("{}", GetDivisors(val));
	ui.txtInterleave_PixelGroupingX->setText(ToQString(str2));

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

	auto str2 = fmt::format("{}", GetDivisors(val));
	ui.txtInterleave_PixelGroupingY->setText(ToQString(str2));

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
	//if (val <= 0)
	//	val = 1;
	//int nPage = GetPageX();
	//auto strColor = (nPage % val) ? "red" : "green";
	//ui.spinInterleave_FieldsCountX->setStyleSheet(std::format("background-color: {}", strColor).c_str());

	//m_interleave.sizeFields.cx = val;
	//VerifyData(true);
	UpdateData(true);
}

void xSplitImageDlg::OnInterleaveFieldsYChanged(int val) {
	//if (val <= 0)
	//	val = 1;
	//int nPage = GetPageY();
	//auto strColor = (nPage % val) ? "red" : "green";
	//ui.spinInterleave_FieldsCountY->setStyleSheet(std::format("background-color: {}", strColor).c_str());

	//m_interleave.sizeFields.cy = val;
	//VerifyData(true);
	UpdateData(true);
}

void xSplitImageDlg::OnInterleavePixelGroupingXChanged(int val) {
	//if (val <= 0)
	//	val = 1;
	//int w = ui.spinWidth->value();
	//if (w <= 0)
	//	w = m_img.cols;
	//auto strColor = (GetPageX() == 1 and w > 1) or (GetPageX() % (ui.spinInterleave_FieldsCountX->value() * val)) ? "red" : "green";
	//ui.spinInterleave_PixelGroupingX->setStyleSheet(std::format("background-color: {}", strColor).c_str());

	//m_interleave.sizePixelGroup.cx = val;
	//VerifyData(true);
	UpdateData(true);
}

void xSplitImageDlg::OnInterleavePixelGroupingYChanged(int val) {
	//if (val <= 0)
	//	val = 1;
	//int h = ui.spinHeight->value();
	//if (h <= 0)
	//	h = m_img.rows;
	//auto strColor = (GetPageY() == 1 and h > 1) or (GetPageY() % (ui.spinInterleave_FieldsCountY->value() * val)) ? "red" : "green";
	//ui.spinInterleave_PixelGroupingY->setStyleSheet(std::format("background-color: {}", strColor).c_str());

	//m_interleave.sizePixelGroup.cy = val;
	//VerifyData(true);
	UpdateData(true);
}

