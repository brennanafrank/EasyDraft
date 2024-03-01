#include "tagmanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QIODevice>


TagManager::TagManager(const QString &configFilePath) : configFilePath(configFilePath) {
    loadTags();
}

void TagManager::loadTags() {
    QFile file(configFilePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = file.readAll();
        QJsonDocument doc(QJsonDocument::fromJson(fileData));
        tagData = doc.object();
    }
}

void TagManager::saveTags() {
    QFile file(configFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(tagData);
        file.write(doc.toJson());
    }
}

void TagManager::addTag(const QString &filePath, const QString &tag) {
    QJsonArray tags = tagData.value(filePath).toArray();
    if (!tags.contains(tag)) {
        tags.append(tag);
        tagData.insert(filePath, tags);
        saveTags();
    }
}

void TagManager::removeTag(const QString &filePath, const QString &tag) {
    QJsonArray tags = tagData.value(filePath).toArray();
    QJsonArray newTags; // Create a new JSON array without the tags to be removed
    for (int i = 0; i < tags.size(); ++i) {
        if (tags[i].toString() != tag) {
            newTags.append(tags[i]);
        }
    }
    // Update and save label data if changes have occurred
    if (tags.size() != newTags.size()) {
        tagData.insert(filePath, newTags);
        saveTags();
    }
}


QStringList TagManager::getTags(const QString &filePath) {
    QJsonArray tags = tagData.value(filePath).toArray();
    QStringList tagList;
    for (const QJsonValue &value : tags) {
        tagList.append(value.toString());
    }
    // qDebug() << "Tags for" << filePath << ":" << tagList;
    return tagList;
}

\

    QStringList TagManager::getFilesByTag(const QString& tag) const {
    QStringList filePaths;
    for (auto it = tagData.begin(); it != tagData.end(); ++it) {
        QJsonArray tags = it.value().toArray();
        if (tags.contains(tag)) {
            filePaths.append(it.key());
        }
    }
    return filePaths;
}

