#include "stdafx.h"
#include <JlCompress.h>
#include <QPieSeries>
#include <QPieSlice>
#if defined(Q_OS_WIN)
#include "ShellContextMenu.h"
#endif
#include "sourcewindow.h"
#include "settings.h"
#include "presencewidget.h"
#include "logmodel.h"
#include "logview.h"

QT_CHARTS_USE_NAMESPACE

bool QuickGetFilesByFileName(const QString& fileName, QStringList& results);

#if defined(Q_OS_WIN)
extern QWinTaskbarButton *g_winTaskbarButton;
extern QWinTaskbarProgress *g_winTaskbarProgress;
#endif
SourceWindow* g_sourceWindow = nullptr;
static QProgressDialog* g_progressDialog = nullptr;
static QAtomicInt g_loadingReferenceCount;
static const QEvent::Type EXTRACTED_EVENT = QEvent::Type(QEvent::User + 1);

class ExtractedEvent : public QEvent
{
public:
    ExtractedEvent() : QEvent(EXTRACTED_EVENT) {}
};

LogView::LogView(QWidget *parent)
    : QWidget (parent)
    , m_verticalSplitter(new QSplitter( Qt::Vertical, parent))
    , m_logTableChartTabWidget(new QTabWidget(m_verticalSplitter))
    , m_logsTableView(new QTableView(m_logTableChartTabWidget))
    , m_levelStatisticChart(new QtCharts::QChartView(m_logTableChartTabWidget))
    , m_threadStatisticChart(new QtCharts::QChartView(m_logTableChartTabWidget))
    , m_sourceFileStatisticChart(new QtCharts::QChartView(m_logTableChartTabWidget))
    , m_sourceLineStatisticChart(new QtCharts::QChartView(m_logTableChartTabWidget))
    , m_categoryStatisticChart(new QtCharts::QChartView(m_logTableChartTabWidget))
    , m_methodStatisticChart(new QtCharts::QChartView(m_logTableChartTabWidget))
    , m_codeEditorTabWidget(new CodeEditorTabWidget(m_verticalSplitter))
    , m_logModel(new LogModel(m_logsTableView))
    , m_presenceWidget(new PresenceWidget(m_logTableChartTabWidget))
    , m_lastId(-1)
    , m_lastColumn(-1)
{
    m_verticalSplitter->addWidget(m_logTableChartTabWidget);
    m_verticalSplitter->addWidget(m_codeEditorTabWidget);

    QList<int> sizes;
    sizes << 0x7FFFF << 0;
    m_verticalSplitter->setSizes(sizes);

    QVBoxLayout* m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(m_verticalSplitter);
    setLayout(m_mainLayout);

    m_logTableChartTabWidget->setTabPosition(QTabWidget::South);
    m_logTableChartTabWidget->setTabsClosable(false);
    m_logTableChartTabWidget->setDocumentMode(true);
    m_logTableChartTabWidget->addTab(m_logsTableView, "Logs");
    m_logTableChartTabWidget->addTab(m_levelStatisticChart, "Level");
    m_logTableChartTabWidget->addTab(m_threadStatisticChart, "Thread");
    m_logTableChartTabWidget->addTab(m_sourceFileStatisticChart, "Source File");
    m_logTableChartTabWidget->addTab(m_sourceLineStatisticChart, "Source Line");
    m_logTableChartTabWidget->addTab(m_categoryStatisticChart, "Category");
    m_logTableChartTabWidget->addTab(m_methodStatisticChart, "Method");
    m_logTableChartTabWidget->addTab(m_presenceWidget, "Presence");
    m_logsTableView->setModel(m_logModel);
    m_logsTableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    m_logsTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_logsTableView, &QAbstractItemView::doubleClicked, this, &LogView::onDoubleClicked);
    connect(m_logsTableView, &QWidget::customContextMenuRequested, this, &LogView::onCustomContextMenuRequested);
    connect(m_logModel, &LogModel::dataLoaded, this, &LogView::onDataLoaded);
    connect(m_logModel, &LogModel::rowCountChanged, this, &LogView::onRowCountChanged);
    connect(this, &LogView::runExtension, m_logModel, &LogModel::runExtension);
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

    m_extractDir = g_settings->temporaryDirectory();
    if (m_extractDir.isEmpty())
    {
        m_extractDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) % "/CiscoJabberLogs";
    }
    m_extractDir.append("/" % fi.completeBaseName());
    QDir dir(m_extractDir);
    if (!dir.exists())
        dir.mkpath(m_extractDir);

    showProgressDialog();

    QtConcurrent::run(this, &LogView::extract, this, path, m_extractDir);
}

