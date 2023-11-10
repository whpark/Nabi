#pragma once

#include "MainWnd.h"

//---------------------------------------------------------------------------------------------------------------------------------
class xApp : public QApplication {
	Q_OBJECT
public:
	using this_t = xApp;
	using base_t = QApplication;

protected:
	QSettings m_reg { "Biscuit-lab", "Nabi" };
	std::unique_ptr<xMainWnd> m_wndMain;

public:
	xApp(int &argc, char **argv/*, int flag = ApplicationFlags*/);
	~xApp();

public:
	bool Init();
	std::filesystem::path m_folderCurrent;

public:
	QSettings& GetReg() { return m_reg; };
	xMainWnd& GetMainWnd() { return *m_wndMain; }

};

//---------------------------------------------------------------------------------------------------------------------------------
extern std::optional<xApp> theApp;

template < typename tOption >
bool SyncOption(QSettings& reg, bool bStore, std::string_view cookie, tOption& option) {
	if (bStore) {
		std::string buffer = glz::write_json(option);
		reg.setValue(cookie, ToQString(buffer));
	}
	else {
		auto str = reg.value(cookie).toString();
		if (str.isEmpty())
			return false;
		auto buffer = ToString(str);
		auto err = glz::read_json(option, buffer);
	}
	return true;
};
