﻿#include "pch.h"

#include "App.h"
#include "MainWnd.h"
#include "AboutDlg.h"
#include "BitmapSaveOptionDlg.h"
#include "SplitImageDlg.h"

#include "FreeImage.h"

using namespace gtl::qt;

xMainWnd::xMainWnd(QWidget *parent) : base_t(parent) {

    ui.setupUi(this);

	ui.chkGPUEnabled->setCheckable(true);
	ui.chkGPUEnabled->setCheckState(gtl::CheckGPU(true) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

	// QCompleter for ui.edtPath
	{
		m_completer.emplace(this);
		m_modelForCompleter.emplace(this);
		m_modelForCompleter->setRootPath("");
		m_completer->setModel(&*m_modelForCompleter);
		
		m_completer->setCompletionMode(QCompleter::PopupCompletion);
		m_completer->setCaseSensitivity(Qt::CaseInsensitive);
		ui.edtPath->setCompleter(&*m_completer);
	}

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
			auto err = glz::read_json(option, buffer);
			//if (err)
			//	return false;
		}
		return true;
	};
	ui.view->LoadOption();

	ui.chkUseFreeImage->setChecked(m_reg.value("misc/useFreeImage", true).toBool());

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

	m_reg.setValue("misc/useFreeImage", ui.chkUseFreeImage->isChecked());

	xWaitCursor wc;
	cv::Mat img;

	// test FreeImage
	std::optional<sBitmapSaveOption> optionBitmap;

#ifdef _DEBUG
	auto t0 = std::chrono::steady_clock::now();
#endif

	// Use FreeImage
	while (ui.chkUseFreeImage->isChecked()) {
		auto eFileType = FreeImage_GetFIFFromFilename(path.extension().string().c_str());
	#if 0	// Progress Dialog
		size_t sizeFile = std::filesystem::file_size(path);
		if (!sizeFile)
			break;

		std::ifstream f(path, std::ios::binary);
		if (!f.is_open())
			break;

		gtl::qt::xProgressDlg dlg(this);
		dlg.m_message = std::format(L"Loading : {}", path.wstring());
		FreeImageIO io{ nullptr, nullptr, nullptr, nullptr};
		FIBITMAP* fb{};

		struct sCookie {
			std::ifstream& is;
			gtl::qt::xProgressDlg& dlg;
			size_t len;
			size_t read {};
		} cookie { .is = f, .dlg = dlg, .len = sizeFile };

		io.read_proc = [](void* buffer, unsigned size, unsigned count, fi_handle handle) -> unsigned {
			auto* p = (sCookie*)(handle);
			auto nToRead = size * count;
			p->is.read((char*)buffer, nToRead);
			p->read += nToRead;
			if (!p->dlg.UpdateProgress(p->read * 100 / p->len, false, false))
				return 0;
			return count;
		};
		io.write_proc = nullptr;
		io.tell_proc = [](fi_handle handle) -> long {
			auto* p = (sCookie*)(handle);
			return static_cast<long>(p->is.tellg());
		};
		io.seek_proc = [](fi_handle handle, long offset, int origin) -> int {
			auto* p = (sCookie*)(handle);
			p->is.seekg(offset, origin);
			return p->is.fail() ? 1 : 0;
		};
		dlg.m_rThreadWorker = std::make_unique<std::jthread>([&]() {
			fb = FreeImage_LoadFromHandle(eFileType, &io, &cookie, 0);
			dlg.m_callback(100, true, fb?false:true);
		});

		auto r = dlg.exec();

		xWaitCursor wc;
		dlg.m_rThreadWorker->join();

		if (r != QDialog::Accepted)
			return false;
	#else
		auto* fb = FreeImage_LoadU(eFileType, path.c_str(), 0);
	#endif

		if (fb) {
			gsl::final_action fa([&]{FreeImage_Unload(fb);});

			img = gtl::ConvertFI2Mat(fb).value_or(cv::Mat{});
			//if (FreeImage_GetImageType(fb) == FREE_IMAGE_TYPE::FIT_BITMAP) {
				optionBitmap.emplace();
				auto& o = *optionBitmap;
				o.bpp = o.GetBPP(FreeImage_GetBPP(fb));
				o.dpi = o.GetDPI({FreeImage_GetDotsPerMeterX(fb), FreeImage_GetDotsPerMeterY(fb)});
				o.bTopToBottom = false;
			//}
		}

		break;
	}

	if (img.empty()) {
		bool bLoadBitmapMatTRIED{};
		if (gtl::tszicmp<char>(path.extension().string(), ".bmp"sv) == 0) {
			if (img.empty()) {
				auto [result, fileHeader, header] = gtl::LoadBitmapHeader(path);
				if (result) {
					auto bh = std::visit([](auto& arg) { return (gtl::BITMAP_HEADER&)arg; }, header);
					optionBitmap.emplace();
					auto& o = *optionBitmap;
					o.bpp = o.GetBPP(bh.nBPP);
					o.dpi = o.GetDPI({bh.XPelsPerMeter, bh.YPelsPerMeter});
					o.bTopToBottom = bh.height > 0;	// when bh.height is zero... assume bottom to top. default is bottom to top.
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
	}

#ifdef _DEBUG 
	if constexpr (false) {
		auto t1 = std::chrono::steady_clock::now();
		auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
		auto msg = std::format("Size({}, {}), LoadTime({}ms)", img.cols, img.rows, ts.count());
		QMessageBox::information(this, "Info", ToQString(msg));
	}
#endif

	auto str = ToQString(path);
	m_reg.setValue(L"misc/LastImage", ToQString(path));
	m_img = img;
	m_optionBitmap = optionBitmap.value_or(sBitmapSaveOption{});	// set or reset
	ui.view->SetImage(img, true, xMatView::eZOOM::fit2window);
	ui.edtPath->setText(ToQString(path));

	auto info = std::format("Size({}, {})", img.cols, img.rows);
	if (optionBitmap) {
		auto const& o = *optionBitmap;
		info += std::format(" BPP({}), dpi({}, {})", o.GetBPP(o.bpp), o.dpi.cx, o.dpi.cy);
	}
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
