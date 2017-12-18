#include "stdafx.h"
#include <functional>
#include <JlCompress.h>
#include <QPieSeries>
#include <QPieSlice>
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
#include "logmodel.h"
#include "logview.h"

QT_CHARTS_USE_NAMESPACE

bool QuickGetFilesByFileName(const QString& fileName, QStringList& results);

#if defined(Q_OS_WIN)
extern QWinTaskbarButton *g_winTaskbarButton;
extern QWinTaskbarProgress *g_winTaskbarProgress;
#endif
SourceWindow* g_sourceWindow = nullptr;

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
    int res = g_settings->logTableColumnVisible();
    for(int i = 0; i < 10; i++)
        m_hheaderColumnHidden.append(!(res & (0x01 << i)));

    m_verticalSplitter->addWidget(m_logTableChartTabWidget);
    m_verticalSplitter->addWidget(m_codeEditorTabWidget);

    QList<int> sizes;
    sizes << 0x7FFFF << 0;
    m_verticalSplitter->setSizes(sizes);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    Q_ASSERT(mainLayout);
    mainLayout->setMargin(0);

    QWidget* logsTab = new QWidget(this);
    QWidget* topBar = new QWidget(logsTab);
    QHBoxLayout* topBarLayout = new QHBoxLayout;
    topBarLayout->setMargin(0);
    topBar->setLayout(topBarLayout);
    QLabel* label = new QLabel(topBar);
    label->setText("Search keyword:");
    topBarLayout->addWidget(label);
    m_cbSearchKeyword = new QComboBox(topBar);
    topBarLayout->addWidget(m_cbSearchKeyword);
    m_extraToolPanelVisibleButton = new QToolButton(topBar);
    m_extraToolPanelVisibleButton->setIcon(QIcon(":/image/openedeye.png"));
    topBarLayout->addWidget(m_extraToolPanelVisibleButton);
    topBarLayout->setStretch(1, 1);

    m_api = m_logModel->getQuickWidgetAPI();

    m_extraToolPanel = new QQuickWidget(logsTab);
    m_extraToolPanel->setAttribute(Qt::WA_TranslucentBackground, true);
    m_extraToolPanel->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    m_extraToolPanel->setClearColor(Qt::transparent);
    m_extraToolPanel->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_extraToolPanel->engine()->rootContext()->setContextProperty("LogViewAPI", m_api);
    m_extraToolPanel->setSource(QUrl("qrc:qml/main.qml"));

    QVBoxLayout* logsTabLayout = new QVBoxLayout;
    logsTabLayout->setMargin(0);
    logsTabLayout->addWidget(topBar);
    logsTabLayout->addWidget(m_extraToolPanel);
    logsTabLayout->addWidget(m_logsTableView);
    logsTabLayout->setStretch(2, 1);
    logsTab->setLayout(logsTabLayout);

    mainLayout->addWidget(m_verticalSplitter);
    setLayout(mainLayout);

    m_extraToolPanel->setVisible(false);
    connect(m_extraToolPanelVisibleButton, &QToolButton::clicked, [&]() {
            m_extraToolPanel->setVisible(!m_extraToolPanel->isVisible());
            m_extraToolPanelVisibleButton->setIcon(m_extraToolPanel->isVisible() ? QIcon(":/image/closedeye.png") : QIcon(":/image/openedeye.png"));
    });

    m_logTableChartTabWidget->setTabPosition(QTabWidget::South);
    m_logTableChartTabWidget->setTabsClosable(false);
    m_logTableChartTabWidget->setDocumentMode(true);
    m_logTableChartTabWidget->addTab(logsTab, "Logs");
    m_logTableChartTabWidget->addTab(m_levelStatisticChart, "Level");
    m_logTableChartTabWidget->addTab(m_threadStatisticChart, "Thread");
    m_logTableChartTabWidget->addTab(m_sourceFileStatisticChart, "Source File");
    m_logTableChartTabWidget->addTab(m_sourceLineStatisticChart, "Source Line");
    m_logTableChartTabWidget->addTab(m_categoryStatisticChart, "Category");
    m_logTableChartTabWidget->addTab(m_methodStatisticChart, "Method");
    m_logTableChartTabWidget->addTab(m_presenceWidget, "Presence");

    connect(m_logTableChartTabWidget, &QTabWidget::currentChanged, this, &LogView::onLogTableChartTabWidgetCurrentChanged);

    m_logsTableView->setModel(m_logModel);
    m_logsTableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    m_logsTableView->horizontalHeader()->setSectionsMovable(true);
    m_logsTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    m_logsTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_cbSearchKeyword->setEditable(true);
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Content"));
    QAction* actionReloadSearchResult = new QAction(QIcon(":/image/keyword.png"), "Reload Search Result", this);
    m_cbSearchKeyword->lineEdit()->addAction(actionReloadSearchResult, QLineEdit::ActionPosition::LeadingPosition);
    QAction* actionClearKeyword = new QAction(QIcon(":/image/clear-keyword.png"), "Clear Keyword", this);
    m_cbSearchKeyword->lineEdit()->addAction(actionClearKeyword, QLineEdit::ActionPosition::TrailingPosition);

    m_keywordChangedTimer = new QTimer;
    m_keywordChangedTimer->setSingleShot(true);
    m_keywordChangedTimer->setInterval(500);

    connect(actionReloadSearchResult, &QAction::triggered, this, &LogView::onReloadSearchResult);
    connect(actionClearKeyword, &QAction::triggered, this, &LogView::onClearKeyword);
    connect(m_cbSearchKeyword, &QComboBox::editTextChanged, this, &LogView::onCbKeywordEditTextChanged);
    connect(m_cbSearchKeyword, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &LogView::onCbKeywordCurrentIndexChanged);
    connect(m_logsTableView, &QAbstractItemView::doubleClicked, this, &LogView::onDoubleClicked);
    connect(m_logsTableView, &QWidget::customContextMenuRequested, this, &LogView::onCustomContextMenuRequested);
    connect(m_logsTableView->horizontalHeader(), &QWidget::customContextMenuRequested, this, &LogView::onHHeaderCustomContextMenuRequested);
    connect(m_logModel, &LogModel::dataLoaded, this, &LogView::onDataLoaded);
    connect(m_logModel, &LogModel::rowCountChanged, this, &LogView::onRowCountChanged);
    connect(m_logModel, &LogModel::databaseCreated, m_presenceWidget, &PresenceWidget::databaseCreated);
    connect(this, &LogView::runExtension, m_logModel, &LogModel::runLuaExtension);
}

