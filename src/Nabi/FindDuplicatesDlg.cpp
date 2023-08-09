#include "pch.h"
#include "App.h"
#include "FindDuplicatesDlg.h"
#include "gtl/qt/QColorButton.h"

using namespace gtl::qt;
namespace stdfs = std::filesystem;

static QColor const crON(0, 255, 0);
static QColor const crOFF(192, 192, 192);
static QColor const crText(0, 0, 0);

xFindDuplicatesDlg::xFindDuplicatesDlg(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.treeFolder->setModel(&m_modelFolder);
	ui.treeFolder->setColumnWidth(0, 300);

	connect(ui.edtRoot, &QLineEdit::returnPressed, this, &xFindDuplicatesDlg::OnEdtRoot_ReturnPressed);
	connect(ui.toolBrowse, &QToolButton::clicked, this, &xFindDuplicatesDlg::OnToolBrowse_Clicked);
	connect(ui.btnFindDuplicates, &QPushButton::clicked, this, &xFindDuplicatesDlg::OnBtnFindDuplicates_Clicked);
	connect(ui.btnDeleteDuplicates, &QPushButton::clicked, this, &xFindDuplicatesDlg::OnBtnDeleteDuplicates_Clicked);
	connect(&m_timerUI, &QTimer::timeout, this, &xFindDuplicatesDlg::OnTimerUI_Timeout);

	ui.btnFindDuplicates->SetMainColor(crOFF, crText);
	ui.btnDeleteDuplicates->SetMainColor(crOFF, crText);

	auto& reg = theApp().GetReg();
	auto root = reg.value("FindDuplicatesDlg/root").toString();
	ui.edtRoot->setText(root);
	OnEdtRoot_ReturnPressed();

	ui.treeFiles->setModel(&m_model);
	ui.treeFiles->setColumnWidth(0, 500);
}

xFindDuplicatesDlg::~xFindDuplicatesDlg() {
	if (m_worker) {
		m_worker->get_stop_source().request_stop();
		m_worker->join();
	}
}

bool CompareFile(std::filesystem::path const& path1, std::filesystem::path const& path2) {
	constexpr size_t len{4*1024};
	static char buf1[len], buf2[len];

	std::ifstream f1(path1, std::ios::binary), f2(path2, std::ios::binary);
	if (!f1 or !f2)
		return false;

	f1.seekg(0, std::ios::end);
	auto left = f1.tellg();
	f1.seekg(0, std::ios::beg);
	while (left > 0) {
		auto to_read = std::min(sizeof(buf1), (size_t)left);
		left -= to_read;
		if (!f1.read(buf1, to_read) or !f2.read(buf2, to_read))
			return false;

		if (memcmp(buf1, buf2, to_read) != 0)
			return false;

	};

	return true;
}

