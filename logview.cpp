#include "stdafx.h"
#include <functional>
#include <QPieSeries>
#include <QPieSlice>
#include <private/qzipreader_p.h>
#if defined(Q_OS_WIN)
#include "ShellContextMenu.h"
#endif
#include "mainwindow.h"
#include "preferencedialog.h"
#include "tabwidget.h"
#include "sourcewindow.h"
#include "settings.h"
#include "presencewidget.h"
#include "quickwidgetapi.h"
#include "utils.h"
#include "logmodel.h"
#include "logtableview.h"
#include "logview.h"

QT_CHARTS_USE_NAMESPACE

bool QuickGetFilesByFileName(const QString& fileName, QStringList& results);

#if defined(Q_OS_WIN)
extern QWinTaskbarButton *g_winTaskbarButton;
extern QWinTaskbarProgress *g_winTaskbarProgress;
#endif
SourceWindow* g_sourceWindow = nullptr;

static const QEvent::Type EXTRACTED_EVENT = QEvent::Type(QEvent::User + 1);
static const QEvent::Type EXTRACT_FAILED_EVENT = QEvent::Type(QEvent::User + 2);

class ExtractedEvent : public QEvent
{
public:
    ExtractedEvent() : QEvent(EXTRACTED_EVENT) {}
};

class ExtractFailedEvent : public QEvent
{
public:
	ExtractFailedEvent() 
		: QEvent(EXTRACT_FAILED_EVENT) 
	{}
	QString m_fileName;
	QString m_dirName;
};


LogView::LogView(QWidget *parent)
    : QWidget (parent)
    , m_sqlite3Helper(new Sqlite3Helper)
    , m_verticalSplitter(new QSplitter( Qt::Vertical, this))
    , m_logTableChartTabWidget(new QTabWidget(m_verticalSplitter))
    , m_codeEditorTabWidget(new CodeEditorTabWidget(m_verticalSplitter))
{
    initialize();
}

LogView::~LogView()
{
    if (!m_extractDir.isEmpty())
    {
        QDir dir(m_extractDir);
        dir.removeRecursively();
    }
}

Sqlite3HelperPtr LogView::getSqlite3Helper()
{
    return m_sqlite3Helper;
}

void LogView::openZipBundle(const QString &zipBundle, const QString &crashInfo)
{
    m_crashInfo = crashInfo;
    openZipBundle(zipBundle);
}

void LogView::openZipBundle(const QString &path)
{
    setPath(path);
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

    getMainWindow()->showProgressDialog(QString(tr("Loading logs from ZIP bundle %1...")).arg(path));

	QtConcurrent::run(this, &LogView::extract, this, path, m_extractDir);
}

void LogView::openRawLogFile(const QStringList &paths)
{
    setPath(paths.join(":"));
    QString path = paths.at(0);
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());

    //getMainWindow()->showProgressDialog(QString(tr("Loading logs from raw log files: %1...")).arg(paths.join(",")));

    for(auto ltv : m_logTableViews)
        ltv->loadFromFiles(paths);
}

void LogView::openFolder(const QString &path)
{
    setPath(path);
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());

    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Time);
    QStringList filters;
    filters << "jabber.log" << "jabber.log.1" << "jabber.log.2" << "jabber.log.3" << "jabber.log.4" << "jabber.log.5"
               << "jabber.log.6" << "jabber.log.7" << "jabber.log.8" << "jabber.log.9" << "jabber.log.10";
    dir.setNameFilters(filters);

    QStringList fileNames;
    QFileInfoList list = dir.entryInfoList();
    std::for_each(list.begin(), list.end(),
                  [&fileNames](const QFileInfo& fileInfo){fileNames << fileInfo.filePath();});

    //getMainWindow()->showProgressDialog(QString(tr("Loading logs from folder %1...")).arg(path));

    for(auto ltv : m_logTableViews)
        ltv->loadFromFiles(fileNames);
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
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->copyCurrentCell();
    }
}

void LogView::copyCurrentRow()
{
    if (m_codeEditorTabWidget->copyable())
        m_codeEditorTabWidget->copy();
    else
    {
        QWidget* w = m_logTableChartTabWidget->currentWidget();
        if (w)
        {
            LogTableView* ltv = qobject_cast<LogTableView*>(w);
            if (ltv)
                ltv->copyCurrentRow();
        }
    }
}

void LogView::copySelectedCells()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->copySelectedCells();
    }
}