void LogView::openRawLogFile(const QStringList &paths)
{
    m_path = paths.join(":");
    QString path = paths.at(0);
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());

    showProgressDialog();

    m_logModel->loadFromFiles(paths);
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
    filters << "jabber.log" << "jabber.log.1" << "jabber.log.2" << "jabber.log.3" << "jabber.log.4" << "jabber.log.5";
    dir.setNameFilters(filters);

    QStringList fileNames;
    QFileInfoList list = dir.entryInfoList();
    std::for_each(list.begin(), list.end(),
                  [&fileNames](const QFileInfo& fileInfo){fileNames << fileInfo.filePath();});

    showProgressDialog();

    m_logModel->loadFromFiles(fileNames);
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
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndex i = selected->currentIndex();
    m_logModel->copyCell(i);
}

void LogView::copyCurrentRow()
{
    if (m_codeEditorTabWidget->copyable())
        m_codeEditorTabWidget->copy();
    else
    {
        QItemSelectionModel* selected =  m_logsTableView->selectionModel();
        if (!selected->hasSelection())
            return;
        int row = selected->currentIndex().row();
        m_logModel->copyRow(row);
    }
}

void LogView::copySelectedCells()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();
    m_logModel->copyCells(l);
}

void LogView::copySelectedRows()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
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
    m_logModel->copyRows(rows);
}

void LogView::scrollToTop()
{
    m_logsTableView->scrollToTop();
}

void LogView::scrollToBottom()
{
    m_logsTableView->scrollToBottom();
}

void LogView::gotoById(int i)
{
    m_logsTableView->scrollTo(m_logModel->index(i-1, 0));
}

void LogView::reload()
{
    m_logModel->reload();
}

int LogView::rowCount()
{
    return m_logModel->rowCount();
}

void LogView::showCodeEditorPane()
{
    const QList<int>& sizes = m_verticalSplitter->sizes();
    if (sizes.length() == 2 && sizes[1] < height()/10)
    {
        QList<int> resizes;
        resizes << height()/2 <<  height()/2;
        m_verticalSplitter->setSizes(resizes);
    }
}

void LogView::filter(const QString &keyword)
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (keyword.isEmpty() && selected && selected->hasSelection())
    {
        // record the selected cell
        QModelIndex i = selected->currentIndex();
        m_lastId = m_logModel->getId(i);
        m_lastColumn = i.column();
    }
    else
    {
        m_lastId = -1;
        m_lastColumn = -1;
    }
    m_logModel->onFilter(keyword);
}

void LogView::extractContent(const QModelIndex& index)
{
    showCodeEditorPane();

    const QString& text = m_logModel->getLogContent(index);
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
        m_codeEditorTabWidget->setContent(header);
    }
    else
    {
        m_codeEditorTabWidget->setContent(text);
    }
}

