#pragma once

#include <QDialog>
#include "ui_SaveOptionDlg.h"

class xSaveOptionDlg : public QDialog {
	Q_OBJECT
public:
	using this_t = xSaveOptionDlg;
	using base_t = QDialog;

public:
	enum class eBPP : int { _1, /*_2, */_4, _8, _24/*, _32*/ };
	eBPP m_eBPP{};
	gtl::xSize2i m_dpi;
	bool m_bBottomToTop{true};

	static constexpr int GetBPP(eBPP e) {
		switch (e) {
		case eBPP::_1: return 1;
		//case eBPP::_2: return 2;
		case eBPP::_4: return 4;
		case eBPP::_8: return 8;
		case eBPP::_24: return 24;
		//case eBPP::_32: return 32;
		default : return 0;
		}
	}
	static constexpr gtl::xSize2i GetPelsPerMeter(gtl::xSize2i dpi) {
		return { gtl::Round(dpi.cx * 1000 / 25.4), gtl::Round(dpi.cy * 1000/25.4)};
	}
public:
	xSaveOptionDlg(QWidget* parent = nullptr);
	~xSaveOptionDlg();

	bool UpdateData(bool bSave);

private:
	Ui::SaveOptionDlgClass ui;
};