void LogView::copySelectedRows()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->copySelectedRows();
    }
}

void LogView::copyCurrentCellWithXMLFormatted()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->copyCurrentCellWithXMLFormatted();
    }
}

void LogView::copyCurrentRowWithXMLFormatted()
{
    if (m_codeEditorTabWidget->copyable())
        m_codeEditorTabWidget->copy();
    else
    {
        QWidget* w = m_logTableChartTabWidget->currentWidget();
        if (w)
        {
            LogTableView* ltv = qobject_cast<LogTableView*>(w);
            if (ltv)
                ltv->copyCurrentRowWithXMLFormatted();
        }
    }
}

void LogView::copySelectedCellsWithXMLFormatted()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->copySelectedCellsWithXMLFormatted();
    }
}

void LogView::copySelectedRowsWithXMLFormatted()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->copySelectedRowsWithXMLFormatted();
    }
}

void LogView::addCurrentRowToBookmark()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->addCurrentRowToBookmark();
    }
}

void LogView::removeCurrentRowFromBookmark()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->removeCurrentRowFromBookmark();
    }
}

void LogView::addSelectedRowsToBookmark()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->addSelectedRowsToBookmark();
    }
}

void LogView::removeSelectedRowsFromBookmark()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->removeSelectedRowsFromBookmark();
    }
}

void LogView::removeAllBookmarks()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->removeAllBookmarks();
    }
}

void LogView::gotoFirstBookmark()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->gotoFirstBookmark();
    }
}

void LogView::gotoPreviousBookmark()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->gotoPreviousBookmark();
    }
}

void LogView::gotoNextBookmark()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->gotoNextBookmark();
    }
}

void LogView::gotoLastBookmark()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->gotoLastBookmark();
    }
}

void LogView::scrollToTop()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->scrollToTop();
    }
}

void LogView::scrollToBottom()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->scrollToBottom();
    }
}

void LogView::gotoById(int i)
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->gotoById(i);
    }
}

int LogView::rowCount()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            return ltv->rowCount();
    }
    return 0;
}

void LogView::showCodeEditorPane()
{
    if (g_settings->multiMonitorEnabled())
    {
        if (!g_sourceWindow)
        {
            MainWindow* mainWindow = getMainWindow();

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
            g_sourceWindow->setMainTabWidget(mainWindow->getMainTabWidget());
        }

        if (!g_sourceWindow->isVisible())
        {
            // show
            g_sourceWindow->showMaximized();
        }
        g_sourceWindow->raise();
    }
    else
    {
        const QList<int>& sizes = m_verticalSplitter->sizes();
        if (sizes.length() == 2 && sizes[1] < height()/10)
        {
            QList<int> resizes;
            resizes << height()/2 <<  height()/2;
            m_verticalSplitter->setSizes(resizes);
        }
    }
}

void LogView::extractContent(const QString& text)
{
    showCodeEditorPane();

    if (g_settings->multiMonitorEnabled())
    {
        g_sourceWindow->getSourceViewTabWidget()->setContent(m_path, text);
    }
    else
    {
        m_codeEditorTabWidget->setContent(text);
    }
}

void LogView::openLog(const QString &logFile)
{
    showCodeEditorPane();

    if (g_settings->multiMonitorEnabled())
    {
        g_sourceWindow->getSourceViewTabWidget()->gotoLine(m_path, logFile);
    }
    else
    {
        m_codeEditorTabWidget->gotoLine(logFile);
    }
}

void LogView::gotoLogLine(int line, const QString &logFile)
{
    showCodeEditorPane();
    if (g_settings->multiMonitorEnabled())
    {
        g_sourceWindow->getSourceViewTabWidget()->gotoLine(m_path, logFile, line);
    }
    else
    {
        m_codeEditorTabWidget->gotoLine(logFile, line);
    }
}

