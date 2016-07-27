#ifndef EXTENSION_H
#define EXTENSION_H

class Extension
{
public:
    Extension() = default;
    ~Extension() = default;
    Extension(const Extension& e);
    Extension& operator=(const Extension& e);

    bool load(const QString& path);
    void save();
    void destroy();

    const QString& title() const
    {
        return m_title;
    }
    void setTitle(const QString& title)
    {
        m_title = title;
    }

    const QString& author() const
    {
        return m_author;
    }
    void setAuthor(const QString& author)
    {
        m_author = author;
    }

    const QString& field() const
    {
        return m_field;
    }
    void setField(const QString& field)
    {
        m_field = field;
    }

    const QString& method() const
    {
        return m_method;
    }
    void setMethod(const QString& method)
    {
        m_method = method;
    }

    const QString& content() const
    {
        return m_content;
    }
    void setContent(const QString& content)
    {
        m_content = content;
    }

    const QString& createdAt() const
    {
        return m_createdAt;
    }
    void setCreatedAt(const QString& createdAt)
    {
        m_createdAt = createdAt;
    }

    const QString& lastModifiedAt() const
    {
        return m_lastModifiedAt;
    }
    void setLastModifiedAt(const QString& lastModifiedAt)
    {
        m_lastModifiedAt = lastModifiedAt;
    }

    const QString& uuid() const
    {
        return m_uuid;
    }
    void setUuid(const QString& uuid)
    {
        m_uuid = uuid;
    }

    const QString& from() const
    {
        return m_from;
    }
    void setFrom(const QString& from)
    {
        m_from = from;
    }

    void changePathToCustomExtensionDirectory();

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

    QString m_path;
};

typedef QSharedPointer<Extension> ExtensionPtr;

#endif // EXTENSION_H
