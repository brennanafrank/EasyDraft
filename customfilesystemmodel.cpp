#include "customfilesystemmodel.h"
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include <QMessageBox>


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

Qt::ItemFlags CustomFileSystemModel::flags(const QModelIndex &index) const {
    return QFileSystemModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QMimeData *CustomFileSystemModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData *mimeData = new QMimeData();
    QList<QUrl> urls;
    QSet<QString> addedPaths;  // To check for duplicate paths

    // qDebug() << "Creating MIME data for indexes:" << indexes.size();

    for (const QModelIndex &index : indexes) {
        if (index.isValid()) {
            QString path = this->filePath(index);
            if (!addedPaths.contains(path)) {
                QUrl url = QUrl::fromLocalFile(path);
                urls.append(url);
                addedPaths.insert(path);
                // qDebug() << "Added URL:" << url;
            } else {
                // qDebug() << "Duplicate path skipped:" << path;
            }
        }
    }
    mimeData->setUrls(urls);
    return mimeData;
}


bool CustomFileSystemModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const {
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);

    return data->hasUrls();
}

bool CustomFileSystemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    // qDebug() << "dropMimeData called with action:" << action;

    if (!data->hasUrls()) {
        qDebug() << "No URLs in data";
        return false;
    }

    QModelIndex dropIndex = parent.isValid() ? parent : this->index(rootPath());
    QString destDirectory = filePath(dropIndex);
    // qDebug() << "Intended destination directory:" << destDirectory;

    // Check if the intended destination is a directory
    QFileInfo checkDestDir(destDirectory);
    if (!checkDestDir.isDir()) {
        // qDebug() << "Destination is not a directory. Using file's parent directory instead.";
        destDirectory = checkDestDir.absolutePath();
    }

    bool anySuccess = false;
    QList<QUrl> urls = data->urls();
    // qDebug() << "URLs to process:" << urls.count();

    QSet<QString> processedPaths; // To avoid processing the same file path multiple times

    foreach (const QUrl &url, urls) {
        QString srcFilePath = url.toLocalFile();
        if (processedPaths.contains(srcFilePath)) {
            // qDebug() << "Skipping already processed file:" << srcFilePath;
            continue;
        }
        processedPaths.insert(srcFilePath);

        QFileInfo srcFileInfo(srcFilePath);
        QString destFilePath = destDirectory + "/" + srcFileInfo.fileName();
        // qDebug() << "Processing file:" << srcFilePath << "to" << destFilePath;

        bool success;
        if (action == Qt::CopyAction) {
            success = QFile::copy(srcFilePath, destFilePath);
            // qDebug() << "Copy";
        } else if (action == Qt::MoveAction) {
            success = QFile::rename(srcFilePath, destFilePath);
            // qDebug() << "Move";
        } else {
            continue; // Skip unsupported actions
        }

        if (success) {
            anySuccess = true;
        } else {
            QMessageBox::critical(nullptr, "Error Moving or Copying File",
                                  QString("Failed to %1 '%2' to '%3'.\nThis may be because a file with the same name already exists at the destination.")
                                      .arg(action == Qt::CopyAction ? "copy" : "move")
                                      .arg(srcFileInfo.fileName())
                                      .arg(destDirectory));

        }
    }

    if (anySuccess) {
        QModelIndex parentIndex = parent.isValid() ? parent : this->index(rootPath());
        emit dataChanged(parentIndex, parentIndex);
        this->directoryLoaded(destDirectory); // Refresh the directory to show new files
    }

    return anySuccess;
}