LogView::~LogView()
{
    if (!m_extractDir.isEmpty())
    {
        QDir dir(m_extractDir);
        dir.removeRecursively();
    }
}

void LogView::openZipBundle(const QString &zipBundle, const QString &crashInfo)
{
    m_crashInfo = crashInfo;
    openZipBundle(zipBundle);
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

    getMainWindow()->showProgressDialog(QString(tr("Loading logs from ZIP bundle %1...")).arg(path));

    QtConcurrent::run(this, &LogView::extract, this, path, m_extractDir);
}

void LogView::openRawLogFile(const QStringList &paths)
{
    m_path = paths.join(":");
    QString path = paths.at(0);
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());

    getMainWindow()->showProgressDialog(QString(tr("Loading logs from raw log files: %1...")).arg(paths.join(",")));

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

    getMainWindow()->showProgressDialog(QString(tr("Loading logs from folder %1...")).arg(path));

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
    for(const QModelIndex& i : l)
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

    QString text = m_logModel->getLogContent(index);
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
        if (header.length() > text.length())
            text = header;
    }

    if (g_settings->multiMonitorEnabled())
    {
        g_sourceWindow->getSourceViewTabWidget()->setContent(m_path, text);
    }
    else
    {
        m_codeEditorTabWidget->setContent(text);
    }
}

void LogView::openSourceFile(const QModelIndex &index, std::function<void(const QString&, int)> callback)
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
        for(const QString& filePath : results)
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
                callback(filePath, m.captured(2).toInt());
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
    if (g_settings->multiMonitorEnabled())
    {
        g_sourceWindow->getSourceViewTabWidget()->gotoLine(m_path, logFile);
    }
    else
    {
        m_codeEditorTabWidget->gotoLine(logFile);
    }
}