void LogView::setChart(QtCharts::QChartView* chartView,const QList<QSharedPointer<StatisticItem> >& sis, const QString& label)
{
    QPieSeries *series = new QPieSeries();
    int totalCount = 0;
    std::for_each(sis.begin(), sis.end(), [&totalCount](QSharedPointer<StatisticItem> si){ totalCount += si->count;});
    bool visible = false;
    for (auto si: sis)
    {
        QString label = QString("%1, %2, %3%").arg(si->content).arg(si->count).arg(si->percent, 0, 'f', 2);
        QPieSlice *slice = series->append(label, si->count);
        slice->setLabelVisible(sis.length() > 20 ? (visible = !visible) : true);
    }

    int index = sis.length() - 1;
    if (sis.length() > 1 && sis.at(index)->count > sis.at(index - 1)->count)
        index--;
    QPieSlice *slice = series->slices().at(index);
    slice->setExploded();

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(label);
    chart->setTheme(QChart::ChartThemeQt);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    QChart *oldChart = chartView->chart();
    if (!oldChart)
        oldChart->deleteLater();
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

MainWindow *LogView::getMainWindow()
{
    QWidgetList widgets = qApp->topLevelWidgets();
    auto it = std::find_if(widgets.begin(), widgets.end(),
                           [](QWidget* w){ return (w->objectName() == "MainWindow"); });
    QWidget* mainWindow = nullptr;
    if (widgets.end() != it)
        mainWindow = *it;
    else if (!widgets.isEmpty())
        mainWindow = widgets.at(0);
    return (MainWindow*)mainWindow;
}

void LogView::openCrashReport()
{
    if (QFile::exists(m_crashInfo))
    {
        showCodeEditorPane();
        if (g_settings->multiMonitorEnabled())
        {
            g_sourceWindow->getSourceViewTabWidget()->gotoLine(m_path, m_crashInfo);
        }
        else
        {
            m_codeEditorTabWidget->gotoLine(m_crashInfo);
        }
    }

    QString dirPath;
    if (QFileInfo(m_path).isDir())
        dirPath = m_path;
    if (QFileInfo(m_path).suffix().toLower() == "zip")
        dirPath = m_extractDir;

    QDir dir(dirPath);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QStringList filters;
    filters << "*.crash";
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();
    if (!list.isEmpty())
    {
        QStringList fileNames;
        std::for_each(list.begin(), list.end(), [&](const QFileInfo& fi) {
            fileNames << fi.fileName();
        });
        if (QMessageBox::question(this, tr("Found crash report"),
                                  QString(tr("Found crash report %1, open it now?")).arg(fileNames.join(", ")),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes) == QMessageBox::Yes)
        {
            showCodeEditorPane();
            std::for_each(list.begin(), list.end(), [&](const QFileInfo& fi) {
                if (g_settings->multiMonitorEnabled())
                {
                    g_sourceWindow->getSourceViewTabWidget()->gotoLine(m_path, fi.absoluteFilePath());
                }
                else
                {
                    m_codeEditorTabWidget->gotoLine(fi.absoluteFilePath());
                }
            });
        }
    }

#if defined(Q_OS_WIN)
    filters.clear();
    filters << "*.dmp";
    dir.setNameFilters(filters);

    list = dir.entryInfoList();
    if (!list.isEmpty())
    {
        QStringList fileNames;
        std::for_each(list.begin(), list.end(), [&](const QFileInfo& fi) {
            fileNames << fi.fileName();
        });
        if (QMessageBox::question(this, tr("Found crash report"),
                                  QString(tr("Found crash report %1, open it by WinDBG now?")).arg(fileNames.join(", ")),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes) == QMessageBox::Yes)
        {
            const QString& windbgPath = g_settings->windbgPath();
            if (windbgPath.isEmpty())
            {
                QMessageBox::warning(this, tr("WinDBG path not found"),
                                     tr("Please set WinDBG path first."),
                                     QMessageBox::Ok);
                PreferenceDialog dlg(this);
                dlg.exec();
                if (windbgPath.isEmpty())
                    return;
            }
            std::for_each(list.begin(), list.end(), [&](const QFileInfo& fi) {
                QStringList args;
                args << "-z" << fi.absoluteFilePath().replace('/', QDir::separator()) << "-c" << "!analyze -v";
                const QString& srcPath = g_settings->sourceDirectory();
                if (!srcPath.isEmpty())
                    args << "-srcpath" << srcPath;
                QProcess::startDetached(windbgPath, args);
            });
        }
    }
#endif
}

void LogView::onOpenSourceFileWithBuiltinEditor(const QString &filePath, int line)
{
    showCodeEditorPane();
    if (g_settings->multiMonitorEnabled())
    {
        g_sourceWindow->getSourceViewTabWidget()->gotoLine(m_path, filePath, line);
    }
    else
    {
        m_codeEditorTabWidget->gotoLine(filePath, line);
    }
}

void LogView::onOpenSourceFileInVS(const QString &filePath, int line)
{
#if defined(Q_OS_WIN)
    // extract resource file
    QString localPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/open-in-vs.vbs";
    QFile::copy(":/open-in-vs.vbs" , localPath);

    // run
    QString arg = QString("\"%1\" %2 0").arg(QDir::toNativeSeparators(filePath)).arg(line);
    ::ShellExecuteW(NULL, L"open", localPath.toStdWString().c_str(), arg.toStdWString().c_str(), NULL, SW_SHOWNORMAL);
#else
    Q_UNUSED(filePath);
    Q_UNUSED(line);
#endif
}

void LogView::onOpenSourceFileWithOpenGrok(const QString &filePath, int line)
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
                .arg(line);
        QDesktopServices::openUrl(url);
    }
}