void LogView::openSourceFile(const QModelIndex &index, bool openWithBuiltinEditor)
{
    const QString& source = m_logModel->getLogSourceFile(index);

    QRegularExpression re("([^\\(]+)\\(([0-9]+)");
    QRegularExpressionMatch m = re.match(source);
    if (m.hasMatch())
    {
        QString s = m.captured(1);

        QString fileName(s);
        QStringList fileDirs;
        int i = s.lastIndexOf('/');
        if (i >= 0)
        {
            fileDirs = s.split('/');
            fileName = s.mid(i + 1, -1);
        }
        i = s.lastIndexOf('\\');
        if (i >= 0)
        {
            fileDirs = s.split('\\');
            fileName = s.mid(i + 1, -1);
        }

        QStringList results;
        if (!QuickGetFilesByFileName(fileName, results))
        {
            if (fileDirs.length() == 1 || fileDirs.isEmpty())
            {
                QString t(fileName);
                while (t.at(0) < QChar('A') || t.at(0) > QChar('Z'))
                    t = t.remove(0,1);

                if (!QuickGetFilesByFileName(t, results))
                {
                    QMessageBox::warning(this, "Warning", QString("Can't find the file named %1.").arg(fileName), QMessageBox::Ok);
                    return;
                }
            }
            else
            {
                QMessageBox::warning(this, "Warning", QString("Can't find the file named %1.").arg(fileName), QMessageBox::Ok);
                return;
            }
        }

        QDir srcDir(g_settings->sourceDirectory());
        Q_FOREACH(const QString& filePath, results)
        {
            QFileInfo fi(filePath);
            if (fileDirs.length() > 1)
            {
                if (fi.fileName().compare(fileName, Qt::CaseInsensitive) != 0)
                {
                    continue;
                }

                if (!filePath.toLower().contains(fileDirs.at(0).toLower()))
                {
                    fileDirs.removeFirst();
                }
            }
            QDir dir(fi.filePath());
            if (!g_settings->sourceDirectory().isEmpty())
                while (dir != srcDir)
                    if (!dir.cdUp())
                        break;
            bool matched = (fileDirs.end() == std::find_if(fileDirs.begin(), fileDirs.end(),
                                      [&filePath](const QString& n) {return (!filePath.toLower().contains(n.toLower()));}));

            if (matched && (g_settings->sourceDirectory().isEmpty() || (!g_settings->sourceDirectory().isEmpty() && dir == srcDir)))
            {
                if (openWithBuiltinEditor)
                {
                    if (g_settings->multiMonitorEnabled())
                    {
                        if (!g_sourceWindow)
                        {
                            QWidgetList widgets = qApp->topLevelWidgets();
                            auto it = std::find_if(widgets.begin(), widgets.end(),
                                                   [](QWidget* w){ return (w->objectName() == "MainWindow"); });
                            QWidget* mainWindow = nullptr;
                            if (widgets.end() != it)
                                mainWindow = *it;
                            else if (!widgets.isEmpty())
                                mainWindow = widgets.at(0);

                            g_sourceWindow = new SourceWindow(mainWindow);

                            g_sourceWindow->show();

                            QList<QScreen *> screens = QApplication::screens();
                            for (QScreen* s : screens)
                            {
                                if (s != mainWindow->windowHandle()->screen())
                                {
                                    g_sourceWindow->move(s->geometry().x(), s->geometry().y());
                                    g_sourceWindow->resize(s->geometry().width(), s->geometry().height());
                                    break;
                                }
                            }
                            g_sourceWindow->showMaximized();
                        }

                        if (!g_sourceWindow->isVisible())
                        {
                            // show
                            g_sourceWindow->showMaximized();
                        }
                        g_sourceWindow->gotoLine(m_path, filePath, m.captured(2).toInt());
                        g_sourceWindow->raise();
                    }
                    else
                    {
                        showCodeEditorPane();
                        m_codeEditorTabWidget->gotoLine(filePath, m.captured(2).toInt());
                    }
                }
                else
                {
                    int index = filePath.indexOf("components");
                    if (index == -1)
                        index = filePath.indexOf("products");
                    if (index == -1)
                        index = filePath.indexOf("services");
                    if (index >= 0)
                    {
                        QString url = QString("http://jmpopengrok.jabberqa.cisco.com:8080/source/xref/trunk/%1#%2")
                                .arg(filePath.mid(index).replace(QChar('\\'), QChar('/')))
                                .arg(m.captured(2).toInt());
                        QDesktopServices::openUrl(url);
                    }
                }
                return;
            }
        }

        QMessageBox::warning(this,
                             "Warning",
                             QString("Can't find the file named %1 in path %2.").arg(fileName).arg(source),
                             QMessageBox::Ok);
    }
}

void LogView::openLog(const QModelIndex &index)
{
    showCodeEditorPane();
    const QString& logFile = m_logModel->getLogFileName(index);
    m_codeEditorTabWidget->gotoLine(logFile);
}

void LogView::gotoLogLine(const QModelIndex &index)
{
    showCodeEditorPane();
    QString logFile;
    int line = m_logModel->getLogFileLine(index, logFile);
    m_codeEditorTabWidget->gotoLine(logFile, line);
}

void LogView::onDoubleClicked(const QModelIndex& index)
{
    switch (index.column())// the content field
    {
    case 4:
        openSourceFile(index, true);
        break;
    case 7:
        extractContent(index);
        break;
    case 8:
        openLog(index);
        break;
    case 9:
        gotoLogLine(index);
        break;
    default:
        break;
    }
}