void LogView::gotoLogLine(const QModelIndex &index)
{
    showCodeEditorPane();
    QString logFile;
    int line = m_logModel->getLogFileLine(index, logFile);
    if (g_settings->multiMonitorEnabled())
    {
        g_sourceWindow->getSourceViewTabWidget()->gotoLine(m_path, logFile, line);
    }
    else
    {
        m_codeEditorTabWidget->gotoLine(logFile, line);
    }
}

void LogView::onDoubleClicked(const QModelIndex& index)
{
    switch (index.column())// the content field
    {
    case 4:
#if defined(Q_OS_WIN)
        if (qApp->keyboardModifiers() & Qt::ControlModifier)
        {
            openSourceFile(index, std::bind(&LogView::openSourceFileInVS,
                                            this,
                                            std::placeholders::_1,
                                            std::placeholders::_2));
        }
        else
#endif
        {
            openSourceFile(index, std::bind(&LogView::openSourceFileWithBuiltinEditor,
                                            this,
                                            std::placeholders::_1,
                                            std::placeholders::_2));
        }
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
    if (QFileInfo(m_path).suffix() == "zip")
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

void LogView::openSourceFileWithBuiltinEditor(const QString &filePath, int line)
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

void LogView::openSourceFileInVS(const QString &filePath, int line)
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

void LogView::openSourceFileWithOpenGrok(const QString &filePath, int line)
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

void LogView::onDataLoaded()
{
    getMainWindow()->closeProgressDialog();

    if (m_api->getTo() < m_logModel->getMaxTotalRowCount())
    {
        m_api->setTo( m_logModel->getMaxTotalRowCount());
        emit m_api->toChanged();
        m_api->setSecondValue( m_logModel->getMaxTotalRowCount());
        emit m_api->secondValueChanged();
    }

    // let it run in main thread, so QMessageBox could work as expected
    QTimer::singleShot(100, [&](){
        openCrashReport();

        for (int idx = 0; idx < m_hheaderColumnHidden.length(); idx ++)
            m_logsTableView->setColumnHidden(idx, m_hheaderColumnHidden[idx] );
    });
}

void LogView::onRowCountChanged()
{
    if (m_lastId >= 0 && m_lastColumn >= 0)
    {
        m_logsTableView->scrollTo(m_logModel->index(m_lastId -1, m_lastColumn));
        m_logsTableView->selectRow(m_lastId -1);
        m_lastId = m_lastColumn = -1;
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

#if defined(Q_OS_WIN)
        QAction* pOpenFileInVSAction = new QAction("Open Source File In Visual Studio", this);
        connect(pOpenFileInVSAction, &QAction::triggered, this, &LogView::onOpenSourceFileInVS);
        menu.addAction(pOpenFileInVSAction);
#endif

        QAction* pSourceFilePreviewAction = new QAction("Source File Preview", this);
        connect(pSourceFilePreviewAction, &QAction::triggered, this, &LogView::onSourceFilePreview);
        menu.addAction(pSourceFilePreviewAction);

        QAction* pContentPreviewAction = new QAction("Content Preview", this);
        connect(pContentPreviewAction, &QAction::triggered, this, &LogView::onContentPreview);
        menu.addAction(pContentPreviewAction);

        QAction* pLogFilePreviewAction = new QAction("Log File Preview", this);
        connect(pLogFilePreviewAction, &QAction::triggered, this, &LogView::onLogFilePreview);
        menu.addAction(pLogFilePreviewAction);

        menu.addSeparator();
        QItemSelectionModel* selected = m_logsTableView->selectionModel();
        if (selected && selected->hasSelection())
        {
            QAction* pOpenSelectedRowsInNewTabAction = new QAction("Open Selected Rows In New Tab", this);
            connect(pOpenSelectedRowsInNewTabAction, &QAction::triggered, this, &LogView::onOpenSelectedRowsInNewTab);
            menu.addAction(pOpenSelectedRowsInNewTabAction);

            QAction* pShowLogItemsBetweenSelectedRows = new QAction("Show Log Items Between Selected Rows", this);
            connect(pShowLogItemsBetweenSelectedRows, &QAction::triggered, this, &LogView::onShowLogItemsBetweenSelectedRows);
            menu.addAction(pShowLogItemsBetweenSelectedRows);

            QAction* pSetBeginAnchorAction = new QAction("Set Begin Anchor At The First Selected Row", this);
            connect(pSetBeginAnchorAction, &QAction::triggered, this, &LogView::onSetBeginAnchor);
            menu.addAction(pSetBeginAnchorAction);

            QAction* pSetEndAnchorAction = new QAction("Set End Anchor At The Last Selected Row", this);
            connect(pSetEndAnchorAction, &QAction::triggered, this, &LogView::onSetEndAnchor);
            menu.addAction(pSetEndAnchorAction);
        }
        QAction* pOpenRowsBetweenAnchorsInNewTabAction = new QAction("Open Rows Between Anchors In New Tab", this);
        connect(pOpenRowsBetweenAnchorsInNewTabAction, &QAction::triggered, this, &LogView::onOpenRowsBetweenAnchorsInNewTab);
        menu.addAction(pOpenRowsBetweenAnchorsInNewTabAction);

        QAction* pShowLogItemsBetweenAnchorsAction = new QAction("Show Log Items Between Anchors", this);
        connect(pShowLogItemsBetweenAnchorsAction, &QAction::triggered, this, &LogView::onShowLogItemsBetweenAnchors);
        menu.addAction(pShowLogItemsBetweenAnchorsAction);
#if defined(Q_OS_WIN)
        CShellContextMenu scm;
        QPoint p = m_logsTableView->viewport()->mapToGlobal(pos);
        scm.ShowContextMenu(&menu, this, p, QDir::toNativeSeparators(m_path));
#else
        menu.exec(m_logsTableView->viewport()->mapToGlobal(pos));
#endif
    }
}

void LogView::onHHeaderContextMenuActionTriggered()
{
    QAction* p = qobject_cast<QAction*>(sender());
    QMap<QString, int> labels = {
        {"Id", 0},
        {"Time",1},
        {"Level",2},
        {"Thread",3},
        {"Source File",4},
        {"Category",5},
        {"Method",6},
        {"Content",7},
        {"Log File",8},
        {"Line",9},
    };
    int idx = labels[p->text()];
    m_hheaderColumnHidden[idx] = !m_hheaderColumnHidden[idx] ;
    m_logsTableView->setColumnHidden(idx, m_hheaderColumnHidden[idx] );
}

void LogView::onHHeaderCustomContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    QStringList labels = {
        "Id",
        "Time",
        "Level",
        "Thread",
        "Source File",
        "Category",
        "Method",
        "Content",
        "Log File",
        "Line",
    };
    for (int i = 0; i < m_hheaderColumnHidden.length(); i++)
    {
        QAction* pAction = new QAction(labels[i], &menu);
        pAction->setCheckable(true);
        pAction->setChecked(!m_hheaderColumnHidden[i]);
        connect(pAction, &QAction::triggered, this, &LogView::onHHeaderContextMenuActionTriggered);
        menu.addAction(pAction);
    }

    menu.exec(m_logsTableView->horizontalHeader()->viewport()->mapToGlobal(pos));
}

void LogView::onBrowseSourceFileWithOpenGrok()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        openSourceFile(selected->currentIndex(), std::bind(&LogView::openSourceFileWithOpenGrok,
                                                           this,
                                                           std::placeholders::_1,
                                                           std::placeholders::_2));
    }
}

