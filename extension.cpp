#include "extension.h"

Extension::Extension(QObject *parent) : QObject(parent)
{

}

const QString& Extension::title() const
{
    return m_title;
}

void Extension::setTitle(const QString& title)
{
    m_title = title;
}

const QString& Extension::author() const
{
    return m_author;
}

void Extension::setAuthor(const QString& author)
{
    m_author = author;
}

const QString& Extension::field() const
{
    return m_field;
}

void Extension::setField(const QString& field)
{
    m_field = field;
}

const QString& Extension::category() const
{
    return m_category;
}

void Extension::setCategory(const QString& category)
{
    m_category = category;
}

const QString& Extension::content() const
{
    return m_content;
}

void Extension::setContent(const QString& content)
{
    m_content = content;
}

const QString& Extension::createdAt() const
{
    return m_createdAt;
}

void Extension::setCreatedAt(const QString& createdAt)
{
    m_createdAt = createdAt;
}

const QString& Extension::lastModifiedAt() const
{
    return m_lastModifiedAt;
}

void Extension::setLastModifiedAt(const QString& lastModifiedAt)
{
    m_lastModifiedAt = lastModifiedAt;
}

const QString& Extension::uuid() const
{
    return m_uuid;
}

void Extension::setUuid(const QString& uuid)
{
    m_uuid = uuid;
}

const QString& Extension::from() const
{
    return m_from;
}

void Extension::setFrom(const QString& from)
{
    m_from = from;
}
