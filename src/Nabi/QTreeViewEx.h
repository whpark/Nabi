#pragma once

#include <QTreeView>

class QTreeViewEx : public QTreeView {
	Q_OBJECT

public:
	using this_t = QTreeViewEx;
	using base_t = QTreeView;

public:
	explicit QTreeViewEx(QWidget* parent);
	~QTreeViewEx();

protected:
	virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
	virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
signals:
	void selChanged();

};
