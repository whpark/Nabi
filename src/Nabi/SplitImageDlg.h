#pragma once

#include <QDialog>
#include "ui_SplitImageDlg.h"
#include "BitmapSaveOption.h"

class xSplitImageDlg : public QDialog {
	Q_OBJECT
public:
	using this_t = xSplitImageDlg;
	using base_t = QDialog;

protected:
	cv::Mat m_img;

public:
	std::filesystem::path m_path;
	gtl::xSize2i m_size;
	sBitmapSaveOption m_option;

public:
	xSplitImageDlg(cv::Mat img, QWidget* parent = nullptr);
	~xSplitImageDlg();

	void UpdateData(bool bSave);

protected:
	void OnWidthChanged(int val);
	void OnHeightChanged(int val);
	void OnBrowse();
	void OnPathChanged();

private:
	Ui::SplitImageDlgClass ui;
};
