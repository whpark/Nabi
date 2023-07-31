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
	if (auto r = m_reg.value("WindowPositions/splitter"); !r.isNull()) {
		ui.splitter->restoreState(r.toByteArray());
	}
	if (auto* left = ui.splitter->widget(0)) {
		left->setMaximumWidth(800);
	}

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
	connect(ui.btnAbout, &QPushButton::clicked, this, [this](auto) { xAboutDlg dlg(this); dlg.exec(); });
	connect(ui.folder, &QTreeViewEx::selChanged, this, &this_t::OnFolder_SelChanged);
	connect(ui.edtPath, &QLineEdit::returnPressed, this, &this_t::OnImage_Load);
	connect(ui.btnLoad, &QPushButton::clicked, this, &this_t::OnImage_Load);
	connect(ui.btnSave, &QPushButton::clicked, this, &this_t::OnImage_Save);

	connect(ui.btnRotateLeft, &QPushButton::clicked, this, &this_t::OnImage_RotateLeft);
	connect(ui.btnRotateRight, &QPushButton::clicked, this, &this_t::OnImage_RotateRight);
	connect(ui.btnRotate180, &QPushButton::clicked, this, &this_t::OnImage_Rotate180);
	connect(ui.btnFlipLR, &QPushButton::clicked, this, &this_t::OnImage_FlipLR);
	connect(ui.btnFlipUD, &QPushButton::clicked, this, &this_t::OnImage_FlipUD);
}

xMainWnd::~xMainWnd() {
	if (auto r = ui.splitter->saveState(); r.size()) {
		m_reg.setValue("WindowPositions/splitter", r);
	}
	SaveWindowPosition(m_reg, "MainWnd", this);
}

