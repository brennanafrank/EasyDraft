#ifndef CUSTOMFILESYSTEMMODEL_H
#define CUSTOMFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include "tagmanager.h"

class CustomFileSystemModel : public QFileSystemModel {
    Q_OBJECT

public:
    explicit CustomFileSystemModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setTagManager(TagManager *manager);
    void setFilterTag(const QString& tag);

protected:

private:
    TagManager *tagManager;
    QString currentFilterTag;
};

#endif // CUSTOMFILESYSTEMMODEL_H
