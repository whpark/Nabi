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

	struct sInterleave {
		bool bUse{};
		gtl::xSize2i sizeFields{1,1};
		gtl::xSize2i sizePixelGroup{1,1};
	} m_interleave;

public:
	xSplitImageDlg(cv::Mat img, QWidget* parent = nullptr);
	~xSplitImageDlg();

	bool UpdateData(bool bSave);

	gtl::xSize2i GetPage() const {
		return gtl::xSize2i{GetPageX(), GetPageY()};
	}
	int GetPageX(int v = 0) const {
		if (v <= 0)
			v = ui.spinWidth->value();
		return (v <= 0) ? 1 : (m_img.cols / v) + (m_img.cols % v ? 1 : 0);
	}
	int GetPageY(int v = 0) const {
		if (v <= 0)
			v = ui.spinHeight->value();
		return (v <= 0) ? 1 : (m_img.rows / v) + (m_img.rows % v ? 1 : 0);
	}

protected:
	void OnWidthChanged(int val);
	void OnHeightChanged(int val);
	void OnBrowse();
	void OnPathChanged();

	void OnInterleaveFieldsXChanged(int val);
	void OnInterleaveFieldsYChanged(int val);
	void OnInterleavePixelGroupingXChanged(int val);
	void OnInterleavePixelGroupingYChanged(int val);

private:
	Ui::SplitImageDlgClass ui;
};
