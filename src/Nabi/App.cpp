#include "pch.h"
#include "App.h"
#include "gtl/win/EnvironmentVariable.h"
#include "FreeImage.h"

std::optional<xApp> theApp;

int main(int argc, char* argv[]) {
	//auto r = gtl::SetCurrentPath_BinFolder();
	theApp.emplace(argc, argv);
	if (!theApp->Init())
		return -1;

	auto r = theApp->exec();
	theApp.reset();

	return r;
}

xApp::xApp(int &argc, char **argv) : QApplication(argc, argv) {
	FreeImage_Initialise();
	//QString str = FreeImage_GetCopyrightMessage();
	////This program uses FreeImage, a free, open source image library supporting all common bitmap formats. See http://freeimage.sourceforge.net for details
}

xApp::~xApp() {
	FreeImage_DeInitialise();
}

bool xApp::Init() {
	setStyle("fusion");
	std::this_thread::sleep_for(100ms);

	m_wndMain = std::make_unique<xMainWnd>();
	m_wndMain->show();

	uint64_t max{};
	if (auto const* var = std::getenv("OPENCV_IO_MAX_IMAGE_PIXELS")) {
		auto len = std::strlen(var);
		max = gtl::tszto<uint64_t>(var, var+len);
	}
	if (max < 0x01ull << 40) {
		auto r = QMessageBox::warning(nullptr,
			"openCV - Large Image",
			"openCV - Large Image not supported. Do you want to use Large Image?",
			QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Cancel);

		if (r == QMessageBox::Yes) {
			// export environment variable
			gtl::qt::xWaitCursor wc;

			using namespace gtl::win;
			xEnvironmentVariable var(xEnvironmentVariable::eSCOPE::LOCAL_MACHINE);
			if (!var)
				var.Open(xEnvironmentVariable::eSCOPE::CURRENT_USER);
			if (var) {
				var.Set(L"OPENCV_IO_MAX_IMAGE_PIXELS", std::format(L"{}", 0x01ull << 40));
				var.Set(L"OPENCV_IO_MAX_IMAGE_WIDTH", std::format(L"{}", 0x01ull << 24));
				var.Set(L"OPENCV_IO_MAX_IMAGE_HEIGHT", std::format(L"{}", 0x01ull << 24));
				var.Broadcast();
			}

			QMessageBox::warning(nullptr,
				"openCV - Large Image",
				"Please, Restart Program",
				QMessageBox::Close);

			return false;
		}
	}

	return true;
}
