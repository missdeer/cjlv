#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDomDocument>
#include "extensionmodel.h"

ExtensionModel* ExtensionModel::m_instance = nullptr;

ExtensionModel* ExtensionModel::instance(QObject* parent)
{
    if (!m_instance)
    {
        m_instance = new ExtensionModel(parent);
    }

    return m_instance;
}

QModelIndex ExtensionModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
            return QModelIndex();

    return createIndex(row, column);
}

int ExtensionModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_extensions.length();
}

int ExtensionModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 5;
}

QVariant ExtensionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.row() < 0 || index.row() >= m_extensions.length())
        return QVariant();

    ExtensionPtr r = m_extensions.at(index.row());
    switch (index.column())
    {
    case 0:
        return QVariant(r->title());
    case 1:
        return QVariant(r->author());
    case 2:
        return QVariant(r->field());
    case 3:
        return QVariant(r->category());
    case 4:
        return QVariant(r->from());
    default:
        break;
    }

    return QVariant();
}

Qt::ItemFlags ExtensionModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant ExtensionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {

        case 0:
            return QVariant(tr("Title"));
        case 1:
            return QVariant(tr("Author"));
        case 2:
            return QVariant(tr("Field"));
        case 3:
            return QVariant(tr("Category"));
        case 4:
            return QVariant(tr("From"));
        default:
            break;
        }

    }

    return QVariant();
}

void ExtensionModel::scanExtensions()
{
#if defined(Q_OS_WIN)
    QString path = QApplication::applicationDirPath() + "/extensions";
#elif defined(Q_OS_MAC)
    QString path = QApplication::applicationDirPath() + "/../PlugIns/extensions";
#endif
    scanExtensionsFromDirectory(path, "Built-in");

    path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/extensionos";
    scanExtensionsFromDirectory(path, "Custom");
}

ExtensionModel::ExtensionModel(QObject* parent)
    :QAbstractTableModel (parent)
{

}

void ExtensionModel::scanExtensionsFromDirectory(const QString& path, const QString& from)
{
    QDir dir(path);
    QStringList nameFilters = QStringList() << "*.xml";
    QFileInfoList fil = dir.entryInfoList(nameFilters, QDir::Files | QDir::NoDotAndDotDot);
    Q_FOREACH(const QFileInfo& fi, fil)
    {
        QDomDocument doc;
        QFile file(fi.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly))
            continue;
        if (!doc.setContent(&file))
        {
            file.close();
            continue;
        }
        ExtensionPtr e(new Extension);
        e->setFrom(from);

        QDomElement docElem = doc.documentElement();
        e->setUuid( docElem.attribute("uuid"));
        e->setField( docElem.attribute("field"));
        e->setAuthor( docElem.attribute("author"));
        e->setTitle( docElem.attribute("title"));
        e->setCategory( docElem.attribute("category"));
        e->setCreatedAt( docElem.attribute("createAt"));
        e->setLastModifiedAt( docElem.attribute("lastModifiedAt"));

        QDomElement contentElem = docElem.firstChildElement("content");
        if (!contentElem.isNull())
            e->setContent(contentElem.text());

        file.close();

        m_extensions.push_back(e);
    }
}