void LogView::initialize()
{
    m_verticalSplitter->addWidget(m_logTableChartTabWidget);
    m_verticalSplitter->addWidget(m_codeEditorTabWidget);

    QList<int> sizes;
    sizes << 0x7FFFF << 0;
    m_verticalSplitter->setSizes(sizes);

    m_logTableChartTabWidget->setTabPosition(QTabWidget::South);
    m_logTableChartTabWidget->setTabsClosable(true);
    m_logTableChartTabWidget->setDocumentMode(true);
    m_logTableChartTabWidget->setTabBarAutoHide(true);

    createLogTableView();

    connect(m_logTableChartTabWidget, &QTabWidget::currentChanged, this, &LogView::onLogTableChartTabWidgetCurrentChanged);
    connect(m_logTableChartTabWidget, &QTabWidget::tabCloseRequested, this, &LogView::onCloseLogTableChartTabWidgetTab);
    connect(m_logTableChartTabWidget, &QTabWidget::tabBarDoubleClicked, this, &LogView::onCloseLogTableChartTabWidgetTab);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    Q_ASSERT(mainLayout);
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_verticalSplitter);
    setLayout(mainLayout);
}

LogTableView *LogView::createLogTableView()
{
    LogTableView *ltv = new LogTableView(m_logTableChartTabWidget, m_sqlite3Helper);
    connect(ltv, &LogTableView::dataLoaded, this, &LogView::onDataLoaded);
    connect(ltv, &LogTableView::rowCountChanged, this, &LogView::rowCountChanged);
    connect(ltv, &LogTableView::runExtension, this, &LogView::runExtension);
    connect(ltv, &LogTableView::gotoLogLine, this, &LogView::gotoLogLine);
    connect(ltv, &LogTableView::extractContent, this, &LogView::extractContent);
    connect(ltv, &LogTableView::openLog, this, &LogView::openLog);
    connect(ltv, &LogTableView::openSourceFileInVS, this, &LogView::onOpenSourceFileInVS);
    connect(ltv, &LogTableView::openSourceFileWithBuiltinEditor, this, &LogView::onOpenSourceFileWithBuiltinEditor);
    connect(ltv, &LogTableView::openSourceFileWithOpenGrok, this, &LogView::onOpenSourceFileWithOpenGrok);
	for (int i = 0; i < m_logTableChartTabWidget->count(); i++)
	{
		auto w = m_logTableChartTabWidget->widget(i);
		auto pw = qobject_cast<PresenceWidget*>(w);
		if (pw)
			connect(ltv, &LogTableView::databaseCreated, pw, &PresenceWidget::databaseCreated);
	}
    m_logTableViews.append(ltv);
    m_logTableViewNr++;
    m_logTableChartTabWidget->addTab(ltv, QString("Logs(%1)").arg(m_logTableViewNr));
	m_logTableChartTabWidget->setCurrentWidget(ltv);
    return ltv;
}

void LogView::onCloseLogTableChartTabWidgetTab(int index)
{
    auto w = m_logTableChartTabWidget->widget(index);
    auto ltv = qobject_cast<LogTableView*>(w);
    if (ltv)
    {
        if (m_logTableViews.size() == 1)
            return; // don't remove the last LogTableView tab
        m_logTableViews.removeAll(ltv);
    }
    m_logTableChartTabWidget->removeTab(index);
    delete w;
}

void LogView::onDataLoaded()
{
    //getMainWindow()->closeProgressDialog();

    //QTimer::singleShot(100, [&](){
        openCrashReport();
    //});
}

void LogView::searchFieldContent()
{
    for(auto ltv : m_logTableViews)
        ltv->searchFieldContent();
}

void LogView::searchFieldID()
{
    for(auto ltv : m_logTableViews)
        ltv->searchFieldID();
}

