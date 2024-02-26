#include "tagfilterproxymodel.h"
#include <QFileSystemModel>
#include <QSortFilterProxyModel>


TagFilterProxyModel::TagFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent), tagManager(nullptr) {}

void TagFilterProxyModel::setTagManager(TagManager *manager) {
    tagManager = manager;
}

void TagFilterProxyModel::setCustomFilterRegExp(const QRegularExpression &regexp) {
    customFilterRegExp = regexp;
}

bool TagFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    if (tagManager == nullptr) {
        return true;
    }

    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QString filePath = sourceModel()->data(index, QFileSystemModel::FilePathRole).toString();
    QStringList tags = tagManager->getTags(filePath);

    return customFilterRegExp.pattern().isEmpty() || tags.contains(customFilterRegExp.pattern());
}

