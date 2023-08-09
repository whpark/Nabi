#pragma once

#include <QFileSystemModel>

class xFolderSystemModel : public QFileSystemModel {
	Q_OBJECT
public:
	using this_t = xFolderSystemModel;
	using base_t = QFileSystemModel;

public:
	xFolderSystemModel(QObject* parent = nullptr);
	~xFolderSystemModel();


	//bool hasIndex(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex index(int row, int column, const QModelIndex& indexParent = QModelIndex()) const override {
		return base_t::index(row, column, indexParent);
	}
	QModelIndex parent(const QModelIndex& child) const override {
		return base_t::parent(child);
	}

	//QModelIndex sibling(int row, int column, const QModelIndex &idx) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override {
		return base_t::rowCount(parent);
	}
	int columnCount(const QModelIndex& parent = QModelIndex()) const override {
		return base_t::columnCount(parent);
	}
	//bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
		return base_t::data(index, role);
	}
	//bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

	//QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	//bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

};