void LogView::setChart(QtCharts::QChartView* chartView,const QList<QSharedPointer<StatisticItem> >& sis, const QString& label)
{
    QPieSeries *series = new QPieSeries();
    int totalCount = 0;
    std::for_each(sis.begin(), sis.end(), [&totalCount](QSharedPointer<StatisticItem> si){ totalCount += si->count;});
    bool visible = false;
    for (QSharedPointer<StatisticItem> si: sis)
    {
        QString label = QString("%1, %2, %3%").arg(si->content).arg(si->count).arg(si->percent, 0, 'f', 2);
        QPieSlice *slice = series->append(label, si->count);
        slice->setLabelVisible(visible = !visible);
    }

    int index = sis.length() - 1;
    if (sis.at(index)->count > sis.at(index - 1)->count)
        index--;
    QPieSlice *slice = series->slices().at(index);
    slice->setExploded();

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(label);
    chart->setTheme(QChart::ChartThemeQt);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

void LogView::onDataLoaded()
{
    closeProgressDialog();
    // draw charts

    QList<QSharedPointer<StatisticItem>> sis;
    if (m_logModel->getLevelStatistic(sis))
    {
        setChart(m_levelStatisticChart, sis, "Level Count Statistic");
        sis.clear();
    }
    if (m_logModel->getThreadStatistic(sis))
    {
        setChart(m_threadStatisticChart, sis, "Thread Count Statistic");
        sis.clear();
    }
    if (m_logModel->getSourceFileStatistic(sis))
    {
        setChart(m_sourceFileStatisticChart, sis, "Source File Count Statistic");
        sis.clear();
    }
    if (m_logModel->getSourceLineStatistic(sis))
    {
        setChart(m_sourceLineStatisticChart, sis, "Source Line Count Statistic");
        sis.clear();
    }
    if (m_logModel->getCategoryStatistic(sis))
    {
        setChart(m_categoryStatisticChart, sis, "Category Count Statistic");
        sis.clear();
    }
    if (m_logModel->getMethodStatistic(sis))
    {
        setChart(m_methodStatisticChart, sis, "Method Count Statistic");
        sis.clear();
    }
}

void LogView::onRowCountChanged()
{
    if (m_lastId >= 0 && m_lastColumn >= 0)
    {
        m_logsTableView->scrollTo(m_logModel->index(m_lastId -1, m_lastColumn));
        m_logsTableView->selectRow(m_lastId -1);
    }
    emit rowCountChanged();
}

void LogView::onCustomContextMenuRequested(const QPoint &pos)
{
    QItemSelectionModel* model = m_logsTableView->selectionModel();
    if (model && model->hasSelection())
    {
        QMenu menu(this);

        QAction* pOpengrokAction = new QAction("Browse Source File with OpenGrok", this);
        connect(pOpengrokAction, &QAction::triggered, this, &LogView::onBrowseSourceFileWithOpenGrok);
        menu.addAction(pOpengrokAction);

        QAction* pSourceFilePreviewAction = new QAction("Source File Preview", this);
        connect(pSourceFilePreviewAction, &QAction::triggered, this, &LogView::onSourceFilePreview);
        menu.addAction(pSourceFilePreviewAction);

        QAction* pContentPreviewAction = new QAction("Content Preview", this);
        connect(pContentPreviewAction, &QAction::triggered, this, &LogView::onContentPreview);
        menu.addAction(pContentPreviewAction);

        QAction* pLogFilePreviewAction = new QAction("Log File Preview", this);
        connect(pLogFilePreviewAction, &QAction::triggered, this, &LogView::onLogFilePreview);
        menu.addAction(pLogFilePreviewAction);

#if defined(Q_OS_WIN)
        CShellContextMenu scm;
        scm.ShowContextMenu(&menu, this, m_logsTableView->viewport()->mapToGlobal(pos), QDir::toNativeSeparators(m_path));
#else
        menu.exec(m_tableView->viewport()->mapToGlobal(pos));
#endif
    }
}

void LogView::onBrowseSourceFileWithOpenGrok()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        openSourceFile(selected->currentIndex(), false);
    }
}

void LogView::onSourceFilePreview()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        openSourceFile(selected->currentIndex(), true);
    }
}

void LogView::onContentPreview()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        extractContent(selected->currentIndex());
    }
}

void LogView::onLogFilePreview()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        gotoLogLine(selected->currentIndex());
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
            filters << "jabber.log" << "jabber.log.1" << "jabber.log.2" << "jabber.log.3" << "jabber.log.4" << "jabber.log.5";
            dir.setNameFilters(filters);

            QStringList fileNames;
            QFileInfoList list = dir.entryInfoList();

            std::for_each(list.begin(), list.end(),
                          [&fileNames](const QFileInfo& fileInfo){fileNames << fileInfo.filePath();});

            m_logModel->loadFromFiles(fileNames);
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

void LogView::showProgressDialog()
{
    g_loadingReferenceCount.ref();
#if defined(Q_OS_WIN)
    g_winTaskbarButton->setOverlayIcon(QIcon(":/image/loading.png"));

    g_winTaskbarProgress->setRange(0, 0);
    g_winTaskbarProgress->setVisible(true);
    qApp->processEvents();
#endif
    if (!g_progressDialog)
    {
        g_progressDialog = new QProgressDialog(this);
        g_progressDialog->setLabelText("Loading logs from files...");
        g_progressDialog->setWindowModality(Qt::WindowModal);
        g_progressDialog->setAutoClose(true);
        g_progressDialog->setAutoReset(true);
        g_progressDialog->setCancelButton(nullptr);
        g_progressDialog->setRange(0,0);
        g_progressDialog->setMinimumDuration(0);
    }
    g_progressDialog->show();
    qApp->processEvents();
}

void LogView::closeProgressDialog()
{
    if (!g_loadingReferenceCount.deref() && g_progressDialog)
    {
        g_progressDialog->setValue(200);
        g_progressDialog->deleteLater();
        g_progressDialog=nullptr;

#if defined(Q_OS_WIN)
        if (g_winTaskbarButton)
            g_winTaskbarButton->clearOverlayIcon();

        if (g_winTaskbarProgress)
            g_winTaskbarProgress->setVisible(false);
#endif
    }
}
