#include "pch.h"

#include "App.h"
#include "MainWnd.h"
#include "AboutDlg.h"
#include "BitmapSaveOptionDlg.h"
#include "SplitImageDlg.h"
#include "palette.h"

#include "FreeImage.h"

using namespace gtl::qt;

xMainWnd::xMainWnd(QWidget *parent) : base_t(parent), m_reg(theApp->GetReg()) {

    ui.setupUi(this);

	setAcceptDrops(true);

	ui.chkGPUEnabled->setChecked(gtl::CheckGPU(false) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
	ui.chkGPUEnabled->setEnabled(false);

	// QCompleter for ui.edtPath
	m_completer.emplace(this, std::filesystem::path{}, false);
	ui.edtPath->setCompleter(&*m_completer);

	//if (s_palette_8bit.empty()) {
	//	auto& pal = s_palette_8bit;
	//	pal.reserve(256);
	//	pal = s_palette_4bit;
	//	for (size_t i = pal.size(); pal.size() < 256; i++) {
	//		pal.push_back(gtl::color_bgra_t{(uint8_t)i, (uint8_t)i, (uint8_t)i});
	//	}
	//}

	// Application Icon
	QIcon* icon = new QIcon(":/image/icon.png");
	setWindowIcon(*icon);

	// Window Position
	LoadWindowPosition(m_reg, "MainWnd", this);
	if (auto r = m_reg.value("WindowPositions/splitterMain"); !r.isNull()) {
		ui.splitterMain->restoreState(r.toByteArray());
	}
	if (auto* left = ui.splitterMain->widget(0)) {
		left->setMaximumWidth(800);
	}

	// Folder View
	m_modelFolderSystem.setFilter(QDir::NoDotAndDotDot | QDir::System | QDir::Dirs | QDir::Drives);
	m_modelFileSystem.setFilter(QDir::NoDotAndDotDot | QDir::System | QDir::Files);
	ui.folder->setModel(&m_modelFolderSystem);
	m_modelFolderSystem.setRootPath("");
	m_modelFileSystem.setRootPath("");
	ui.folder->setRootIndex(m_modelFolderSystem.index(""));
	ui.folder->setColumnWidth(0, 300);
	ui.folder->hideColumn(1);
	ui.folder->hideColumn(2);
	ui.folder->hideColumn(3);
	ui.files->setModel(&m_modelFileSystem);
	ui.files->setColumnWidth(0, 300);

	auto strPath = m_reg.value(L"misc/LastImage").toString();
	if (!strPath.isEmpty()) {
		auto index = m_modelFileSystem.index(strPath).parent();
		std::filesystem::path path = ToWString(strPath);
		bool bFolder = std::filesystem::is_directory(path);
		if (bFolder)
			theApp->m_folderCurrent = path;
		else
			theApp->m_folderCurrent = path.parent_path();

		if (auto indexFolder = m_modelFolderSystem.index(ToQString(theApp->m_folderCurrent)); indexFolder.isValid())
			ui.folder->setCurrentIndex(indexFolder);
		if (auto idxRoot = m_modelFileSystem.setRootPath(ToQString(theApp->m_folderCurrent)); idxRoot.isValid())
			ui.files->setRootIndex(idxRoot);
		if (!bFolder) {
			if (auto idx = m_modelFileSystem.index(strPath); idx.isValid()) {
				ui.files->setCurrentIndex(idx);
				OnFile_SelChanged();
			}
		}
	}
	//// Image View
	////bool(bool bStore, std::string_view cookie, S_OPTION&);
	//{
	//	// 처음 실행시에만 설정. 초기값만... 두 번째 실행부터는 레지스트리에서 읽어옴.
	//	auto option = ui.view->GetOption();
	//	option.eZoomOut = xMatView::eZOOM_OUT::nearest;
	//	ui.view->SetOption(option, false);
	//}
	ui.view->m_strCookie = "misc/viewOption";
	ui.view->m_fnSyncSetting = [this](bool bStore, std::string_view cookie, xMatView::S_OPTION& option) -> bool {
		return SyncOption<xMatView::S_OPTION>(m_reg, bStore, cookie, option);
	};
	//ui.view->m_fnSyncSetting = [this](bool bStore, std::string_view cookie, xMatView::S_OPTION& option) -> bool {
	//	if (bStore) {
	//		std::string buffer = glz::write_json(option);
	//		m_reg.setValue("misc/viewOption", ToQString(buffer));
	//	}
	//	else {
	//		auto str = m_reg.value("misc/viewOption").toString();
	//		if (str.isEmpty())
	//			return false;
	//		auto buffer = ToString(str);
	//		auto err = glz::read_json(option, buffer);
	//		//if (err)
	//		//	return false;
	//	}
	//	return true;
	//};
	ui.view->LoadOption();

	ui.chkUseFreeImage->setChecked(m_reg.value("misc/useFreeImage", true).toBool());

	//m_dlgBlendTest.emplace(this);
	//m_dlgFindDuplicate.emplace(this);

	// Connection
	connect(ui.btnAbout, &QPushButton::clicked, this, [this](auto) { xAboutDlg dlg(this); dlg.exec(); });
	connect(ui.folder->selectionModel(), &QItemSelectionModel::currentChanged, this,
		[this](auto const& current, auto const& prev) {
			std::filesystem::path path = ToWString(m_modelFolderSystem.filePath(current));
			theApp->m_folderCurrent = path;
			auto index = m_modelFileSystem.setRootPath(ToQString(path));
			ui.files->setRootIndex(index);
		}
	);
	connect(ui.files->selectionModel(), &QItemSelectionModel::currentChanged, this, &this_t::OnFile_SelChanged);
	connect(ui.edtPath, &QLineEdit::returnPressed, this, &this_t::OnImage_Load);
	connect(ui.btnLoad, &QPushButton::clicked, this, &this_t::OnImage_Load);
	connect(ui.btnSave, &QPushButton::clicked, this, &this_t::OnImage_Save);
	connect(ui.btnSplit, &QPushButton::clicked, this, &this_t::OnImage_Split);

	connect(ui.btnRotateLeft, &QPushButton::clicked, this, &this_t::OnImage_RotateLeft);
	connect(ui.btnRotateRight, &QPushButton::clicked, this, &this_t::OnImage_RotateRight);
	connect(ui.btnRotate180, &QPushButton::clicked, this, &this_t::OnImage_Rotate180);
	connect(ui.btnFlipLR, &QPushButton::clicked, this, &this_t::OnImage_FlipLR);
	connect(ui.btnFlipUD, &QPushButton::clicked, this, &this_t::OnImage_FlipUD);
	connect(ui.btnTest, &QPushButton::clicked, this, &this_t::OnImage_Test);

	connect(ui.btnBlend, &QPushButton::clicked, this, &this_t::OnBtnBlend_Clicked);
	connect(ui.btnFindDuplicates, &QPushButton::clicked, this, &this_t::OnBtnFindDuplicates_Clicked);

	connect(ui.btnPixelCount, &QPushButton::clicked, this, &this_t::OnBtnPixelCount_Clicked);

	ui.btnCvt->addAction("toPNG", this, [this](auto) { ConvertsCurrentImageTo(".png"); });
	ui.btnCvt->addAction("toBMP", this, [this](auto) { ConvertsCurrentImageTo(".bmp"); });
	ui.btnCvt->addAction("toJPG", this, [this](auto) { ConvertsCurrentImageTo(".jpg"); });

	connect(ui.btnOpenShell, &QPushButton::clicked, this, [this](auto) {
		if (auto path = m_modelFolderSystem.filePath(ui.folder->currentIndex());
			!path.isEmpty())
		QDesktopServices::openUrl(QUrl::fromLocalFile(path));
	});

	ui.btnFFT->addAction("FFT", this, [this](auto) { OnImage_FFT(false); });
	ui.btnFFT->addAction("FFT(LogScale)", this, [this](auto) { OnImage_FFT(true); });
}

xMainWnd::~xMainWnd() {
	if (auto r = ui.splitterMain->saveState(); r.size()) {
		m_reg.setValue("WindowPositions/splitterMain", r);
	}
	SaveWindowPosition(m_reg, "MainWnd", this);
}

bool xMainWnd::ShowImage(std::filesystem::path const& path) {
	std::error_code ec;
	if (!std::filesystem::is_regular_file(path, ec))
		return false;
	size_t sizeFile = std::filesystem::file_size(path, ec);
	if (!sizeFile)
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
		auto strExt = path.extension().string();
		auto eFileType = FreeImage_GetFIFFromFilename(strExt.c_str());
		if (eFileType == FIF_UNKNOWN)
			break;

		bool bUseProgressDlg = sizeFile > 1024 * 1024 * 10;

		if (bUseProgressDlg) {
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
				dlg.m_message = L"Post Processing...";
				gsl::final_action fa([&]{FreeImage_Unload(fb);});

				img = gtl::ConvertFI2Mat(fb).value_or(cv::Mat{});
				//if (FreeImage_GetImageType(fb) == FREE_IMAGE_TYPE::FIT_BITMAP) {
					optionBitmap.emplace();
					auto& o = *optionBitmap;
					o.bpp = o.GetBPP(FreeImage_GetBPP(fb));
					o.dpi = o.GetDPI({FreeImage_GetDotsPerMeterX(fb), FreeImage_GetDotsPerMeterY(fb)});
					o.bTopToBottom = false;
				//}

				dlg.UpdateProgress(100, true, fb?false:true);
			});

			auto r = dlg.exec();

			xWaitCursor wc;
			dlg.m_rThreadWorker->join();

			if (r != QDialog::Accepted)
				return false;
		}
		else {
			auto* fb = FreeImage_LoadU(eFileType, path.c_str(), 0);
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
			if (!gtl::IsImageExtension(path))
				return false;
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
	//ui.view->SetImage(img, true, xMatView::eZOOM::fit2window);
	ui.view->SetImage(img, false);
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
	else if (img0.channels() == 4) {
		cv::cvtColor(img0, img, cv::COLOR_BGRA2RGBA);
	}
	else {
		img = img0;
	}
	auto ext = path.extension().string();
	if (ext.empty())
		ext = ".png";
	gtl::MakeLower(ext);
	if ( (ext == ".bmp") and (img.channels() == 1) ) {

		std::span<gtl::color_bgra_t const> palette;
		switch (option.bpp) {
		case sBitmapSaveOption::eBPP::_1: palette = s_palette_1bit_bw; break;
		case sBitmapSaveOption::eBPP::_4: palette = s_palette_4bit_grayscale; break;
		case sBitmapSaveOption::eBPP::_8: palette = s_palette_8bit_grayscale; break;
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

bool xMainWnd::ConvertsCurrentImageTo(std::string const& ext) {
	auto index = ui.files->currentIndex();
	if (!index.isValid())
		return false;
	std::filesystem::path path = m_modelFileSystem.filePath(index).toStdWString();
	if (!exists(path))
		return false;
	cv::Mat img = m_img;
	//path += ext;
	path.replace_extension(ext);
	std::vector<uchar> buf;
	std::vector<int> params{cv::IMWRITE_JPEG_QUALITY, 95};
	if (!cv::imencode(ext, m_img, buf, params))
		return false;
	return gtl::ContainerToFile(buf, path);
}

cv::Mat xMainWnd::FFT(cv::Mat const& m, bool bLogScale) {
	// Expand the image to an optimal size for DFT performance (powers of 2, 3, and 5 are efficient)
	int rows = cv::getOptimalDFTSize(m.rows);
	int cols = cv::getOptimalDFTSize(m.cols);
	cv::Mat padded;
	cv::copyMakeBorder(m, padded, 0, rows - m.rows, 0, cols - m.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

	// Prepare the complex matrix (real and imaginary parts)
	cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
	cv::Mat complexI;
	cv::merge(planes, 2, complexI);

	// Compute the 2D DFT
	cv::dft(complexI, complexI, cv::DFT_COMPLEX_OUTPUT);

	// Compute the magnitude spectrum
	cv::split(complexI, planes); // planes[0] = Re(DFT(m)), planes[1] = Im(DFT(m))
	cv::magnitude(planes[0], planes[1], planes[0]);
	cv::Mat magI = planes[0];

	// Shift the zero-frequency component to the center of the spectrum
	// (for visualization purposes, as the origin is at the top-left by default)
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;
	cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));   // Top-Left
	cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));  // Top-Right
	cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));  // Bottom-Left
	cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

	cv::Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	// Transform the magnitude to a logarithmic scale for better visualization
	// (high and low frequencies can vary significantly)
	if (bLogScale) {
		magI += cv::Scalar::all(1); // switch to logarithmic scale
		cv::log(magI, magI);
	}
	// Normalize the spectrum for display (values between 0 and 1)
	auto org = std::exchange(magI.at<float>(cy, cx), 0.0f);	// DC component (zero-frequency) is set to zero for better visualization
	cv::normalize(magI, magI, 0, 1, cv::NORM_MINMAX);
	magI.at<float>(cy, cx) = 1.0;	// restore DC component
	magI.convertTo(magI, CV_8UC1, 255);
	return magI;
}

