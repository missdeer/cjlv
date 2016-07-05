#include <QDomDocument>
#include <QStandardPaths>
#include <QTextStream>
#include <QFile>
#include "extension.h"

Extension::Extension()
{

}

bool Extension::load(const QString& path)
{
    QDomDocument doc;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!doc.setContent(&file))
    {
        file.close();
        return false;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    setUuid( docElem.attribute("uuid"));
    setField( docElem.attribute("field"));
    setAuthor( docElem.attribute("author"));
    setTitle( docElem.attribute("title"));
    setCategory( docElem.attribute("category"));
    setCreatedAt( docElem.attribute("createAt"));
    setLastModifiedAt( docElem.attribute("lastModifiedAt"));

    QDomElement contentElem = docElem.firstChildElement("content");
    if (!contentElem.isNull())
        setContent(contentElem.text());

    return true;
}

void Extension::save()
{
    QDomDocument doc;
    QDomElement root = doc.createElement("extension");
    doc.appendChild(root);
    root.setAttribute("uuid", m_uuid);
    root.setAttribute("field", m_field);
    root.setAttribute("author", m_author);
    root.setAttribute("title", m_title);
    root.setAttribute("category", m_category);
    root.setAttribute("createAt", m_createdAt);
    root.setAttribute("lastModifiedAt", m_lastModifiedAt);

    QDomElement contentElem = doc.createElement("content");
    QDomCDATASection contentCData = doc.createCDATASection(m_content);
    contentElem.appendChild(contentCData);
    root.appendChild(contentElem);

    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/extensionos/" + m_title + ".xml";
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return ;
    QTextStream out(&file);
    const int IndentSize = 4;
    doc.save(out, IndentSize);
    file.close();
}

void Extension::destroy()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/extensionos/" + m_title + ".xml";
    QFile file(path);
    file.remove();
}

bool Extension::run()
{
    return false;
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
