#include "pch.h"
#include "App.h"
#include "gtl/win/EnvironmentVariable.h"

int main(int argc, char* argv[]) {
	//auto r = gtl::SetCurrentPath_BinFolder();
	xApp a(argc, argv);

	auto var = qEnvironmentVariable("OPENCV_IO_MAX_IMAGE_PIXELS");
	uint64_t max = var.toULongLong();
	if (max < 0x01ull << 40) {
		auto r = QMessageBox::warning(nullptr,
			"openCV - Large Image",
			"openCV - Large Image not supported.",
			QMessageBox::Close);
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
		}
	}

	return a.exec();
}

xApp::xApp(int &argc, char **argv) : QApplication(argc, argv) {
	theApp().setStyle("fusion");

	m_wndMain = std::make_unique<xMainWnd>();
	m_wndMain->show();
}

xApp::~xApp() {
}
