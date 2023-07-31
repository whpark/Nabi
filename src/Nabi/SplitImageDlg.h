#pragma once

#include <QDialog>
#include "ui_SplitImageDlg.h"

class xSplitImageDlg : public QDialog {
	Q_OBJECT
public:
	using this_t = xSplitImageDlg;
	using base_t = QDialog;

public:
	xSplitImageDlg(QWidget* parent = nullptr);
	~xSplitImageDlg();

private:
	Ui::SplitImageDlgClass ui;
};