void LogView::onSourceFilePreview()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        openSourceFile(selected->currentIndex(), std::bind(&LogView::openSourceFileWithBuiltinEditor,
                                                           this,
                                                           std::placeholders::_1,
                                                           std::placeholders::_2));
    }
}

void LogView::onOpenSourceFileInVS()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        openSourceFile(selected->currentIndex(), std::bind(&LogView::openSourceFileInVS,
                                                           this,
                                                           std::placeholders::_1,
                                                           std::placeholders::_2));
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

void LogView::onCbKeywordEditTextChanged(const QString & /*text*/)
{
    m_keywordChangedTimer->disconnect();
    connect(m_keywordChangedTimer, &QTimer::timeout,
            [this](){
        filter(m_cbSearchKeyword->lineEdit()->text().trimmed());
    });
    m_keywordChangedTimer->start();
}

void LogView::onCbKeywordCurrentIndexChanged(const QString &text)
{
    filter(text.trimmed());
}

void LogView::inputKeyword()
{
    m_cbSearchKeyword->setFocus();
    m_cbSearchKeyword->lineEdit()->selectAll();
}

void LogView::searchFieldContent()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Content"));
    m_logModel->setSearchField("content");
}

void LogView::searchFieldID()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field ID"));
    m_logModel->setSearchField("id");
}

