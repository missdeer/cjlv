#include "stdafx.h"
#include "extension.h"

Extension::Extension(const Extension &e)
{
    m_title = e.m_title;
    m_author = e.m_author;
    m_field = e.m_field;
    m_method = e.m_method;
    m_content = e.m_content;
    m_createdAt = e.m_createdAt;
    m_lastModifiedAt = e.m_lastModifiedAt;
    m_uuid = e.m_uuid;
    m_from = e.m_from;

    m_path = e.m_path;
}

Extension &Extension::operator=(const Extension &e)
{
    m_title = e.m_title;
    m_author = e.m_author;
    m_field = e.m_field;
    m_method = e.m_method;
    m_content = e.m_content;
    m_createdAt = e.m_createdAt;
    m_lastModifiedAt = e.m_lastModifiedAt;
    m_uuid = e.m_uuid;
    m_from = e.m_from;

    m_path = e.m_path;
    return *this;
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
    setUuid(docElem.attribute("uuid"));
    setField(docElem.attribute("field"));
    setAuthor(docElem.attribute("author"));
    setTitle(docElem.attribute("title"));
    setMethod(docElem.attribute("method"));
    setCreatedAt(docElem.attribute("createAt"));
    setLastModifiedAt(docElem.attribute("lastModifiedAt"));

    QDomElement contentElem = docElem.firstChildElement("content");
    if (!contentElem.isNull())
        setContent(contentElem.text());

    m_path = path;
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
    root.setAttribute("method", m_method);
    root.setAttribute("createAt", m_createdAt);
    root.setAttribute("lastModifiedAt", m_lastModifiedAt);

    QDomElement contentElem = doc.createElement("content");
    QDomCDATASection contentCData = doc.createCDATASection(m_content);
    contentElem.appendChild(contentCData);
    root.appendChild(contentElem);

    QFile file(m_path);
    if (!file.open(QIODevice::WriteOnly))
        return ;
    QTextStream out(&file);
    const int IndentSize = 4;
    doc.save(out, IndentSize);
    file.close();
}

void Extension::destroy()
{
    QFile file(m_path);
    file.remove();
}

void Extension::changePathToCustomExtensionDirectory()
{
    m_path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/extensions";
    QDir dir(m_path);
    if (!dir.exists())
        dir.mkpath(m_path);
    m_path.append("/" % m_uuid % ".xml");
}
