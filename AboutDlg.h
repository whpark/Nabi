#pragma once

#include <QDialog>
#include "ui_AboutDlg.h"

class xAboutDlg : public QDialog {
	Q_OBJECT

public:
	xAboutDlg(QWidget* parent = nullptr);
	~xAboutDlg();

private:
	Ui::AboutDlgClass ui;
};
