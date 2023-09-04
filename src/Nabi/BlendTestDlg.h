#pragma once

#include <QDialog>
#include "ui_BlendTestDlg.h"

class xBlendTestDlg : public QDialog {
	Q_OBJECT

public:
	xBlendTestDlg(QWidget* parent = nullptr);
	~xBlendTestDlg();

protected:
	void OnBtnTest1();
	void OnBtnTest2();
	void OnBtnTest3();

private:
	Ui::BlendTestDlgClass ui;
};
