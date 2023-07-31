#pragma once

#include <QDialog>
#include "ui_BitmapSaveOptionDlg.h"
#include "BitmapSaveOption.h"

//---------------------------------------------------------------------------------------------------------------------------------
class xBitmapSaveOptionDlg : public QDialog {
	Q_OBJECT
public:
	using this_t = xBitmapSaveOptionDlg;
	using base_t = QDialog;

	sBitmapSaveOption m_option;
public:
	xBitmapSaveOptionDlg(QWidget* parent = nullptr);
	~xBitmapSaveOptionDlg();

	bool UpdateData(bool bSave);

private:
	Ui::BitmapSaveOptionDlgClass ui;
};
