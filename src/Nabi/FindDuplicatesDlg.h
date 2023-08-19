#pragma once

#include <QDialog>
#include "ui_FindDuplicatesDlg.h"
#include "FolderSystemModel.h"
#include "FindDuplicatesModel.h"
#include "gtl/qt/QPathCompleter.h"

class xFindDuplicatesDlg : public QDialog {
	Q_OBJECT
public:
	using this_t = xFindDuplicatesDlg;
	using base_t = QDialog;

protected:
	xFolderSystemModel m_modelFolder;
	xFindDuplicatesModel m_model;
	QTimer m_timerUI;

	std::optional<gtl::qt::QPathCompleter> m_completer;

public:
	xFindDuplicatesDlg(QWidget* parent = nullptr);
	~xFindDuplicatesDlg();

protected:
	std::optional<std::jthread> m_worker;
	std::mutex m_mtxList;
	//std::deque<std::vector<std::filesystem::path>> m_fileCollection;
	std::pair<size_t, size_t> m_pos;
	std::atomic<bool> m_done{};
	void DuplicatedImageFinder(std::stop_token st, std::vector<std::filesystem::path> const& folder, bool bRecursive);

protected:
	void OnEdtRoot_ReturnPressed();
	void OnToolBrowse_Clicked(bool checked = false);
	void OnBtnFindDuplicates_Clicked();
	void OnBtnDeleteDuplicates_Clicked();
	void OnTreeFiles_CurrentChanged(QModelIndex const &current, QModelIndex const& previous);
	void OnTimerUI_Timeout();

private:
	Ui::FindDuplicatesDlgClass ui;
};
