#include "pch.h"
#include "MainWnd.h"
#include "AboutDlg.h"

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
	connect(ui.actionAbout, &QAction::triggered, this, [this](auto) { xAboutDlg dlg(this); dlg.exec();});
	//connect(ui.folder, &QTreeView::activated, this, &xMainWnd::OnFolder_Activated);
	connect(ui.folder, &QTreeViewEx::selChanged, this, &this_t::OnFolder_SelChanged);
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
				gtl::xSize2i pelsPerMeter;
				bLoadBitmapMatTRIED = true;
				img = gtl::LoadBitmapMat(path, pelsPerMeter);
			}
		}
	}
	if (img.empty())
		img = gtl::LoadImageMat(path);
	if (img.empty() and (path.extension() == L".bmp") and !bLoadBitmapMatTRIED) {
		gtl::xSize2i pelsPerMeter;
		img = gtl::LoadBitmapMat(path, pelsPerMeter);
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
	return true;
}

void xMainWnd::OnFolder_SelChanged() {
	auto index = ui.folder->currentIndex();
	std::filesystem::path path = m_modelFileSystem.filePath(index).toStdWString();
	if (path.empty())
		return;
	ShowImage(path);
}
