#ifndef CUSTOMFILESYSTEMMODEL_H
#define CUSTOMFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include "tagmanager.h"

class CustomFileSystemModel : public QFileSystemModel {
    Q_OBJECT

public:
    explicit CustomFileSystemModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setTagManager(TagManager *manager);

    void setFilterTag(const QString& tag);
protected:
           // bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
private:
    TagManager *tagManager;
    QString currentFilterTag;
};

#endif // CUSTOMFILESYSTEMMODEL_H
