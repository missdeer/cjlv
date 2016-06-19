#include <QtCore>
#include <QCoreApplication>
#include <QHeaderView>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QtConcurrent>
#include <QVBoxLayout>
#include <QTableView>
#include <QSplitter>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <JlCompress.h>
#include "ScintillaEdit.h"
#include "logmodel.h"
#include "logview.h"

static const QEvent::Type EXTRACTED_EVENT = QEvent::Type(QEvent::User + 1);

class ExtractedEvent : public QEvent
{
public:
    ExtractedEvent() : QEvent(EXTRACTED_EVENT) {}
};

LogView::LogView(QWidget *parent)
    : QWidget (parent)
    , m_verticalSplitter(new QSplitter( Qt::Vertical, parent))
    , m_tableView(new QTableView(m_verticalSplitter))
    , m_codeEditor(new ScintillaEdit(m_verticalSplitter))
    , m_model(new LogModel(m_tableView))
{
    m_verticalSplitter->addWidget(m_tableView);
    m_verticalSplitter->addWidget(m_codeEditor);

    QList<int> sizes;
    sizes << 0x7FFFF << 0;
    m_verticalSplitter->setSizes(sizes);

    QVBoxLayout* m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(m_verticalSplitter);
    setLayout(m_mainLayout);

    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);

    m_sc.initScintilla(m_codeEditor);
    m_sc.initEditorStyle(m_codeEditor);

    connect(this, &LogView::filter, m_model, &LogModel::onFilter);
    connect(m_tableView, &QAbstractItemView::doubleClicked, this, &LogView::onDoubleClicked);
}

LogView::~LogView()
{
    if (!m_extractDir.isEmpty())
    {
        QDir dir(m_extractDir);
        dir.removeRecursively();
    }
}

void LogView::openZipBundle(const QString &path)
{
    m_path = path;
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());

    m_extractDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    m_extractDir.append("/CiscoJabberLogs/" + fi.completeBaseName());
    QDir dir(m_extractDir);
    if (!dir.exists())
        dir.mkpath(m_extractDir);
    QtConcurrent::run(this, &LogView::extract, this, path, m_extractDir);
}

void LogView::openRawLogFile(const QStringList &paths)
{
    m_path = paths.join(":");
    QString path = paths.at(0);
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());

    m_model->loadFromFiles(paths);
}

void LogView::openFolder(const QString &path)
{
    m_path = path;
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());

    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Time);
    QStringList filters;
    filters << "jabber.log" << "jabber.log.*";
    dir.setNameFilters(filters);

    QStringList fileNames;
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        fileNames << fileInfo.filePath();
    }

    m_model->loadFromFiles(fileNames);
}

bool LogView::matched(const QString &path)
{
    return path == m_path;
}

bool LogView::matched(const QStringList &paths)
{
    return paths.join(":") == m_path;
}

void LogView::copyCurrentCell()
{
    QItemSelectionModel* selected =  m_tableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndex i = selected->currentIndex();

}

void LogView::copyCurrentRow()
{
    QItemSelectionModel* selected =  m_tableView->selectionModel();
    if (!selected->hasSelection())
        return;
    int row = selected->currentIndex().row();
    if (row >= 0)
    {
    }
}

void LogView::copySelectedCells()
{
    QItemSelectionModel* selected =  m_tableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();
}

void LogView::copySelectedRows()
{
    QItemSelectionModel* selected =  m_tableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();
}

void LogView::onDoubleClicked(const QModelIndex& index)
{
    if (index.column() == 7)// the content field
    {
        const QString& text = m_model->getText(index);
        // try to format XML document
        int startPos = text.indexOf(QChar('<'));
        int endPos = text.lastIndexOf(QChar('>'));
        if (startPos > 0 && endPos > startPos)
        {
            QString header = text.mid(0, startPos);
            QString xmlIn = text.mid(startPos, endPos - startPos + 1);

            QString xmlOut;

            QXmlStreamReader reader(xmlIn);
            QXmlStreamWriter writer(&xmlOut);
            writer.setAutoFormatting(true);

            while (!reader.atEnd()) {
                reader.readNext();
                if (!reader.isWhitespace()) {
                    writer.writeCurrentToken(reader);
                }
            }

            header.append("\n");
            header.append(xmlOut);
            m_codeEditor->setText(header.toLatin1().data());
        }
        else
        {
            m_codeEditor->setText(text.toLatin1().data());
        }
        m_codeEditor->emptyUndoBuffer();
        m_sc.initEditorStyle(m_codeEditor);
        m_codeEditor->colourise(0, -1);
    }
}

bool LogView::event(QEvent* e)
{
    QMutexLocker lock(&m_mutex);

    switch (int(e->type()))
    {
    case EXTRACTED_EVENT:
        {
            QDir dir(m_extractDir);
            dir.setFilter(QDir::Files | QDir::NoSymLinks);
            dir.setSorting(QDir::Name);
            QStringList filters;
            filters << "jabber.log" << "jabber.log.*";
            dir.setNameFilters(filters);

            QStringList fileNames;
            QFileInfoList list = dir.entryInfoList();
            for (int i = 0; i < list.size(); ++i) {
                QFileInfo fileInfo = list.at(i);
                fileNames << fileInfo.filePath();
            }

            m_model->loadFromFiles(fileNames);
        }
        return true;
    default:
        return QObject::event(e);
    }
}

void LogView::extract(LogView* v, const QString& fileName, const QString& dirName)
{
    ExtractedEvent* e = new ExtractedEvent;
    JlCompress::extractDir(fileName, dirName);
    QCoreApplication::postEvent(v, e);
}
