#include "pch.h"
#include "FolderSystemModel.h"

xFolderSystemModel::xFolderSystemModel(QObject* parent) : QFileSystemModel(parent) {
	setFilter(QDir::Filter::AllDirs | QDir::Filter::NoDotAndDotDot);
}

xFolderSystemModel::~xFolderSystemModel() {
}
