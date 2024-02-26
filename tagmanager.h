#ifndef TAGMANAGER_H
#define TAGMANAGER_H

#include <QString>
#include <QStringList>
#include <QJsonObject>

class TagManager {
public:
    explicit TagManager(const QString &configFilePath);
    void addTag(const QString &filePath, const QString &tag);
    void removeTag(const QString &filePath, const QString &tag);
    QStringList getTags(const QString &filePath);

private:
    QString configFilePath;
    QJsonObject tagData;

    void loadTags();
    void saveTags();
};


#endif // TAGMANAGER_H
