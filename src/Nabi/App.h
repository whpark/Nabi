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

	xMainWnd& GetMainWnd() { return *m_wndMain; }

public:
	QSettings& GetReg() { return m_reg; };

};

//---------------------------------------------------------------------------------------------------------------------------------
inline xApp& theApp() {
	return *static_cast<xApp*>(QCoreApplication::instance());
}