void xMainWnd::dragEnterEvent(QDragEnterEvent* event) {
	if (event->mimeData()->hasUrls())
		event->acceptProposedAction();
}

void xMainWnd::dragMoveEvent(QDragMoveEvent* event) {
}

void xMainWnd::dragLeaveEvent(QDragLeaveEvent* event) {
}

void xMainWnd::dropEvent(QDropEvent* event) {
	const QMimeData* mimeData = event->mimeData();

	// check for our needed mime type, here a file or a list of files
	if (!mimeData->hasUrls())
		return;

	QStringList pathList;
	QList<QUrl> urlList = mimeData->urls();
	if (urlList.empty())
		return;

	auto path = urlList[0].toLocalFile();
	ui.edtPath->setText(path);
	OnImage_Load();
}

void xMainWnd::OnFile_SelChanged() {
	auto index = ui.files->currentIndex();
	std::filesystem::path path = m_modelFileSystem.filePath(index).toStdWString();
	if (path.empty())
		return;
	if (!path.is_absolute()) {
		auto indexFolder = ui.folder->currentIndex();
		if (!indexFolder.isValid())
			return;
		std::filesystem::path pathFolder = m_modelFolderSystem.filePath(indexFolder).toStdWString();
		path = pathFolder / path;
	}

	if (!ShowImage(path))
		ui.view->SetImage({});
}

