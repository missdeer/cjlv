#ifndef EXTENSION_H
#define EXTENSION_H

#include <QString>
#include <QSharedPointer>

class Extension
{
public:
    Extension() = default;
    ~Extension() = default;
    Extension(const Extension& e);
    Extension& operator=(const Extension& e);

    bool load(const QString& path);
    bool save();
    void destroy();

    const QString& title() const;
    void setTitle(const QString& title);

    const QString& author() const;
    void setAuthor(const QString& author);

    const QString& field() const;
    void setField(const QString& field);

    const QString& method() const;
    void setMethod(const QString& method);

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

    const QString& shortcut() const;
    void setShortcut(const QString& shortcut);

    const QString& comment() const;
    void setComment(const QString& comment);

    void changePathToCustomExtensionDirectory();

    const QString& category() const;
    void setCategory(const QString &category);

private:
    QString m_title;
    QString m_author;
    QString m_field;
    QString m_method;
    QString m_content;
    QString m_createdAt;
    QString m_lastModifiedAt;
    QString m_uuid;
    QString m_from;
    QString m_comment;
    QString m_shortcut;
    QString m_category;

    QString m_path;
};

typedef QSharedPointer<Extension> ExtensionPtr;

#endif // EXTENSION_H
