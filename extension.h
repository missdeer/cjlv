#ifndef EXTENSION_H
#define EXTENSION_H

#include <QObject>
#include <QSharedPointer>

class Extension : public QObject
{
    Q_OBJECT
public:
    explicit Extension(QObject *parent = 0);

    const QString& title() const;
    void setTitle(const QString& title);

    const QString& author() const;
    void setAuthor(const QString& author);

    const QString& field() const;
    void setField(const QString& field);

    const QString& category() const;
    void setCategory(const QString& category);

    const QString& content() const;
    void setContent(const QString& content);

    const QString& createdAt() const;
    void setCreatedAt(const QString& createdAt);

    const QString& lastModifiedAt() const;
    void setLastModifiedAt(const QString& lastModifiedAt);

    const QString& uuid() const;
    void setUuid(const QString& uuid);

    const QString& from() const;
    void setFrom(const QString& from);

signals:

public slots:

private:
    QString m_title;
    QString m_author;
    QString m_field;
    QString m_category;
    QString m_content;
    QString m_createdAt;
    QString m_lastModifiedAt;
    QString m_uuid;
    QString m_from;
};

typedef QSharedPointer<Extension> ExtensionPtr;

#endif // EXTENSION_H
