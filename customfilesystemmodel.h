#ifndef CUSTOMFILESYSTEMMODEL_H
#define CUSTOMFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include "tagmanager.h"
#include "config.hpp"
#include <QCryptographicHash>

class CustomFileSystemModel : public QFileSystemModel {
    Q_OBJECT

public:
    explicit CustomFileSystemModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setTagManager(TagManager *manager);
    void setFilterTag(const QString& tag);

    // Functions to support drag and drop of files
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    void moveTagsAndAutoSaveFile(const QString &srcFilePath, const QString &destFilePath);
    QString hashFilePath(const QString &path) const;
protected:

private:
    TagManager *tagManager;
    QString currentFilterTag;
};

#endif // CUSTOMFILESYSTEMMODEL_H
