#include "stdafx.h"
#include <functional>
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

bool QuickGetFilesByFileName(const QString& fileName, QStringList& results);

LogTableView::LogTableView(QWidget *parent, Sqlite3HelperPtr sqlite3Helper)
	: QWidget(parent)
    , m_api(new QuickWidgetAPI)
	, m_logsTableView(new QTableView(this))
    , m_logModel(new LogModel(m_logsTableView, sqlite3Helper, m_api))
{
    initialize();
    m_logModel->postInitialize();
}

LogModel *LogTableView::getModel()
{
    return m_logModel;
}

void LogTableView::initialize()
{
    int res = g_settings->logTableColumnVisible();
    for(int i = 0; i < 10; i++)
        m_hheaderColumnHidden.append(!(res & (0x01 << i)));

    QWidget* topBar = new QWidget(this);
    QHBoxLayout* topBarLayout = new QHBoxLayout;
    topBarLayout->setMargin(0);
    topBar->setLayout(topBarLayout);
    QLabel* label = new QLabel(topBar);
    label->setText("Search keyword:");
    topBarLayout->addWidget(label);
    m_cbSearchKeyword = new QComboBox(topBar);
    topBarLayout->addWidget(m_cbSearchKeyword);
    m_inputKeywordlLockButton = new QToolButton(topBar);
    m_inputKeywordlLockButton->setIcon(QIcon(":/image/unlock.png"));
    m_inputKeywordlLockButton->setShortcut(QKeySequence("Ctrl+L"));
    topBarLayout->addWidget(m_inputKeywordlLockButton);
    m_extraToolPanelVisibleButton = new QToolButton(topBar);
    m_extraToolPanelVisibleButton->setIcon(QIcon(":/image/openedeye.png"));
    topBarLayout->addWidget(m_extraToolPanelVisibleButton);
    topBarLayout->setStretch(1, 1);

    m_extraToolPanel = new QQuickWidget(this);
    m_extraToolPanel->setAttribute(Qt::WA_TranslucentBackground, true);
    m_extraToolPanel->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    m_extraToolPanel->setClearColor(Qt::transparent);
    m_extraToolPanel->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_extraToolPanel->engine()->rootContext()->setContextProperty("LogViewAPI", m_api.data());
    m_extraToolPanel->setSource(QUrl("qrc:qml/main.qml"));

    QVBoxLayout* thisLayout = new QVBoxLayout;
    thisLayout->setMargin(0);
    thisLayout->addWidget(topBar);
    thisLayout->addWidget(m_extraToolPanel);
    thisLayout->addWidget(m_logsTableView);
    thisLayout->setStretch(2, 1);
    this->setLayout(thisLayout);

    connect(m_inputKeywordlLockButton, &QToolButton::clicked, [&]() {
        m_cbSearchKeyword->setEnabled(!m_cbSearchKeyword->isEnabled());
        m_inputKeywordlLockButton->setIcon(m_cbSearchKeyword->isEnabled() ? QIcon(":/image/unlock.png") : QIcon(":/image/lock.png"));
    });

    m_extraToolPanel->setVisible(false);
    connect(m_extraToolPanelVisibleButton, &QToolButton::clicked, [&]() {
            m_extraToolPanel->setVisible(!m_extraToolPanel->isVisible());
            m_extraToolPanelVisibleButton->setIcon(m_extraToolPanel->isVisible() ? QIcon(":/image/closedeye.png") : QIcon(":/image/openedeye.png"));
    });

    m_logsTableView->setModel(m_logModel);
    m_logsTableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    m_logsTableView->horizontalHeader()->setSectionsMovable(true);
    m_logsTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    m_logsTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_cbSearchKeyword->setEditable(true);
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Content"));
    QAction* actionSearchKeywordOptions = new QAction(QIcon(":/image/keyword.png"), "Reload Search Result", this);
    m_cbSearchKeyword->lineEdit()->addAction(actionSearchKeywordOptions, QLineEdit::ActionPosition::LeadingPosition);
    QAction* actionClearKeyword = new QAction(QIcon(":/image/clear-keyword.png"), "Clear Keyword", this);
    m_cbSearchKeyword->lineEdit()->addAction(actionClearKeyword, QLineEdit::ActionPosition::TrailingPosition);

    m_keywordChangedTimer = new QTimer;
    m_keywordChangedTimer->setSingleShot(true);
    m_keywordChangedTimer->setInterval(500);
    connect(actionSearchKeywordOptions, &QAction::triggered, this, &LogTableView::onSearchKeywordOptions);
    connect(actionClearKeyword, &QAction::triggered, this, &LogTableView::onClearKeyword);
    connect(m_cbSearchKeyword, &QComboBox::editTextChanged, this, &LogTableView::onCbKeywordEditTextChanged);
    connect(m_cbSearchKeyword, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &LogTableView::onCbKeywordCurrentIndexChanged);
    connect(m_logsTableView, &QAbstractItemView::doubleClicked, this, &LogTableView::onDoubleClicked);
    connect(m_logsTableView, &QWidget::customContextMenuRequested, this, &LogTableView::onCustomContextMenuRequested);
    connect(m_logsTableView->horizontalHeader(), &QWidget::customContextMenuRequested, this, &LogTableView::onHHeaderCustomContextMenuRequested);
    connect(m_logModel, &LogModel::dataLoaded, this, &LogTableView::onDataLoaded);
    connect(m_logModel, &LogModel::rowCountChanged, this, &LogTableView::onRowCountChanged);
    connect(m_logModel, &LogModel::databaseCreated, this, &LogTableView::databaseCreated);
    connect(this, &LogTableView::runExtension, m_logModel, &LogModel::runLuaExtension);
}

