#include "customfilesystemmodel.h"

CustomFileSystemModel::CustomFileSystemModel(QObject *parent) : QFileSystemModel(parent), tagManager(nullptr) {}

void CustomFileSystemModel::setTagManager(TagManager *manager) {
    tagManager = manager;
}

QVariant CustomFileSystemModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        int totalColumns = QFileSystemModel::columnCount(index.parent());
        if (index.column() == totalColumns) {
            QString filePath = this->filePath(index.siblingAtColumn(0));
            QStringList tags = tagManager->getTags(filePath);
            return tags.join(", ");
        }
    }
    return QFileSystemModel::data(index, role);
}

int CustomFileSystemModel::columnCount(const QModelIndex &parent) const {
    return QFileSystemModel::columnCount(parent) + 1;
}

QVariant CustomFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == QFileSystemModel::columnCount() - 1 + 1) {
            return QString("Tags");
        }
    }
    return QFileSystemModel::headerData(section, orientation, role);
}

// Optionally, implement filtering and other functionalities as needed
