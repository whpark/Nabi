#pragma once

#include "QTreeViewEx.h"
#include "ui_MainWnd.h"
#include "BitmapSaveOption.h"
#include "BlendTestDlg.h"
#include "FindDuplicatesDlg.h"
#include "gtl/qt/QPathCompleter.h"

class xMainWnd : public QMainWindow {
	Q_OBJECT
public:
	using this_t = xMainWnd;
	using base_t = QMainWindow;

protected:
	QFileSystemModel m_modelFileSystem;
	cv::Mat m_img;
	sBitmapSaveOption m_optionBitmap;

	//std::optional<QFileSystemModel> m_modelForCompleter;
	//std::optional<QCompleter> m_completer;

	std::optional<gtl::qt::QPathCompleter> m_completer;

	std::optional<xBlendTestDlg> m_dlgBlendTest;
	std::optional<xFindDuplicatesDlg> m_dlgFindDuplicate;

public:
	QSettings m_reg { "Biscuit-lab", "Nabi" };

public:
	xMainWnd(QWidget* parent = nullptr);
	~xMainWnd();

public:
	bool ShowImage(std::filesystem::path const& path);
protected:
	bool SaveImage(cv::Mat img0, std::filesystem::path const& path, sBitmapSaveOption const& option);

public:
	bool UseFreeImage() const { return ui.chkUseFreeImage->isChecked(); }

protected:
	// slots
	//void OnFolder_Activated(const QModelIndex &index);
	void OnFolder_SelChanged();
	void OnImage_Load();
	void OnImage_Save();
	void OnImage_Split();
	void OnImage_RotateLeft();
	void OnImage_RotateRight();
	void OnImage_Rotate180();
	void OnImage_FlipLR();
	void OnImage_FlipUD();

	void OnBtnBlend_Clicked();
	void OnBtnFindDuplicates_Clicked();

private:
	Ui::MainWndClass ui;
};