void LogTableView::onClearKeyword()
{
    if (m_cbSearchKeyword->isEnabled())
    {
        m_cbSearchKeyword->setFocus();
        m_cbSearchKeyword->clearEditText();
    }
}

void LogTableView::onDoubleClicked(const QModelIndex &index)
{
    switch (index.column())// the content field
    {
    case 4:
#if defined(Q_OS_WIN)
        if (qApp->keyboardModifiers() & Qt::ControlModifier)
        {
            openSourceFile(index, [this](const QString& filePath, int line){
                emit openSourceFileInVS(filePath, line);
            });
        }
        else
#endif
        {
            openSourceFile(index, [this](const QString& filePath, int line){
                emit openSourceFileWithBuiltinEditor(filePath, line);
            });
        }
        break;
    case 7:
    {
        QString text = Utils::formatXML( m_logModel->getLogContent(index));
        emit extractContent(text);
    }
        break;
    case 8:
    {
        QString file = m_logModel->getLogFileName(index);
        emit openLog(file);
    }
        break;
    case 9:
    {
        QString logFile;
        int line = m_logModel->getLogFileLine(index, logFile);
        emit gotoLogLine(line, logFile);
    }
        break;
    default:
        break;
    }
}

void LogTableView::onDataLoaded()
{
    if (m_api->getTo() < m_logModel->getMaxTotalRowCount())
    {
        m_api->setTo( m_logModel->getMaxTotalRowCount());
        emit m_api->toChanged();
        m_api->setSecondValue( m_logModel->getMaxTotalRowCount());
        emit m_api->secondValueChanged();
    }

    // let it run in main thread, so QMessageBox could work as expected
    QTimer::singleShot(100, [&](){
        for (int idx = 0; idx < m_hheaderColumnHidden.length(); idx ++)
            m_logsTableView->setColumnHidden(idx, m_hheaderColumnHidden[idx] );
    });

    emit dataLoaded();
}

void LogTableView::onRowCountChanged()
{
    if (m_lastId >= 0 && m_lastColumn >= 0)
    {
        m_logsTableView->scrollTo(m_logModel->index(m_lastId -1, m_lastColumn));
        m_logsTableView->selectRow(m_lastId -1);
        m_lastId = m_lastColumn = -1;
    }
    emit rowCountChanged();
}

void LogTableView::onCustomContextMenuRequested(const QPoint &pos)
{
    QItemSelectionModel* model = m_logsTableView->selectionModel();
    if (model && model->hasSelection())
    {
        QMenu menu(this);

        QAction* pOpengrokAction = new QAction("Browse Source File with OpenGrok", &menu);
        connect(pOpengrokAction, &QAction::triggered, this, &LogTableView::onBrowseSourceFileWithOpenGrok);
        menu.addAction(pOpengrokAction);

#if defined(Q_OS_WIN)
        QAction* pOpenFileInVSAction = new QAction("Open Source File In Visual Studio", &menu);
        connect(pOpenFileInVSAction, &QAction::triggered, this, &LogTableView::onOpenSourceFileInVS);
        menu.addAction(pOpenFileInVSAction);
#endif

        QAction* pSourceFilePreviewAction = new QAction("Source File Preview", &menu);
        connect(pSourceFilePreviewAction, &QAction::triggered, this, &LogTableView::onSourceFilePreview);
        menu.addAction(pSourceFilePreviewAction);

        QAction* pContentPreviewAction = new QAction("Content Preview", &menu);
        connect(pContentPreviewAction, &QAction::triggered, this, &LogTableView::onContentPreview);
        menu.addAction(pContentPreviewAction);

        QAction* pLogFilePreviewAction = new QAction("Log File Preview", &menu);
        connect(pLogFilePreviewAction, &QAction::triggered, this, &LogTableView::onLogFilePreview);
        menu.addAction(pLogFilePreviewAction);

        menu.addSeparator();
        QItemSelectionModel* selected = m_logsTableView->selectionModel();
        if (selected && selected->hasSelection())
        {
            QAction* pShowLogItemsBetweenSelectedRows = new QAction("Show Log Items Between Selected Rows", this);
            connect(pShowLogItemsBetweenSelectedRows, &QAction::triggered, this, &LogTableView::onShowLogItemsBetweenSelectedRows);
            menu.addAction(pShowLogItemsBetweenSelectedRows);
        }
#if defined(Q_OS_WIN)
        CShellContextMenu scm;
        QPoint p = m_logsTableView->viewport()->mapToGlobal(pos);
        scm.ShowContextMenu(&menu, this, p, QDir::toNativeSeparators(m_path));
#else
        menu.exec(m_logsTableView->viewport()->mapToGlobal(pos));
#endif
    }
}

