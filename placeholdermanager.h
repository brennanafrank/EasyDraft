#ifndef PLACEHOLDERMANAGER_H
#define PLACEHOLDERMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>

class PlaceholderManager : public QObject
{
    Q_OBJECT

public:
    explicit PlaceholderManager(QObject *parent = nullptr);

    // Adds a new placeholder
    void addPlaceholder(const QString &name, const QString &value);

    // Returns the placeholder value for the given name
    QString getPlaceholderValue(const QString &name) const;

    // Returns all placeholder names
    QStringList getPlaceholderNames() const;

    // Include this function declaration
    void addDefaultPlaceholders();

    void savePlaceholders();
    void loadPlaceholders();
    void removePlaceholder(const QString &name);



private:
    QMap<QString, QString> placeholders; // Store placeholder names and values
};

#endif // PLACEHOLDERMANAGER_H