void LogView::searchFieldDateTime()
{
    for(auto ltv : m_logTableViews)
        ltv->searchFieldDateTime();
}

void LogView::searchFieldThread()
{
    for(auto ltv : m_logTableViews)
        ltv->searchFieldThread();
}

void LogView::searchFieldCategory()
{
    for(auto ltv : m_logTableViews)
        ltv->searchFieldCategory();
}

void LogView::searchFieldSourceFile()
{
    for(auto ltv : m_logTableViews)
        ltv->searchFieldSourceFile();
}

void LogView::searchFieldMethod()
{
    for(auto ltv : m_logTableViews)
        ltv->searchFieldMethod();
}

void LogView::searchFieldLogFile()
{
    for(auto ltv : m_logTableViews)
        ltv->searchFieldLogFile();
}

void LogView::searchFieldLine()
{
    for(auto ltv : m_logTableViews)
        ltv->searchFieldLine();
}

void LogView::searchFieldLevel()
{
    for(auto ltv : m_logTableViews)
        ltv->searchFieldLevel();
}

void LogView::onRunExtension(ExtensionPtr e)
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->onRunExtension(e);
    }
}

const QString &LogView::getPath() const
{
    return m_path;
}

void LogView::setPath(const QString &path)
{
    m_path = path;
    for( auto ltv : m_logTableViews)
        ltv->setPath(path);
}

void LogView::newLogTableView()
{
    createLogTableView();
}

void LogView::newLogLevelPieChart()
{
    auto w = new QtCharts::QChartView(m_logTableChartTabWidget);

    Q_ASSERT(!m_logTableViews.isEmpty());
    LogModel* logModel = m_logTableViews.at(0)->getModel();
    Q_ASSERT(logModel);

    QList<QSharedPointer<StatisticItem>> sis;
    logModel->getLevelStatistic(sis);
    setChart(w, sis, "Level Count Statistic");

	m_logTableChartTabWidget->addTab(w, "Level");
	m_logTableChartTabWidget->setCurrentWidget(w);
}

void LogView::newLogThreadPieChart()
{
    auto w = new QtCharts::QChartView(m_logTableChartTabWidget);

    Q_ASSERT(!m_logTableViews.isEmpty());
    LogModel* logModel = m_logTableViews.at(0)->getModel();
    Q_ASSERT(logModel);

    QList<QSharedPointer<StatisticItem>> sis;
    logModel->getThreadStatistic(sis);
    setChart(w, sis, "Thread Count Statistic");

	m_logTableChartTabWidget->addTab(w, "Thread");
	m_logTableChartTabWidget->setCurrentWidget(w);
}

void LogView::newLogSourceFilePieChart()
{
    auto w = new QtCharts::QChartView(m_logTableChartTabWidget);

    Q_ASSERT(!m_logTableViews.isEmpty());
    LogModel* logModel = m_logTableViews.at(0)->getModel();
    Q_ASSERT(logModel);

    QList<QSharedPointer<StatisticItem>> sis;
    logModel->getSourceFileStatistic(sis);
    setChart(w, sis, "Source File Count Statistic");

	m_logTableChartTabWidget->addTab(w, "Source File");
	m_logTableChartTabWidget->setCurrentWidget(w);
}

void LogView::newLogSourceLinePieChart()
{
    auto w = new QtCharts::QChartView(m_logTableChartTabWidget);

    Q_ASSERT(!m_logTableViews.isEmpty());
    LogModel* logModel = m_logTableViews.at(0)->getModel();
    Q_ASSERT(logModel);

    QList<QSharedPointer<StatisticItem>> sis;
    logModel->getSourceLineStatistic(sis);
    setChart(w, sis, "Source Line Count Statistic");

	m_logTableChartTabWidget->addTab(w, "Source Line");
	m_logTableChartTabWidget->setCurrentWidget(w);
}

void LogView::newLogCategoryPieChart()
{
    auto w = new QtCharts::QChartView(m_logTableChartTabWidget);

    Q_ASSERT(!m_logTableViews.isEmpty());
    LogModel* logModel = m_logTableViews.at(0)->getModel();
    Q_ASSERT(logModel);

    QList<QSharedPointer<StatisticItem>> sis;
    logModel->getCategoryStatistic(sis);
    setChart(w, sis, "Category Count Statistic");

	m_logTableChartTabWidget->addTab(w, "Category");
	m_logTableChartTabWidget->setCurrentWidget(w);
}

