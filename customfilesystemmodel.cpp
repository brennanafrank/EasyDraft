#include "customfilesystemmodel.h"

CustomFileSystemModel::CustomFileSystemModel(QObject *parent) : QFileSystemModel(parent), tagManager(nullptr) {}

void CustomFileSystemModel::setTagManager(TagManager *manager) {
    tagManager = manager;
}

QVariant CustomFileSystemModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole && index.column() == 0) {
        QString filePath = this->filePath(index);
        QStringList tags = tagManager->getTags(filePath);
        QString displayText = QFileSystemModel::data(index, role).toString();
        if (!tags.isEmpty()) {
            displayText += " [" + tags.join(", ") + "]";
        }
        // qDebug() << "Display text for" << filePath << ":" << displayText;
        return displayText;
    }
    return QFileSystemModel::data(index, role);
}


void CustomFileSystemModel::setFilterTag(const QString& tag) {
    currentFilterTag = tag;
    emit layoutChanged(); // Notify views that the data has changed
}