void xMainWnd::OnImage_Load() {
	std::filesystem::path path = ui.edtPath->text().toStdWString();
	if (path.empty())
		return;
	auto parent = path.parent_path();
	if (parent.empty())
		return;
	if (auto index = m_modelFolderSystem.index(ToQString(parent)); index.isValid())
		ui.folder->setCurrentIndex(index);
	if (auto index = m_modelFileSystem.index(ToQString(path)); index.isValid()) {
		ui.files->setCurrentIndex(index);
	}
	else {
		if (!ShowImage(path))
			ui.view->SetImage({});
	}
}

void xMainWnd::OnImage_Save() {
	if (m_img.empty())
		return;

	QString strFolder;
	if (auto index = ui.folder->currentIndex(); index.isValid()) {
		strFolder = m_modelFolderSystem.filePath(index);
		std::filesystem::path path = strFolder.toStdWString();
		//if (std::filesystem::is_regular_file(path)) {
		//	path = path.parent_path();
		//	strFolder = ToQString(path.wstring());
		//}
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
	auto const& interleave = dlg.m_interleave;
	bool bInterleave = interleave.bUse and ( (interleave.sizeFields.cx > 1) or (interleave.sizeFields.cy > 1) );
	gtl::xSize2i size = dlg.m_size;
	if (size.cx <= 0)
		size.cx = m_img.cols;
	if (size.cy <= 0)
		size.cy = m_img.rows;

	cv::Mat img;
	if (bInterleave)
		img = cv::Mat::zeros(size.cy*page.cy, size.cx*page.cx, m_img.type());	// not img1.size().
	else
		img = m_img;		// alias
	int nImage{};
	if (interleave.bUse) {
		cv::Mat img0 = m_img;	// alias

		auto InterleaveRow = [](cv::Mat img0, cv::Mat& img, int nInterleave, int nPixelGroup, int sizePart) {
			for (int y{}, y0{}; y < sizePart; y++) {
				for (int y1{y*nPixelGroup}; y1 < img.rows; y1 += sizePart) {	// not img1.cols. img1.cols is not multiple of size.cx
					if (y0+nPixelGroup-1 >= img0.rows)
						break;
					//img0.row(y0++).copyTo(img.row(y1+g));
					cv::Rect r0(0, y0, img0.cols, nPixelGroup), r1(0, y1, img0.cols, nPixelGroup);
					img0(r0).copyTo(img(r1));
					y0+=nPixelGroup;
				}
			}
		};

		if (interleave.sizeFields.cx > 1 and interleave.sizeFields.cy > 1) {
			InterleaveRow(img0, img, interleave.sizeFields.cy, interleave.sizePixelGroup.cy, size.cy);
			cv::Mat imgT = img.t(), imgD = cv::Mat::zeros(size.cx*page.cx, size.cy*page.cy, img.type());
			InterleaveRow(imgT, imgD, interleave.sizeFields.cx, interleave.sizePixelGroup.cx, size.cx);
			img = imgD.t();
		} else if (interleave.sizeFields.cy > 1) {
			InterleaveRow(img0, img, interleave.sizeFields.cy, interleave.sizePixelGroup.cy, size.cy);
		} else if (interleave.sizeFields.cx > 1) {
			cv::Mat imgT = img0.t(), imgD = cv::Mat::zeros(size.cx*page.cx, size.cy*page.cy, img.type());
			InterleaveRow(imgT, imgD, interleave.sizeFields.cx, interleave.sizePixelGroup.cx, size.cx);
			img = imgD.t();
		}
	}

	for (int iy{}, y{}; y < img.rows; iy++, y += size.cy) {
		for (int ix{}, x{}; x < img.cols; ix++, x += size.cx) {
			gtl::xRect2i rect{x, y, x + size.cx, y + size.cy};
			if (rect.right > img.cols)
				rect.right = img.cols;
			if (rect.bottom > img.rows)
				rect.bottom = img.rows;
			cv::Mat imgPart((cv::Size)size, img.type());
			auto roi = gtl::GetSafeROI((cv::Rect)rect, img.size());
			img(roi).copyTo(imgPart(cv::Rect(0, 0, roi.width, roi.height)));

			std::filesystem::path path = dlg.m_path.parent_path();
			path /= dlg.m_path.stem().wstring();
			path += std::format(L"_x{:04d}y{:04d}", ix+1, iy+1);
			path += dlg.m_path.extension().wstring();

			if (!SaveImage(imgPart, path, dlg.m_option))
				return ;
			nImage++;
		}
	}

	QMessageBox::information(this, "Done", std::format("{} images saved", nImage).c_str());
}

void xMainWnd::OnImage_RotateLeft() {
	if (m_img.empty())
		return;
	//if (ui.chkUseFreeImage->isChecked()) {
	//	auto index = ui.folder->currentIndex();
	//	std::filesystem::path path = m_modelFileSystem.filePath(index).toStdWString();
	//	if (path.empty())
	//		return;
	//	//FreeImage_Load();
	//	return;
	//}
	xWaitCursor wc;
	cv::flip(m_img.t(), m_img, 0);
	ui.view->SetImage(m_img, false);
}

void xMainWnd::OnImage_RotateRight() {
	if (m_img.empty())
		return;
	xWaitCursor wc;
	cv::flip(m_img.t(), m_img, 1);
	ui.view->SetImage(m_img, false);
}

void xMainWnd::OnImage_Rotate180() {
	if (m_img.empty())
		return;
	xWaitCursor wc;
	cv::flip(m_img, m_img, -1);
	ui.view->SetImage(m_img, false);
}

void xMainWnd::OnImage_FlipLR() {
	if (m_img.empty())
		return;
	xWaitCursor wc;
	cv::flip(m_img, m_img, 1);
	ui.view->SetImage(m_img, false);
}

void xMainWnd::OnImage_FlipUD() {
	if (m_img.empty())
		return;
	xWaitCursor wc;
	cv::flip(m_img, m_img, 0);
	ui.view->SetImage(m_img, false);
}

void xMainWnd::OnImage_FFT(bool bLogScale) {
	auto index = ui.files->currentIndex();
	if (!index.isValid())
		return;
	std::filesystem::path path = m_modelFileSystem.filePath(index).toStdWString();
	if (path.empty())
		return;
	path += L"_ft.png";
	cv::Mat m;
	if (m_img.channels() == 3)
		cv::cvtColor(m_img, m, cv::COLOR_BGR2GRAY);
	else if (m_img.channels() == 4)
		cv::cvtColor(m_img, m, cv::COLOR_BGRA2GRAY);
	else
		m = m_img;
	if (m.empty() or m.channels() != 1)
		return;
	auto mFT = FFT(m, bLogScale);
	if (mFT.empty())
		return;
	std::vector<uchar> buf;
	cv::imencode(".png", mFT, buf);
	if (buf.empty())
		return;
	gtl::ContainerToFile(buf, path);
}

void xMainWnd::OnImage_Test() {
	if (m_img.empty())
		return;
	xWaitCursor wc;
	//cv::threshold(m_img, m_img, 254, 128, cv::ThresholdTypes::THRESH_BINARY_INV);
	cv::threshold(m_img, m_img, 128, 128, cv::ThresholdTypes::THRESH_TRUNC);
	ui.view->SetImage(m_img, false);
}

void xMainWnd::OnBtnBlend_Clicked() {
	if (!m_dlgBlendTest)
		m_dlgBlendTest.emplace(this);
	m_dlgBlendTest->show();
	m_dlgBlendTest->setFocus();
}

void xMainWnd::OnBtnFindDuplicates_Clicked() {
	if (!m_dlgFindDuplicate)
		m_dlgFindDuplicate.emplace(this);
	m_dlgFindDuplicate->show();
	m_dlgFindDuplicate->setFocus();
}

void xMainWnd::OnBtnPixelCount_Clicked() {
	if (m_img.empty())
		return;
	if (m_img.channels() != 1) {
		QMessageBox::warning(this, "Pixel Count", "only supports single channel image");
		return;
	}

	xWaitCursor wc;
	cv::Mat img;
	if (auto rect = ui.view->GetSelectionRect()) {
		auto roi = gtl::GetSafeROI(cv::Rect(*rect), m_img.size());
		img = m_img(roi);
	}
	else {
		img = m_img;
	}

	int channels[] = {0};
	int histSize[] = {256};
	cv::MatND matHist;
	float sranges[] = { 0, 256 };
	const float* ranges[] = { sranges };
	cv::calcHist(&img, 1, channels, cv::Mat(), matHist, 1, histSize, ranges);
	std::wstring str;
	for (int i{}; i < matHist.rows; i++) {
		if (matHist.at<float>(i, 0) > 0) {
			// add locale en_US.utf8
			str += fmt::format(L"Value {} : ", i);
			str += gtl::AddThousandComma<wchar_t>(std::format(L"{}", (int)matHist.at<float>(i, 0)));
			str += L"\n";
		}
	}
	QMessageBox::information(this, "Color Count", ToQString(str));

}