void LogTableView::onHHeaderCustomContextMenuRequested(const QPoint &pos)
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
        connect(pAction, &QAction::triggered, this, &LogTableView::onHHeaderContextMenuActionTriggered);
        menu.addAction(pAction);
    }

    menu.exec(m_logsTableView->horizontalHeader()->viewport()->mapToGlobal(pos));
}

void LogTableView::onBrowseSourceFileWithOpenGrok()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        openSourceFile(selected->currentIndex(), [this](const QString& filePath, int line){
            emit openSourceFileWithOpenGrok(filePath, line);
        });
    }
}

void LogTableView::onSourceFilePreview()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        openSourceFile(selected->currentIndex(), [this](const QString& filePath, int line){
            emit openSourceFileWithBuiltinEditor(filePath, line);
        });
    }
}

void LogTableView::onOpenSourceFileInVS()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        openSourceFile(selected->currentIndex(), [this](const QString& filePath, int line){
            emit openSourceFileInVS(filePath, line);
        });
    }
}

void LogTableView::onContentPreview()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        QModelIndex index = selected->currentIndex();
        QString text = Utils::formatXML( m_logModel->getLogContent(index));
        emit extractContent(text);
    }
}

void LogTableView::onLogFilePreview()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        QModelIndex index = selected->currentIndex();
        QString logFile;
        int line = m_logModel->getLogFileLine(index, logFile);
        emit gotoLogLine(line, logFile);
    }
}

void LogTableView::onCbKeywordEditTextChanged(const QString &)
{
    m_keywordChangedTimer->disconnect();
    connect(m_keywordChangedTimer, &QTimer::timeout,
            [this](){
        filter(m_cbSearchKeyword->lineEdit()->text().trimmed());
    });
    m_keywordChangedTimer->start();
}

void LogTableView::onCbKeywordCurrentIndexChanged(const QString &)
{
    filter(m_cbSearchKeyword->lineEdit()->text().trimmed());
}

void LogTableView::onShowLogItemsBetweenSelectedRows()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();

    QString sqlWhereClause = m_logModel->getSqlWhereClause(l.at(0), l.last());

    Q_ASSERT(m_cbSearchKeyword);
    m_cbSearchKeyword->lineEdit()->setText(QString("sql>%1").arg(sqlWhereClause));
}

void LogTableView::onSearchKeywordOptions()
{
    QMenu menu(this);

    QAction* pRegexpModeAction = new QAction("Regexp Mode", &menu);
    pRegexpModeAction->setCheckable(true);
    pRegexpModeAction->setChecked(false);
    menu.addAction(pRegexpModeAction);

    QPoint pt = pos();
    pt.setY(pt.y() + m_cbSearchKeyword->height());
    menu.exec(m_cbSearchKeyword->mapToGlobal(pt));
}

void LogTableView::onHHeaderContextMenuActionTriggered()
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

void LogTableView::openSourceFile(const QModelIndex &index, std::function<void (const QString &, int)> callback)
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

void LogTableView::filter(const QString &keyword)
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

const QString &LogTableView::path() const
{
    return m_path;
}

void LogTableView::setPath(const QString &path)
{
    m_path = path;
}

void LogTableView::loadFromFiles(const QStringList &fileNames)
{
    Q_ASSERT(m_logModel);
    m_logModel->loadFromFiles(fileNames);
}

void LogTableView::copyCurrentCell()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndex i = selected->currentIndex();
    m_logModel->copyCell(i);
}

void LogTableView::copyCurrentRow()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    int row = selected->currentIndex().row();
    m_logModel->copyRow(row);
}

void LogTableView::copySelectedCells()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();
    m_logModel->copyCells(l);
}

void LogTableView::copySelectedRows()
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

void LogTableView::copyCurrentCellWithXMLFormatted()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndex i = selected->currentIndex();
    m_logModel->copyCellWithXMLFormatted(i);
}

void LogTableView::copyCurrentRowWithXMLFormatted()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    int row = selected->currentIndex().row();
    m_logModel->copyRowWithXMLFormatted(row);
}

