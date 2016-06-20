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
#include <QDomDocument>
#include <QTextStream>
#include <QProgressDialog>
#include <QTimer>
#include <JlCompress.h>
#include "ScintillaEdit.h"
#include "settings.h"
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
    , m_progressDialog(nullptr)
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
    connect(m_model, &LogModel::dataLoaded, this, &LogView::onDataLoaded);
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

    m_extractDir = g_settings.temporaryDirectory();
    if (m_extractDir.isEmpty())
    {
        m_extractDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/CiscoJabberLogs";
    }
    m_extractDir.append("/" + fi.completeBaseName());
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

    showProgressDialog();

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

    showProgressDialog();

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
    m_model->copyCell(i);
}

void LogView::copyCurrentRow()
{
    QItemSelectionModel* selected =  m_tableView->selectionModel();
    if (!selected->hasSelection())
        return;
    int row = selected->currentIndex().row();
    m_model->copyRow(row);
}

void LogView::copySelectedCells()
{
    QItemSelectionModel* selected =  m_tableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();
    m_model->copyCells(l);
}

void LogView::copySelectedRows()
{
    QItemSelectionModel* selected =  m_tableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();
    QList<int> rows;
    Q_FOREACH(const QModelIndex& i, l)
    {
        rows.append(i.row());
    }
    auto t = rows.toStdList();
    t.unique();
    rows = QList<int>::fromStdList(t);
    m_model->copyRows(rows);
}

void LogView::scrollToTop()
{
    m_tableView->scrollToTop();
}

void LogView::scrollToBottom()
{
    m_tableView->scrollToBottom();
}

void LogView::gotoById(int i)
{
    m_tableView->scrollTo(m_model->index(i-1, 0));
}

void LogView::reload()
{
    m_model->reload();
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
#ifndef QT_NO_DEBUG
            qDebug() << "raw text:" << text;
            qDebug() << "xml in:" << xmlIn;
#endif
            QString xmlOut;

            QDomDocument doc;
            doc.setContent(xmlIn, false);
            QTextStream writer(&xmlOut);
            doc.save(writer, 4);

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

        const QList<int>& sizes = m_verticalSplitter->sizes();
        if (sizes.length() == 2 && sizes[1] < height()/10)
        {
            QList<int> resizes;
            resizes << height()/2 <<  height()/2;
            m_verticalSplitter->setSizes(resizes);
        }
    }
}

void LogView::onDataLoaded()
{
    closeProgressDialog();
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
    showProgressDialog();

    ExtractedEvent* e = new ExtractedEvent;
    JlCompress::extractDir(fileName, dirName);
    QCoreApplication::postEvent(v, e);
}

void LogView::showProgressDialog()
{
    if (!m_progressDialog)
    {
        m_progressDialog = new QProgressDialog(this);
        m_progressDialog->setLabelText("Loading log from files...");
        m_progressDialog->setWindowModality(Qt::WindowModal);
        m_progressDialog->setAutoClose(true);
        m_progressDialog->setAutoReset(true);
        m_progressDialog->setCancelButton(nullptr);
        m_progressDialog->setRange(0,0);
        m_progressDialog->setMinimumDuration(0);
    }
    m_progressDialog->show();
    qApp->processEvents();
}

void LogView::closeProgressDialog()
{
    if (m_progressDialog)
    {
        m_progressDialog->setValue(200);
        m_progressDialog->deleteLater();
        m_progressDialog=nullptr;
    }
}
