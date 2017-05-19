#include "stdafx.h"
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
    return 8;
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
        return QVariant(r->method());
    case 4:
        return QVariant(r->from());
    case 5:
        return QVariant(r->shortcut());
    case 6:
        return QVariant(r->comment());
    case 7:
        return QVariant(r->content());
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
            return QVariant(tr("Method"));
        case 4:
            return QVariant(tr("From"));
        case 5:
            return QVariant(tr("Shortcut"));
        case 6:
            return QVariant(tr("Comment"));
        case 7:
            return QVariant(tr("Content"));
        default:
            break;
        }

    }

    return QVariant();
}

void ExtensionModel::scanExtensions()
{
    QtConcurrent::run(this, &ExtensionModel::doScanExtensions);
}

void ExtensionModel::doScanExtensions()
{
#if defined(Q_OS_WIN)
    QString path = QApplication::applicationDirPath() % "/extensions";
#elif defined(Q_OS_MAC)
    QString path = QApplication::applicationDirPath() % "/../PlugIns/extensions";
#endif
    path = ":/extensions";
    scanExtensionsFromDirectory(path, "Built-in");

    path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/extensions";
    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(path);
    else
        scanExtensionsFromDirectory(path, "Custom");

    emit extensionScanned();
}

ExtensionPtr ExtensionModel::extensionByUuid(const QString& uuid)
{
    auto it = std::find_if(m_extensions.begin(),
                 m_extensions.end(),
                 [=](ExtensionPtr e){return e->uuid() == uuid;});

    return m_extensions.end() != it ? *it : ExtensionPtr();
}

void ExtensionModel::removeExtension(ExtensionPtr e)
{
    auto it = std::find_if(m_extensions.begin(),
                 m_extensions.end(),
                 [e](ExtensionPtr ie){return ie->uuid() == e->uuid();});
    if (m_extensions.end() != it)
    {
        emit extensionRemoved(e);
        auto i = std::distance(m_extensions.begin(), it);
        beginRemoveRows(QModelIndex(), i, i);
        m_extensions.removeAt(i);
        endRemoveRows();
    }
}

void ExtensionModel::updateExtension(ExtensionPtr e)
{
    auto it = std::find_if(m_extensions.begin(),
                 m_extensions.end(),
                 [e](ExtensionPtr ie){return ie->uuid() == e->uuid();});
    if (m_extensions.end() != it)
    {
        // modify
        *it = e;
        auto i = std::distance(m_extensions.begin(), it);
        emit dataChanged(index(i, 0), index(i, 0));
        emit extensionModified(e);
        return;
    }
    // append
    beginInsertRows(QModelIndex(),  m_extensions.size(), m_extensions.size());
    m_extensions.push_back(e);
    endInsertRows();
    emit extensionCreated(e);
}

ExtensionPtr ExtensionModel::extension(const QModelIndex &index)
{
    if (index.row() < 0 || index.row() >= m_extensions.length())
        return ExtensionPtr();

    return m_extensions.at(index.row());
}

void ExtensionModel::onExtensionScanned()
{
    emit dataChanged(index(0, 0), index(m_extensions.length() -1, 0));
}

ExtensionModel::ExtensionModel(QObject* parent)
    :QAbstractTableModel (parent)
{
    connect(this, &ExtensionModel::extensionScanned, this, &ExtensionModel::onExtensionScanned);
}

void ExtensionModel::scanExtensionsFromDirectory(const QString& path, const QString& from)
{
    QDir dir(path);
    QStringList nameFilters = QStringList() << "*.xml";
    QFileInfoList fil = dir.entryInfoList(nameFilters, QDir::Files | QDir::NoDotAndDotDot);
    for(const QFileInfo& fi : fil)
    {
        ExtensionPtr e(new Extension);

        if (e->load(fi.absoluteFilePath()))
        {
            e->setFrom(from);
            m_extensions.push_back(e);
        }
    }
}
