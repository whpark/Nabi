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

protected:
	// Palette
	inline static std::vector<gtl::color_bgra_t> s_palette_1bit {
		gtl::color_bgra_t{  0,   0,   0},	// 0
		gtl::color_bgra_t{255, 255, 255},	// 1
	};
	inline static std::vector<gtl::color_bgra_t> s_palette_4bit {
		gtl::color_bgra_t{255, 255, 255},	// 0
		gtl::color_bgra_t{  0,   0,   0},	// 1
		gtl::color_bgra_t{120, 250,  70},	// 2
		gtl::color_bgra_t{ 80,  50, 230},	// 3
		gtl::color_bgra_t{ 80, 180, 250},	// 4
		gtl::color_bgra_t{225, 110,  10},	// 5
		gtl::color_bgra_t{240,   0,   0},	// 6
		gtl::color_bgra_t{170,  50, 240},	// 7
		gtl::color_bgra_t{240,   0,  50},	// 8
		gtl::color_bgra_t{220,   0,   0},	// 9
		gtl::color_bgra_t{200,   0,   0},	// 10
		gtl::color_bgra_t{170,   0,   0},	// 11
		gtl::color_bgra_t{150,   0,   0},	// 12
		gtl::color_bgra_t{130,   0,   0},	// 13
		gtl::color_bgra_t{100,   0,   0},	// 14
		gtl::color_bgra_t{200, 100, 100},	// 15
	};
	inline static std::vector<gtl::color_bgra_t> s_palette_8bit;

private:
	Ui::MainWndClass ui;
};