void LogView::searchFieldDateTime()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Date Time"));
    m_logModel->setSearchField("time");
}

void LogView::searchFieldThread()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Thread"));
    m_logModel->setSearchField("thread");
}

void LogView::searchFieldCategory()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Category"));
    m_logModel->setSearchField("category");
}

void LogView::searchFieldSourceFile()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Source File"));
    m_logModel->setSearchField("source");
}

void LogView::searchFieldMethod()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Method"));
    m_logModel->setSearchField("method");
}

void LogView::searchFieldLogFile()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Log File"));
    m_logModel->setSearchField("log");
}

void LogView::searchFieldLine()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Line"));
    m_logModel->setSearchField("line");
}

void LogView::searchFieldLevel()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Level"));
    m_logModel->setSearchField("level");
}

void LogView::onRunExtension(ExtensionPtr e)
{
    if (e->method() == "Regexp")
    {
        m_cbSearchKeyword->lineEdit()->setText("r>" + e->content());
    }
    else if (e->method() == "Keyword")
    {
        QMap<QString, QString> m = {
            { "id"      , "i"},
            { "datetime", "d"},
            { "level"   , "v"},
            { "thread"  , "t"},
            { "source"  , "s"},
            { "category", "a"},
            { "method"  , "m"},
            { "content" , "c"},
            { "logfile" , "f"},
            { "line"    , "l"},
        };

        auto it = m.find(e->field());

        if (m.end() != it)
        {
            m_cbSearchKeyword->lineEdit()->setText(*it % ">" % e->content());
        }
    }
    else if (e->method() == "SQL WHERE clause")
    {
        m_cbSearchKeyword->lineEdit()->setText("sql>" + e->content());
    }
    else
    {
        m_cbSearchKeyword->lineEdit()->clear();
        emit runExtension(e);
    }
    m_cbSearchKeyword->setFocus();
    m_cbSearchKeyword->lineEdit()->end(false);
}

void LogView::onClearKeyword()
{
    m_cbSearchKeyword->setFocus();
    m_cbSearchKeyword->clearEditText();
}

void LogView::enableRegexpMode(bool enabled)
{
    m_logModel->setRegexpMode(enabled);
}

void LogView::onReloadSearchResult()
{
    filter(m_cbSearchKeyword->lineEdit()->text().trimmed());
}

void LogView::onOpenSelectedRowsInNewTab()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();
    QList<int> rows;
    for(const QModelIndex& i : l)
    {
        rows.append(i.row());
    }
    auto t = rows.toStdList();
    t.unique();
    rows = QList<int>::fromStdList(t);

    QString name = QInputDialog::getText(this, tr("Input Name"), tr("Please input name for the selected rows, it will be used as tab title."));

    QString tempDir = g_settings->temporaryDirectory();
    if (tempDir.isEmpty())
    {
        tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) % "/CiscoJabberLogs";
    }
    tempDir.append("/" % name);
    QDir dir(tempDir);
    if (!dir.exists())
        dir.mkpath(tempDir);
    else
    {
        if (QMessageBox::question(this,
                              tr("Duplicated Name"),
                              tr("There is a PRT with the same name, overwrite it?"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No) == QMessageBox::No)
        {
            return;
        }
    }

    m_logModel->saveRowsInFolder(rows, tempDir);
    TabWidget* tabWidget = getMainWindow()->getMainTabWidget();
    tabWidget->openFolder(tempDir, false);
}

void LogView::onShowLogItemsBetweenSelectedRows()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();

    QString sqlWhereClause = m_logModel->getSqlWhereClause(l.at(0), l.last());

    Q_ASSERT(m_cbSearchKeyword);
    m_cbSearchKeyword->lineEdit()->setText(QString("sql>%1").arg(sqlWhereClause));
}

