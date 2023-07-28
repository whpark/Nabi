#pragma once

#include "QTreeViewEx.h"
#include "ui_MainWnd.h"

class xMainWnd : public QMainWindow {
	Q_OBJECT
public:
	using this_t = xMainWnd;
	using base_t = QMainWindow;

protected:
	QFileSystemModel m_modelFileSystem;
public:
	QSettings m_reg { "Biscuit-lab", "Nabi" };

public:
	xMainWnd(QWidget* parent = nullptr);
	~xMainWnd();

public:
	bool ShowImage(std::filesystem::path const& path);

protected:
	// slots
	//void OnFolder_Activated(const QModelIndex &index);
	void OnFolder_SelChanged();
	void OnImage_Load();
	void OnImage_Save();
	void OnImage_RotateLeft();
	void OnImage_RotateRight();
	void OnImage_FlipLR();
	void OnImage_FlipUD();

private:
	Ui::MainWndClass ui;
};
