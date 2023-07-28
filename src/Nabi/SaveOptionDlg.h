#pragma once

#include <QDialog>
#include "ui_SaveOptionDlg.h"

class xSaveOptionDlg : public QDialog {
	Q_OBJECT
public:
	using this_t = xSaveOptionDlg;
	using base_t = QDialog;

public:
	xSaveOptionDlg(QWidget* parent = nullptr);
	~xSaveOptionDlg();

private:
	Ui::SaveOptionDlgClass ui;
};