void LogView::onSetBeginAnchor()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();
    m_beginAnchor = l.at(0);
}

void LogView::onSetEndAnchor()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();
    m_endAnchor = l.last();
}

void LogView::onOpenRowsBetweenAnchorsInNewTab()
{
    if (!m_beginAnchor.isValid())
    {
        QMessageBox::warning(this, tr("Error"), tr("Begin Anchor hasn't been set."), QMessageBox::Ok);
        return;
    }

    if (!m_endAnchor.isValid())
    {
        QMessageBox::warning(this, tr("Error"), tr("End Anchor hasn't been set."), QMessageBox::Ok);
        return;
    }

    QString name = QInputDialog::getText(this, tr("Input Name"), tr("Please input name for the rows between anchors, it will be used as tab title."));

    QString tempDir = g_settings->temporaryDirectory();
    if (tempDir.isEmpty())
    {
        tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) % "/CiscoJabberLogs";
    }
    tempDir.append("/" % name);
    QDir dir(tempDir);
    if (!dir.exists())
        dir.mkpath(tempDir);
    else
    {
        if (QMessageBox::question(this,
                              tr("Duplicated Name"),
                              tr("There is a PRT with the same name, overwrite it?"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No) == QMessageBox::No)
        {
            return;
        }
    }

    m_logModel->saveRowsBetweenAnchorsInFolder(m_beginAnchor, m_endAnchor, tempDir);
    TabWidget* tabWidget = getMainWindow()->getMainTabWidget();
    tabWidget->openFolder(tempDir, false);
}

void LogView::onShowLogItemsBetweenAnchors()
{
    if (!m_beginAnchor.isValid())
    {
        QMessageBox::warning(this, tr("Error"), tr("Begin Anchor hasn't been set."), QMessageBox::Ok);
        return;
    }

    if (!m_endAnchor.isValid())
    {
        QMessageBox::warning(this, tr("Error"), tr("End Anchor hasn't been set."), QMessageBox::Ok);
        return;
    }

    QString sqlWhereClause = m_logModel->getSqlWhereClause(m_beginAnchor, m_endAnchor);
    Q_ASSERT(m_cbSearchKeyword);
    m_cbSearchKeyword->lineEdit()->setText(QString("sql>%1").arg(sqlWhereClause));
}

void LogView::onLogTableChartTabWidgetCurrentChanged(int index)
{
    if (index == 7)
    {
        // presence widget
        m_presenceWidget->onRefreshBuddyList();
        return;
    }
    // draw charts
    static struct {
        bool created;
        int index;
        std::function<bool(QList<QSharedPointer<StatisticItem>>&)> getter;
        QtCharts::QChartView* chartView;
        QString label;
    }  m[] = {
        {false, 1, std::bind(&LogModel::getLevelStatistic,      m_logModel, std::placeholders::_1), m_levelStatisticChart,      "Level Count Statistic"},
        {false, 2, std::bind(&LogModel::getThreadStatistic,     m_logModel, std::placeholders::_1), m_threadStatisticChart,     "Thread Count Statistic"},
        {false, 3, std::bind(&LogModel::getSourceFileStatistic, m_logModel, std::placeholders::_1), m_sourceFileStatisticChart, "Source File Count Statistic"},
        {false, 4, std::bind(&LogModel::getSourceLineStatistic, m_logModel, std::placeholders::_1), m_sourceLineStatisticChart, "Source Line Count Statistic"},
        {false, 5, std::bind(&LogModel::getCategoryStatistic,   m_logModel, std::placeholders::_1), m_categoryStatisticChart,   "Category Count Statistic"},
        {false, 6, std::bind(&LogModel::getMethodStatistic,     m_logModel, std::placeholders::_1), m_methodStatisticChart,     "Method Count Statistic"},
    };

    QList<QSharedPointer<StatisticItem>> sis;
    auto it = std::find_if(std::begin(m), std::end(m), [&](decltype(m[0])& t){
        return !t.created && t.index == index && t.getter(sis);
    });
    if (it != std::end(m))
    {
        setChart(it->chartView, sis, it->label);
        it->created = true;
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
