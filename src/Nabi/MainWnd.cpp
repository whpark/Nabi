#include "pch.h"

#include "gtl/gtl.h"
#include "gtl/qt/qt.h"
#include "gtl/qt/util.h"
#include "gtl/qt/MatView/MatView.h"
#include "gtl/qt/MatBitmapArchive.h"

#include "MainWnd.h"
#include "AboutDlg.h"
#include "SaveOptionDlg.h"

using namespace gtl::qt;

xMainWnd::xMainWnd(QWidget *parent) : base_t(parent) {
    ui.setupUi(this);

	// Application Icon
	QIcon* icon = new QIcon(":/image/icon.png");
	setWindowIcon(*icon);

	// Window Position
	LoadWindowPosition(m_reg, "MainWnd", this);

	// Folder View
	ui.folder->setModel(&m_modelFileSystem);
	m_modelFileSystem.setRootPath("");
	ui.folder->setRootIndex(m_modelFileSystem.index(""));
	auto strPath = m_reg.value(L"misc/LastImage").toString();
	if (!strPath.isEmpty()) {
		auto index = m_modelFileSystem.index(strPath).parent();
		ui.folder->setCurrentIndex(index);
		ui.folder->expand(index);
	}
	ui.folder->setColumnWidth(0, 300);

	// Image View
	//bool(bool bStore, std::string_view cookie, S_OPTION&);
	ui.view->m_fnSyncSetting = [this](bool bStore, std::string_view cookie, xMatView::S_OPTION& option) -> bool {
		if (bStore) {
			std::string buffer = glz::write_json(option);
			m_reg.setValue("misc/viewOption", ToQString(buffer));
		}
		else {
			auto str = m_reg.value("misc/viewOption").toString();
			if (str.isEmpty())
				return false;
			auto buffer = ToString(str);
			glz::read_json(option, buffer);
		}
		return true;
	};
	ui.view->LoadOption();

	// Connection
	connect(ui.btnAbout, &QPushButton::clicked, this, [this](auto) { xAboutDlg dlg(this); dlg.exec();});
	connect(ui.folder, &QTreeViewEx::selChanged, this, &this_t::OnFolder_SelChanged);
	connect(ui.edtPath, &QLineEdit::returnPressed, this, &this_t::OnImage_Load);
	connect(ui.btnLoad, &QPushButton::clicked, this, &this_t::OnImage_Load);
	connect(ui.btnSave, &QPushButton::clicked, this, &this_t::OnImage_Save);

	connect(ui.btnRotateLeft, &QPushButton::clicked, this, &this_t::OnImage_RotateLeft);
	connect(ui.btnRotateRight, &QPushButton::clicked, this, &this_t::OnImage_RotateRight);
	connect(ui.btnFlipLR, &QPushButton::clicked, this, &this_t::OnImage_FlipLR);
	connect(ui.btnFlipUD, &QPushButton::clicked, this, &this_t::OnImage_FlipUD);
}

xMainWnd::~xMainWnd() {
	SaveWindowPosition(m_reg, "MainWnd", this);
}

bool xMainWnd::ShowImage(std::filesystem::path const& path) {
	if (!gtl::IsImageExtension(path))
		return false;

	xWaitCursor wc;
	cv::Mat img;
	bool bLoadBitmapMatTRIED{};
	if (path.extension() == L".bmp") {
		auto [result, fileHeader, header] = gtl::LoadBitmapHeader(path);
		if (result) {
			auto [w, h] = std::visit([](auto& arg) { return std::pair<int32_t, int32_t>(arg.width, arg.height); }, header);
			if (w < 0) w = -w;
			if (h < 0) h = -h;
			if ((uint64_t)w * h > 32767 * 32767) {
				bLoadBitmapMatTRIED = true;
				if (auto r = LoadBitmapMatProgress(path); !r.img.empty()) {
					img = r.img;
				}
			}
		}
	}
	if (img.empty())
		img = gtl::LoadImageMat(path);
	if (img.empty() and (path.extension() == L".bmp") and !bLoadBitmapMatTRIED) {
		if (auto r = LoadBitmapMatProgress(path); !r.img.empty()) {
			img = r.img;
		}
	}
	if (img.empty())
		return false;
	if (img.channels() == 3) {
		cv::cvtColor(img, img, cv::ColorConversionCodes::COLOR_BGR2RGB);
	} else if (img.channels() == 4) {
		cv::cvtColor(img, img, cv::ColorConversionCodes::COLOR_BGRA2RGBA);
	}
	auto str = ToQString(path);
	m_reg.setValue(L"misc/LastImage", ToQString(path));
	ui.view->SetImage(img, true, xMatView::eZOOM::fit2window);
	ui.edtPath->setText(ToQString(path));
	return true;
}

void xMainWnd::OnFolder_SelChanged() {
	auto index = ui.folder->currentIndex();
	std::filesystem::path path = m_modelFileSystem.filePath(index).toStdWString();
	if (path.empty())
		return;
	ShowImage(path);
}

void xMainWnd::OnImage_Load() {
	std::filesystem::path path = ui.edtPath->text().toStdWString();
	if (path.empty())
		return;
	if (auto index = m_modelFileSystem.index(ToQString(path)); index.isValid())
		ui.folder->setCurrentIndex(index);
	else
		ShowImage(path);
}

void xMainWnd::OnImage_Save() {
	auto const& img = ui.view->GetImage();
	if (img.empty())
		return;
	xSaveOptionDlg dlg(this);
	if (auto r = dlg.exec(); r != QDialog::Accepted)
		return;
}

void xMainWnd::OnImage_RotateLeft() {
}

void xMainWnd::OnImage_RotateRight() {
}

void xMainWnd::OnImage_FlipLR() {
}

void xMainWnd::OnImage_FlipUD() {
}
