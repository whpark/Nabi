#include "pch.h"
#include "AboutDlg.h"

xAboutDlg::xAboutDlg(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);

	QString strBuildDate;
	try {
		std::istringstream strDate(__DATE__);
		std::string month;
		int day{}, year{};
		strDate >> month;
		strDate >> day;
		strDate >> year;
		static std::vector<std::string> const months { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", };
		auto pos = std::find(months.begin(), months.end(), month);
		auto m = std::distance(months.begin(), pos) + 1;
		strBuildDate = ToQString(std::format("{}-{:02d}-{:02d}", year, (int)m, day));
	} catch (...) {
		strBuildDate = __DATE__;
	}

	ui.groupBoxNabi->setTitle(strBuildDate);

	connect(ui.buttonBox, &QDialogButtonBox::clicked, this, [this](auto*){ accept(); });
}

xAboutDlg::~xAboutDlg() {
}
