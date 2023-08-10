#include "pch.h"

#include "opencv2/core/utility.hpp"

#include "gtl/gtl.h"
#include "gtl/qt/qt.h"
#include "gtl/qt/util.h"
#include "gtl/qt/MatView/MatView.h"
#include "gtl/qt/MatBitmapArchive.h"

#include "App.h"
#include "MainWnd.h"
#include "AboutDlg.h"
#include "BitmapSaveOptionDlg.h"
#include "SplitImageDlg.h"

using namespace gtl::qt;

xMainWnd::xMainWnd(QWidget *parent) : base_t(parent) {

    ui.setupUi(this);

	if (s_palette_8bit.empty()) {
		auto& pal = s_palette_8bit;
		pal.reserve(256);
		pal = s_palette_4bit;
		for (size_t i = pal.size(); pal.size() < 256; i++) {
			pal.push_back(gtl::color_bgra_t{(uint8_t)i, (uint8_t)i, (uint8_t)i});
		}
	}

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

	m_dlgFindDuplicate.emplace(this);

	// Connection
	connect(ui.btnAbout, &QPushButton::clicked, this, [this](auto) { xAboutDlg dlg(this); dlg.exec(); });
	connect(ui.folder, &QTreeViewEx::selChanged, this, &this_t::OnFolder_SelChanged);
	connect(ui.edtPath, &QLineEdit::returnPressed, this, &this_t::OnImage_Load);
	connect(ui.btnLoad, &QPushButton::clicked, this, &this_t::OnImage_Load);
	connect(ui.btnSave, &QPushButton::clicked, this, &this_t::OnImage_Save);
	connect(ui.btnSplit, &QPushButton::clicked, this, &this_t::OnImage_Split);

	connect(ui.btnRotateLeft, &QPushButton::clicked, this, &this_t::OnImage_RotateLeft);
	connect(ui.btnRotateRight, &QPushButton::clicked, this, &this_t::OnImage_RotateRight);
	connect(ui.btnRotate180, &QPushButton::clicked, this, &this_t::OnImage_Rotate180);
	connect(ui.btnFlipLR, &QPushButton::clicked, this, &this_t::OnImage_FlipLR);
	connect(ui.btnFlipUD, &QPushButton::clicked, this, &this_t::OnImage_FlipUD);

	connect(ui.btnFindDuplicates, &QPushButton::clicked, this, &this_t::OnBtnFindDuplicates_Clicked);
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
	sBitmapSaveOption optionBitmap;
	if (gtl::tszicmp<char>(path.extension().string(), ".bmp"sv) == 0) {
		auto [result, fileHeader, header] = gtl::LoadBitmapHeader(path);
		if (result) {
			auto bh = std::visit([](auto& arg) { return (gtl::BITMAP_HEADER&)arg; }, header);
			infoBMP = std::format("BPP({}), dpi({}, {})", bh.nBPP, gtl::Round(bh.XPelsPerMeter * 25.4 / 1000), gtl::Round(bh.YPelsPerMeter * 25.4/ 1000));
			optionBitmap.bpp = optionBitmap.GetBPP(bh.nBPP);
			optionBitmap.dpi = optionBitmap.GetDPI({bh.XPelsPerMeter, bh.YPelsPerMeter});
			optionBitmap.bTopToBottom = bh.height > 0;	// when bh.height is zero... assume bottom to top. default is bottom to top.
			//auto w = bh.width;
			//auto h = bh.height;
			//if (w < 0) w = -w;
			//if (h < 0) h = -h;
			if ((bh.nBPP <= 8) and (bh.compression == 0) and (bh.planes == 1) /* and ((uint64_t)w * h > 32767ull * 32767)*/) {
				bLoadBitmapMatTRIED = true;
				if (auto r = LoadBitmapMatProgress(path); !r.img.empty()) {
					img = r.img;
				}
			}
		}
	}
	if (img.empty()) {
		img = gtl::LoadImageMat(path);
	}
	if (img.empty() and (gtl::tszicmp<char>(path.extension().string(), ".bmp") == 0) and !bLoadBitmapMatTRIED) {
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
	m_optionBitmap.Reset();
	m_optionBitmap = optionBitmap;
	ui.view->SetImage(img, true, xMatView::eZOOM::fit2window);
	ui.edtPath->setText(ToQString(path));
	auto info = std::format("Size({}, {}) {}", img.cols, img.rows, infoBMP);
	ui.edtImageInfo->setText(ToQString(info));
	return true;
}

bool xMainWnd::SaveImage(cv::Mat img0, std::filesystem::path const& path, sBitmapSaveOption const& option) {
	if (img0.empty() or path.empty())
		return false;
	xWaitCursor wc;
	cv::Mat img;
	if (img0.channels() == 3) {
		cv::cvtColor(img0, img, cv::COLOR_BGR2RGB);
	}
	else {
		img = img0;
	}
	auto ext = path.extension().string();
	if (ext.empty())
		ext = ".png";
	gtl::MakeLower(ext);
	if ( (ext == ".bmp") and (img.channels() == 1) ) {

		std::span<gtl::color_bgra_t> palette;
		switch (option.bpp) {
		case sBitmapSaveOption::eBPP::_1: palette = s_palette_1bit; break;
		case sBitmapSaveOption::eBPP::_4: palette = s_palette_4bit; break;
		case sBitmapSaveOption::eBPP::_8: palette = s_palette_8bit; break;
		}

		// Save
		if (!gtl::qt::SaveBitmapMatProgress(path, img, option.GetBPP(option.bpp), option.GetPelsPerMeter(option.dpi), palette, false, !option.bTopToBottom)) {
			QMessageBox::critical(this, "Error", "Failed to save image.");
			return false;
		}
	}
	else {
		std::vector<int> params{ cv::IMWRITE_JPEG_QUALITY, 95};
		std::vector<uchar> buf;
		if (!cv::imencode(ext, img, buf, params) or !gtl::ContainerToFile(buf, path)) {
			QMessageBox::critical(this, "Error", "Failed to save image.");
			return false;
		}
	}
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
	if (m_img.empty())
		return;

	QString strFolder;
	if (auto index = ui.folder->currentIndex(); index.isValid()) {
		strFolder = m_modelFileSystem.filePath(index);
		std::filesystem::path path = strFolder.toStdString();
		if (std::filesystem::is_regular_file(path)) {
			path = path.parent_path();
			strFolder = ToQString(path.wstring());
		}
	}
	QString strPath = QFileDialog::getSaveFileName(this, "Save Image", strFolder, "Image Files (*.bmp *.jpg *.jpeg *.png *.tif *.tiff);;All Files(*.*)");
	std::filesystem::path path = ToWString(strPath);

	auto option = m_optionBitmap;
	auto ext = path.extension().string();
	if (ext.empty())
		ext = ".png";
	gtl::MakeLower(ext);
	if ( (ext == ".bmp") and (m_img.channels() == 1) ) {
		xBitmapSaveOptionDlg dlg(this);
		dlg.m_option = option;
		dlg.UpdateData(false);
		if (auto r = dlg.exec(); r != QDialog::Accepted)
			return;
		option = dlg.m_option;
	}

	SaveImage(m_img, path, option);
}

void xMainWnd::OnImage_Split() {
	if (m_img.empty())
		return;

	xSplitImageDlg dlg(m_img, this);
	dlg.m_option = m_optionBitmap;
	dlg.m_path = ToWString(ui.edtPath->text());
	dlg.UpdateData(false);
	if (auto r = dlg.exec(); r != QDialog::Accepted)
		return;
	auto const& page = dlg.GetPage();
	if (page.cx <= 1 and page.cy <= 1)
		return;
	//auto const& interleave = dlg.m_interleave;
	//bool bInterleave = interleave.bUse and ( (interleave.sizeFields.cx > 1) or (interleave.sizeFields.cy > 1) );
	gtl::xSize2i size = dlg.m_size;
	if (size.cx <= 0)
		size.cx = m_img.cols;
	if (size.cy <= 0)
		size.cy = m_img.rows;

	auto option = dlg.m_option;
	int nImage{};
	for (int iy{}, y{}; y < m_img.rows; iy++, y += size.cy) {
		for (int ix{}, x{}; x < m_img.cols; ix++, x += size.cx) {
			gtl::xRect2i rect{x, y, x + size.cx, y + size.cy};
			if (rect.right > m_img.cols)
				rect.right = m_img.cols;
			if (rect.bottom > m_img.rows)
				rect.bottom = m_img.rows;
			cv::Mat imgPart((cv::Size)size, m_img.type());
			auto roi = gtl::GetSafeROI((cv::Rect)rect, m_img.size());
			m_img(roi).copyTo(imgPart(cv::Rect(0, 0, roi.width, roi.height)));

			std::filesystem::path path = dlg.m_path.parent_path();
			path /= dlg.m_path.stem().wstring();
			path += std::format(L"_X{:04d}Y{:04d}", ix+1, iy+1);
			path += dlg.m_path.extension().wstring();

			if (!SaveImage(imgPart, path, option))
				return ;
			nImage++;
		}
	}

	QMessageBox::information(this, "Done", std::format("{} images saved", nImage).c_str());

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

void xMainWnd::OnBtnFindDuplicates_Clicked() {
	if (m_dlgFindDuplicate) {
		m_dlgFindDuplicate->show();
		m_dlgFindDuplicate->setFocus();
	}
}
