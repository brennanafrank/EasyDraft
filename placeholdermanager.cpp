#include "placeholdermanager.h"
#include <QDateTime>
#include <QSettings>

PlaceholderManager::PlaceholderManager(QObject *parent) : QObject(parent)
{
}

void PlaceholderManager::addPlaceholder(const QString &name, const QString &value)
{
    placeholders[name] = value;
}

QString PlaceholderManager::getPlaceholderValue(const QString &name) const
{
    return placeholders.value(name);
}

QStringList PlaceholderManager::getPlaceholderNames() const
{
    return placeholders.keys();
}


void PlaceholderManager::addDefaultPlaceholders() {
    QString currentDateTime = QDateTime::currentDateTime().toString("MM-dd-yyyy HH:mm:ss");
    addPlaceholder("CurrentDateTime", currentDateTime);
}



void PlaceholderManager::savePlaceholders() {
    QSettings settings("YourOrganization", "YourApp");
    settings.beginGroup("Placeholders");
    settings.remove(""); // Clear existing settings before saving
    for (auto it = placeholders.constBegin(); it != placeholders.constEnd(); ++it) {
        settings.setValue(it.key(), it.value());
    }
    settings.endGroup();
}

void PlaceholderManager::loadPlaceholders() {
    QSettings settings("YourOrganization", "YourApp");
    settings.beginGroup("Placeholders");
    QStringList keys = settings.allKeys();
    for (const QString &key : keys) {
        QString value = settings.value(key).toString();
        placeholders.insert(key, value);
    }
    settings.endGroup();
}

void PlaceholderManager::removePlaceholder(const QString &name) {
    placeholders.remove(name); // Remove from the in-memory map

    // Now, update the persistent storage
    QSettings settings("YourOrganization", "YourApp");
    settings.beginGroup("Placeholders");
    settings.remove(name); // Remove the specific placeholder
    settings.endGroup();
}