bool xMainWnd::ShowImage(std::filesystem::path const& path) {
	if (!gtl::IsImageExtension(path))
		return false;

	xWaitCursor wc;
	cv::Mat img;
	bool bLoadBitmapMatTRIED{};
	std::string infoBMP;
	if (path.extension() == L".bmp") {
		auto [result, fileHeader, header] = gtl::LoadBitmapHeader(path);
		if (result) {
			auto bh = std::visit([](auto& arg) { return (gtl::BITMAP_HEADER&)arg; }, header);
			infoBMP = std::format("BPP({}), dpi({}, {})", bh.nBPP, gtl::Round(bh.XPelsPerMeter * 25.4 / 1000), gtl::Round(bh.YPelsPerMeter * 25.4/ 1000));
			//auto w = bh.width;
			//auto h = bh.height;
			//if (w < 0) w = -w;
			//if (h < 0) h = -h;
			if (bh.nBPP <= 8 and bh.compression == 0 and bh.planes == 1/*(uint64_t)w * h > 32767 * 32767*/) {
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
	}
	else if (img.channels() == 4) {
		cv::cvtColor(img, img, cv::ColorConversionCodes::COLOR_BGRA2RGBA);
	}
	auto str = ToQString(path);
	m_reg.setValue(L"misc/LastImage", ToQString(path));
	m_img = img;
	ui.view->SetImage(img, true, xMatView::eZOOM::fit2window);
	ui.edtPath->setText(ToQString(path));
	auto info = std::format("Size({}, {}) {}", img.cols, img.rows, infoBMP);
	ui.edtImageInfo->setText(ToQString(info));
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
	cv::Mat img0 = m_img;
	if (img0.empty())
		return;
	cv::Mat img;
	if (img0.channels() == 3) {
		cv::cvtColor(img0, img, cv::COLOR_BGR2RGB);
	}
	else {
		img = img0;
	}
	QString strFolder;
	if (auto index = ui.folder->currentIndex(); index.isValid()) {
		strFolder = m_modelFileSystem.filePath(index);
		std::filesystem::path path = strFolder.toStdString();
		if (std::filesystem::is_regular_file(path)) {
			path = path.parent_path();
			strFolder = ToQString(path.wstring());
		}
	}
	QString strPath = QFileDialog::getSaveFileName(this, "Save Image", strFolder, "Image Files (*.bmp *.jpg *.jpeg *.png *.tif *.tiff *.gif);;All Files(*.*)");
	std::filesystem::path path = ToWString(strPath);
	if (path.empty())
		return;
	auto ext = path.extension().string();
	gtl::MakeLower(ext);
	if ( (ext == ".bmp") and (img.channels() == 1) ) {
		xSaveOptionDlg dlg(this);
		if (auto r = dlg.exec(); r != QDialog::Accepted)
			return;

		xWaitCursor wc;

		// Palette
		static std::vector<gtl::color_bgra_t> palette_1bit {
			gtl::color_bgra_t{  0,   0,   0},	// 0
			gtl::color_bgra_t{255, 255, 255},	// 1
		};
		static std::vector<gtl::color_bgra_t> palette_4bit {
			gtl::color_bgra_t{255, 255, 255},	// 0
			gtl::color_bgra_t{  0,   0,   0},	// 1
			gtl::color_bgra_t{120, 250,  70},	// 2
			gtl::color_bgra_t{ 80,  50, 230},	// 3
			gtl::color_bgra_t{ 80, 180, 250},	// 4
			gtl::color_bgra_t{225, 110,  10},	// 5
			gtl::color_bgra_t{240,   0,   0},	// 6
			gtl::color_bgra_t{170,  50, 240},	// 7
			gtl::color_bgra_t{240,   0,  50},	// 8
			gtl::color_bgra_t{220,   0,   0},	// 9
			gtl::color_bgra_t{200,   0,   0},	// 10
			gtl::color_bgra_t{170,   0,   0},	// 11
			gtl::color_bgra_t{150,   0,   0},	// 12
			gtl::color_bgra_t{130,   0,   0},	// 13
			gtl::color_bgra_t{100,   0,   0},	// 14
			gtl::color_bgra_t{200, 100, 100},	// 15
		};
		static std::vector<gtl::color_bgra_t> palette_8bit = []{
			std::vector<gtl::color_bgra_t> vec;
			vec.reserve(256);
			for (int i{}; i < 256; i++) {
				vec.push_back(gtl::color_bgra_t{(uint8_t)i, (uint8_t)i, (uint8_t)i});
			}
			return vec;
		}();

		std::span<gtl::color_bgra_t> palette;
		switch (dlg.m_eBPP) {
		case xSaveOptionDlg::eBPP::_1: palette = palette_1bit; break;
		case xSaveOptionDlg::eBPP::_4: palette = palette_4bit; break;
		case xSaveOptionDlg::eBPP::_8: palette = palette_8bit; break;
		}

		// Save
		gtl::qt::SaveBitmapMatProgress(path, img, dlg.GetBPP(dlg.m_eBPP), dlg.GetPelsPerMeter(dlg.m_dpi), palette, false, dlg.m_bBottomToTop);
	}
	else {
		xWaitCursor wc;
		std::vector<int> params{ cv::IMWRITE_JPEG_QUALITY, 95};
		std::vector<uchar> buf;
		if (!cv::imencode(ext, img, buf, params) or !gtl::ContainerToFile(buf, path)) {
			QMessageBox::critical(this, "Error", "Failed to save image.");
			return;
		}
	}
}

void xMainWnd::OnImage_RotateLeft() {
	xWaitCursor wc;
	cv::flip(m_img.t(), m_img, 0);
	ui.view->SetImage(m_img, false);
}

void xMainWnd::OnImage_RotateRight() {
	xWaitCursor wc;
	cv::flip(m_img.t(), m_img, 1);
	ui.view->SetImage(m_img, false);
}

void xMainWnd::OnImage_Rotate180() {
	xWaitCursor wc;
	cv::flip(m_img, m_img, -1);
	ui.view->SetImage(m_img, false);
}

void xMainWnd::OnImage_FlipLR() {
	xWaitCursor wc;
	cv::flip(m_img, m_img, 1);
	ui.view->SetImage(m_img, false);
}

void xMainWnd::OnImage_FlipUD() {
	xWaitCursor wc;
	cv::flip(m_img, m_img, 0);
	ui.view->SetImage(m_img, false);
}
