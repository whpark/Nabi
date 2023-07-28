#include "pch.h"
#include "QTreeViewEx.h"

QTreeViewEx::QTreeViewEx(QWidget* parent) : base_t(parent) {
}

QTreeViewEx::~QTreeViewEx() {
}

void QTreeViewEx::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {
	base_t::selectionChanged(selected, deselected);
	emit selChanged();
}

//void QTreeViewEx::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
//	base_t::currentChanged(current, previous);
//	emit selChanged();
//}
