#include "pch.h"
#include "scn/scn.h"
#include "MainWnd.h"
#include "gtl/win/EnvironmentVariable.h"

//OPENCV_IO_MAX_IMAGE_PIXELS=1099511627776
int main(int argc, char* argv[]) {

	QApplication a(argc, argv);
	a.setStyle("fusion");

	//if (auto hProcess = GetCurrentProcess()) {
	//	HANDLE hToken{};
	//	OpenProcessToken(hProcess, TOKEN_QUERY, &hToken);
	//	PRIVILEGE_SET ps{.PrivilegeCount = 1, .Control = PRIVILEGE_SET_ALL_NECESSARY};
	//	BOOL result{};
	//	PrivilegeCheck(hToken, &ps, &result);
	//	if (result) {
	//		qDebug() << "PrivilegeCheck: " << result;
	//	}
	//}

	xMainWnd w;
	w.show();

	auto var = qEnvironmentVariable("OPENCV_IO_MAX_IMAGE_PIXELS");
	uint64_t max = var.toULongLong();
	if (max < 0x01ull << 40) {
		auto r = QMessageBox::warning(nullptr,
			"openCV - Large Image",
			"openCV - Large Image not supported.",
			QMessageBox::Close);
		if (r == QMessageBox::Yes) {
			// export environmet variable

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

			//auto h = RegisterApplicationRestart(L"", 0);

			return 0;
		}
	}

	return a.exec();
}
//
