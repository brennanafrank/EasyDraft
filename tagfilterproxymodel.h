#ifndef TAGFILTERPROXYMODEL_H
#define TAGFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QRegularExpression>
#include "tagmanager.h"

class TagFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit TagFilterProxyModel(QObject *parent = nullptr);
    void setTagManager(TagManager *manager);
    void setCustomFilterRegExp(const QRegularExpression &regexp);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    TagManager *tagManager;
    QRegularExpression customFilterRegExp;
};

#endif // TAGFILTERPROXYMODEL_H