void LogTableView::copySelectedCellsWithXMLFormatted()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();
    m_logModel->copyCellsWithXMLFormatted(l);
}

void LogTableView::copySelectedRowsWithXMLFormatted()
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
    m_logModel->copyRowsWithXMLFormatted(rows);
}

void LogTableView::addCurrentRowToBookmark()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    int row = selected->currentIndex().row();
    m_logModel->addBookmark(row);
}

void LogTableView::removeCurrentRowFromBookmark()
{
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (!selected->hasSelection())
        return;
    int row = selected->currentIndex().row();
    m_logModel->removeBookmark(row);
}

void LogTableView::addSelectedRowsToBookmark()
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
    m_logModel->addBookmarks(rows);
}

void LogTableView::removeSelectedRowsFromBookmark()
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
    m_logModel->removeBookmarks(rows);
}

void LogTableView::removeAllBookmarks()
{
    m_logModel->clearBookmarks();
}

void LogTableView::gotoFirstBookmark()
{
    int bookmark = m_logModel->getFirstBookmark();
    if (bookmark >= 0)
    {
        m_logsTableView->scrollTo(m_logModel->index(bookmark-1, 0));
        m_logsTableView->selectRow(bookmark-1);
        m_logsTableView->setFocus();
    }
}

void LogTableView::gotoPreviousBookmark()
{
    int id = -1;
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        // record the selected cell
        QModelIndex i = selected->currentIndex();
        id = m_logModel->getId(i);
    }
    if (id == -1)
    {
        gotoFirstBookmark();
        return;
    }
    int prevousBookmark = m_logModel->getPreviousBookmark(id);
    m_logsTableView->scrollTo(m_logModel->index(prevousBookmark-1, 0));
    m_logsTableView->selectRow(prevousBookmark-1);
    m_logsTableView->setFocus();
}

void LogTableView::gotoNextBookmark()
{
    int id = -1;
    QItemSelectionModel* selected =  m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        // record the selected cell
        QModelIndex i = selected->currentIndex();
        id = m_logModel->getId(i);
    }
    if (id == -1)
    {
        gotoLastBookmark();
        return;
    }
    int nextBookmark = m_logModel->getNextBookmark(id);
    m_logsTableView->scrollTo(m_logModel->index(nextBookmark-1, 0));
    m_logsTableView->selectRow(nextBookmark-1);
    m_logsTableView->setFocus();
}

void LogTableView::gotoLastBookmark()
{
    int bookmark = m_logModel->getLastBookmark();
    if (bookmark >= 0)
    {
        m_logsTableView->scrollTo(m_logModel->index(bookmark-1, 0));
        m_logsTableView->selectRow(bookmark-1);
        m_logsTableView->setFocus();
    }
}

void LogTableView::scrollToTop()
{
    m_logsTableView->scrollToTop();
    m_logsTableView->selectRow(0);
    m_logsTableView->setFocus();
}

void LogTableView::scrollToBottom()
{
    m_logsTableView->scrollToBottom();
    m_logsTableView->selectRow(rowCount() -1);
    m_logsTableView->setFocus();
}

void LogTableView::gotoById(int i)
{
    m_logsTableView->scrollTo(m_logModel->index(i-1, 0));
    m_logsTableView->selectRow(i-1);
    m_logsTableView->setFocus();
}

int LogTableView::rowCount()
{
    return m_logModel->rowCount();
}

void LogTableView::enableRegexpMode(bool enabled)
{
    m_logModel->setRegexpMode(enabled);
}

void LogTableView::inputKeyword()
{
    m_cbSearchKeyword->setFocus();
    m_cbSearchKeyword->lineEdit()->selectAll();
}

void LogTableView::searchFieldContent()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Content"));
    m_logModel->setSearchField("content");
}

void LogTableView::searchFieldID()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field ID"));
    m_logModel->setSearchField("id");
}

void LogTableView::searchFieldDateTime()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Date Time"));
    m_logModel->setSearchField("time");
}

void LogTableView::searchFieldThread()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Thread"));
    m_logModel->setSearchField("thread");
}

void LogTableView::searchFieldCategory()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Category"));
    m_logModel->setSearchField("category");
}

void LogTableView::searchFieldSourceFile()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Source File"));
    m_logModel->setSearchField("source");
}

void LogTableView::searchFieldMethod()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Method"));
    m_logModel->setSearchField("method");
}

void LogTableView::searchFieldLogFile()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Log File"));
    m_logModel->setSearchField("log");
}

void LogTableView::searchFieldLine()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Line"));
    m_logModel->setSearchField("line");
}

void LogTableView::searchFieldLevel()
{
    m_cbSearchKeyword->lineEdit()->setPlaceholderText(tr("Search Field Level"));
    m_logModel->setSearchField("level");
}

void LogTableView::onRunExtension(ExtensionPtr e)
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