void xFindDuplicatesDlg::DuplicatedImageFinder(std::stop_token st, std::vector<std::filesystem::path> const& folders, bool bRecursive) {
	m_pos = {0, 0};

	// check folders are excluded
	for (size_t i{}; bRecursive and i < folders.size(); i++) {
		auto const& p0 = folders[i];
		for (size_t j{}; j < folders.size(); j++) {
			if (i == j)
				continue;
			auto const& p1 = folders[j];

			auto p = p0.lexically_relative(p1);
			if (!p.empty()) {
				bRecursive = false;
				break;
			}
		}
	}

	auto AddFiles = [&](auto&& dir_iter, std::deque<sDuplicatedItem>& files) {
		for (auto entry : dir_iter) {
			if (st.stop_requested())
				break;
			files.emplace_back(nullptr, entry.path(), entry.file_size(), entry.last_write_time());
			m_pos.second++;
		}
	};

	std::deque<sDuplicatedItem> files;
	if (bRecursive) {
		for (auto const& folder : folders) {
			AddFiles(std::filesystem::recursive_directory_iterator(folder), files);
		}
	}
	else {
		for (auto const& folder : folders) {
			AddFiles(std::filesystem::directory_iterator(folder), files);
		}
	}

	// sort by size
	if (!st.stop_requested()) {
		std::sort(files.begin(), files.end(),
			[](auto const& a, auto const& b) -> bool {
				if (a.len == b.len)
					return a.tLastWrite < b.tLastWrite;
				return a.len < b.len;
			} 
		);
	}

	m_pos.second = files.size();
	for (size_t i{}; !st.stop_requested() and i < files.size(); i++) {
		m_pos.first = i;
		auto const& e1 = files[i];
		std::vector<sDuplicatedItem const*> dups;
		for (size_t j{ i+1 }; !st.stop_requested() and j < files.size(); j++) {
			auto const& e2 = files[j];

			//if (std::abs((int64_t)e1.file_size() - (int64_t)e2.file_size()) > 100)
			//	break;
			if (e1.len != e2.len)
				break;

			if (!CompareFile(e1.path, e2.path))
				continue;

			dups.push_back(&e2);

			i = j;
		}

		if (dups.empty())
			continue;

		{
			auto item = std::make_unique<sDuplicatedItem>(&m_model.GetRootItem(), e1.path, e1.len, e1.tLastWrite);
			for (auto const* dup : dups) {
				auto child = std::make_unique<sDuplicatedItem>(item.get(), dup->path, dup->len, dup->tLastWrite);
				item->children.push_back(std::move(child));
			}
			m_model.InsertLeaf(std::move(item));
		}
	}

	m_pos = {0, 0};
	m_done = true;
}

void xFindDuplicatesDlg::OnEdtRoot_ReturnPressed() {
	auto strRoot = ui.edtRoot->text();
	std::filesystem::path path(strRoot.toStdWString());
	if (!std::filesystem::is_directory(path))
		return ;
	theApp().GetReg().setValue("FindDuplicatesDlg/root", strRoot);
	auto index = m_modelFolder.setRootPath(strRoot);
	ui.treeFolder->setRootIndex(index);
}

void xFindDuplicatesDlg::OnToolBrowse_Clicked(bool checked) {
	auto str = QFileDialog::getExistingDirectory(this, "Select a folder", ui.edtRoot->text());
	ui.edtRoot->setText(str);
	OnEdtRoot_ReturnPressed();
}

void xFindDuplicatesDlg::OnBtnFindDuplicates_Clicked() {
	if (m_worker) {
		m_worker->get_stop_source().request_stop();
		return;
	}

	m_done = false;
	std::vector<std::filesystem::path> folders;
	auto indices = ui.treeFolder->selectionModel()->selectedIndexes();
	if (indices.empty()) {
		ui.treeFolder->selectAll();
		indices = ui.treeFolder->selectionModel()->selectedIndexes();
	}
	folders.reserve(indices.size() / ui.treeFolder->model()->columnCount());
	for (auto& index : indices) {
		if (index.column())
			continue;
		auto strPath = m_modelFolder.filePath(index);
		folders.emplace_back(strPath.toStdWString());
	}

	m_worker.emplace([this, folders](std::stop_token st) { DuplicatedImageFinder(st, folders, ui.chkRecursive->isChecked()); });

	m_timerUI.start(100ms);
	ui.btnFindDuplicates->SetMainColor(crON, crText);
}

void xFindDuplicatesDlg::OnBtnDeleteDuplicates_Clicked() {
	xWaitCursor wc;

	for (auto& item : m_model.GetRootItem().children) {
		for (auto const& c : item->children) {
			//OutputDebugStringW(fmt::format(L"file: {}\n", c->path.wstring()).c_str());
			std::error_code ec;
			stdfs::remove(c->path, ec);
		}
	}
}

void xFindDuplicatesDlg::OnTimerUI_Timeout() {
	if (m_done and m_worker) {
		m_worker->join();
		m_worker.reset();
		m_done = false;
		m_timerUI.stop();

		ui.btnFindDuplicates->SetMainColor(crOFF, crText);
	}

	// status
	auto str = ToQString(std::format("{} / {}", m_pos.first, m_pos.second));
	if (ui.txtStatus->text() != str)
		ui.txtStatus->setText(str);
}
