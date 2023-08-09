#pragma once

#include <QAbstractItemModel>

struct sDuplicatedItem {
	sDuplicatedItem const* parent{};
	std::filesystem::path path;
	size_t len{};
	std::filesystem::file_time_type tLastWrite{};
	std::vector<std::unique_ptr<sDuplicatedItem>> children;
};

class xFindDuplicatesModel : public QAbstractItemModel {
	Q_OBJECT
public:
	using this_t = xFindDuplicatesModel;
	using base_t = QAbstractItemModel;

protected:
	mutable std::recursive_mutex m_mtx;
	sDuplicatedItem m_top;

public:
	auto const& GetRootItem() const { return m_top; }

	auto const* GetItem(QModelIndex const& index) const {
		return std::bit_cast<sDuplicatedItem const*>(index.constInternalPointer());
	}
	auto* GetItem(QModelIndex const& index) {
		return std::bit_cast<sDuplicatedItem*>(index.constInternalPointer());
	}

	//bool hasIndex(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex index(int row, int column, const QModelIndex& indexParent = QModelIndex()) const override {
		if (row < 0 or column < 0 or row >= rowCount(indexParent) or column >= columnCount(indexParent))
			return {};

		std::scoped_lock lock(m_mtx);
		auto* parent = GetItem(indexParent);
		if (!parent)
			parent = &m_top;

		if (row >= parent->children.size())
			return {};

		return createIndex(row, column, std::bit_cast<void*>(parent->children[row].get()));
	}
	QModelIndex parent(const QModelIndex& child) const override {
		//std::scoped_lock lock(m_mtx);
		auto* item = GetItem(child);
		if (!item)
			return {};
		if (!item->parent or item->parent == &m_top)
			return {};

		auto* pparent = item->parent->parent;
		if (!pparent)
			return {};
		int row{};
		auto const& s = pparent->children;
		auto const* p = item->parent;
		for (int row{}; row < s.size(); row++) {
			if (s[row].get() == p)
				return createIndex(row, 0, std::bit_cast<void*>(item->parent));
		}

		return {};
	}

	//QModelIndex sibling(int row, int column, const QModelIndex &idx) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override {
		if (parent.column() > 0)
			return 0;

		if (parent.row() < 0 or !parent.constInternalPointer()) {
			return m_top.children.size();
		}

		std::scoped_lock lock(m_mtx);
		if (auto* item = GetItem(parent)) {
			return item->children.size();
		}
		return 0;
	}
	int columnCount(const QModelIndex& parent = QModelIndex()) const override {
		if (parent.column() > 0)
			return 0;
		return 2;
	}
	//bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
		using namespace gtl::qt;
		auto* item = GetItem(index);
		if (!item)
			return {};
		if (role == Qt::DisplayRole) {
			std::scoped_lock lock(m_mtx);
			switch (index.column()) {
			case 0: return ToQString(item->path);
			case 1: return item->len;
			}
		}
		return {};
	}
	//bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
		if (role == Qt::DisplayRole) {
			switch (section) {
			case 0: return "path";
			case 1: return "size";
			}
		}
		return {};
	}
	//bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

	bool InsertLeaf(std::unique_ptr<sDuplicatedItem> item) {
		if (!item or item->parent != &m_top)
			return false;
		auto s = m_top.children.size();
		auto i = index(-1, -1);
		{
			std::scoped_lock lock(m_mtx);
			beginInsertRows(i, s, s+1);
			m_top.children.push_back(std::move(item));
			endInsertRows();
		}

		return true;
	}

};