void LogView::newLogMethodPieChart()
{
    auto w = new QtCharts::QChartView(m_logTableChartTabWidget);

    Q_ASSERT(!m_logTableViews.isEmpty());
    LogModel* logModel = m_logTableViews.at(0)->getModel();
    Q_ASSERT(logModel);

    QList<QSharedPointer<StatisticItem>> sis;
    logModel->getMethodStatistic(sis);
    setChart(w, sis, "Method Count Statistic");

	m_logTableChartTabWidget->addTab(w, "Method");
	m_logTableChartTabWidget->setCurrentWidget(w);
}

void LogView::newLogPresenceTableView()
{
    auto w = new PresenceWidget(m_logTableChartTabWidget, m_sqlite3Helper);
	m_logTableChartTabWidget->addTab(w, "Presence");
	m_logTableChartTabWidget->setCurrentWidget(w);
    w->onRefreshBuddyList();
	
	for (auto ltv : m_logTableViews)
		connect(ltv, &LogTableView::databaseCreated, w, &PresenceWidget::databaseCreated);
}

void LogView::enableRegexpMode(bool enabled)
{
    for( auto ltv : m_logTableViews)
        ltv->enableRegexpMode(enabled);
}

void LogView::inputKeyword()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->inputKeyword();
    }
}

void LogView::onClearKeyword()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->onClearKeyword();
    }
}

void LogView::onShowLogItemsBetweenSelectedRows()
{
    QWidget* w = m_logTableChartTabWidget->currentWidget();
    if (w)
    {
        LogTableView* ltv = qobject_cast<LogTableView*>(w);
        if (ltv)
            ltv->onShowLogItemsBetweenSelectedRows();
    }
}

void LogView::onLogTableChartTabWidgetCurrentChanged(int index)
{
    auto w = m_logTableChartTabWidget->widget(index);
    auto ltv = qobject_cast<LogTableView*>(w);
    if (ltv)
    {
        emit rowCountChanged();
    }
}

bool LogView::event(QEvent* e)
{
    switch (int(e->type()))
    {
    case EXTRACTED_EVENT:
	{
        getMainWindow()->closeProgressDialog();
		QDir dir(m_extractDir);
		dir.setFilter(QDir::Files | QDir::NoSymLinks);
		dir.setSorting(QDir::Name);
		QStringList filters;
		filters << "jabber.log" << "jabber.log.1" << "jabber.log.2" << "jabber.log.3" << "jabber.log.4" << "jabber.log.5"
                << "jabber.log.6" << "jabber.log.7" << "jabber.log.8" << "jabber.log.9" << "jabber.log.10";
		dir.setNameFilters(filters);

		QStringList fileNames;
		QFileInfoList list = dir.entryInfoList();

		std::for_each(list.begin(), list.end(),
			[&fileNames](const QFileInfo& fileInfo) {fileNames << fileInfo.filePath(); });

		for (auto ltv : m_logTableViews)
			ltv->loadFromFiles(fileNames);
	}
        return true;
	case EXTRACT_FAILED_EVENT:
	{
		getMainWindow()->closeProgressDialog();
		ExtractFailedEvent* efe = dynamic_cast<ExtractFailedEvent*>(e);
		Q_ASSERT(efe);
		QMessageBox::critical(this, tr("Error"), 
			QString("Extracting %1 to %2 failed.").arg(efe->m_fileName).arg(efe->m_dirName),
			QMessageBox::Ok);
	}
		return true;
    default:
        return QObject::event(e);
    }
}

void LogView::extract(LogView* v, const QString& fileName, const QString& dirName)
{
    QZipReader zr(fileName);
	if (!zr.isReadable())
	{
		ExtractFailedEvent* e = new ExtractFailedEvent;
		e->m_fileName = QDir::toNativeSeparators(fileName);
		e->m_dirName = QDir::toNativeSeparators(dirName);
		QCoreApplication::postEvent(v, e);
		return;
	}
	int count = zr.count();
	for (int i = 0; i < count; i++)
	{
		auto fi = zr.entryInfoAt(i);
		if (!fi.isFile)
			continue;
		auto fileData = zr.fileData(fi.filePath);
		QFile f(QString("%1/%2").arg(dirName).arg(fi.filePath));
		if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
		{
			f.write(fileData);
			f.close();
		}
	}
	zr.close();

    ExtractedEvent* e = new ExtractedEvent;
    QCoreApplication::postEvent(v, e);
}
