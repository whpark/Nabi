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
	void OnOpen_clicked();

private:
	Ui::MainWndClass ui;
};
