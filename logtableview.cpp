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

LogTableView::LogTableView(QWidget *parent, Sqlite3HelperPtr sqlite3Helper, QuickWidgetAPIPtr api)
    : QWidget(parent)
    , m_logModel(new LogModel(m_logsTableView, sqlite3Helper, api))
{
    QWidget* topBar = new QWidget(this);
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

    m_extraToolPanel = new QQuickWidget(this);
    m_extraToolPanel->setAttribute(Qt::WA_TranslucentBackground, true);
    m_extraToolPanel->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    m_extraToolPanel->setClearColor(Qt::transparent);
    m_extraToolPanel->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_extraToolPanel->engine()->rootContext()->setContextProperty("LogTableViewAPI", m_api.data());
    m_extraToolPanel->setSource(QUrl("qrc:qml/main.qml"));

    QVBoxLayout* thisLayout = new QVBoxLayout;
    thisLayout->setMargin(0);
    thisLayout->addWidget(topBar);
    thisLayout->addWidget(m_extraToolPanel);
    thisLayout->addWidget(m_logsTableView);
    thisLayout->setStretch(2, 1);
    this->setLayout(thisLayout);

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
    QAction* actionReloadSearchResult = new QAction(QIcon(":/image/keyword.png"), "Reload Search Result", this);
    m_cbSearchKeyword->lineEdit()->addAction(actionReloadSearchResult, QLineEdit::ActionPosition::LeadingPosition);
    QAction* actionClearKeyword = new QAction(QIcon(":/image/clear-keyword.png"), "Clear Keyword", this);
    m_cbSearchKeyword->lineEdit()->addAction(actionClearKeyword, QLineEdit::ActionPosition::TrailingPosition);

    m_keywordChangedTimer = new QTimer;
    m_keywordChangedTimer->setSingleShot(true);
    m_keywordChangedTimer->setInterval(500);
    connect(actionClearKeyword, &QAction::triggered, this, &LogTableView::onClearKeyword);
    connect(m_cbSearchKeyword, &QComboBox::editTextChanged, this, &LogTableView::onCbKeywordEditTextChanged);
    connect(m_cbSearchKeyword, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &LogTableView::onCbKeywordCurrentIndexChanged);
    connect(m_logsTableView, &QAbstractItemView::doubleClicked, this, &LogTableView::onDoubleClicked);
    connect(m_logsTableView, &QWidget::customContextMenuRequested, this, &LogTableView::onCustomContextMenuRequested);
    connect(m_logsTableView->horizontalHeader(), &QWidget::customContextMenuRequested, this, &LogTableView::onHHeaderCustomContextMenuRequested);
}

void LogTableView::onClearKeyword()
{
    m_cbSearchKeyword->setFocus();
    m_cbSearchKeyword->clearEditText();
}

void LogTableView::onDoubleClicked(const QModelIndex &index)
{
    emit itemDoubleClicked(index);
}

void LogTableView::onDataLoaded()
{

}

void LogTableView::onRowCountChanged()
{

}

void LogTableView::onCustomContextMenuRequested(const QPoint &pos)
{
    QItemSelectionModel* model = m_logsTableView->selectionModel();
    if (model && model->hasSelection())
    {
        QMenu menu(this);

        QAction* pOpengrokAction = new QAction("Browse Source File with OpenGrok", this);
        connect(pOpengrokAction, &QAction::triggered, this, &LogTableView::onBrowseSourceFileWithOpenGrok);
        menu.addAction(pOpengrokAction);

#if defined(Q_OS_WIN)
        QAction* pOpenFileInVSAction = new QAction("Open Source File In Visual Studio", this);
        connect(pOpenFileInVSAction, &QAction::triggered, this, &LogTableView::onOpenSourceFileInVS);
        menu.addAction(pOpenFileInVSAction);
#endif

        QAction* pSourceFilePreviewAction = new QAction("Source File Preview", this);
        connect(pSourceFilePreviewAction, &QAction::triggered, this, &LogTableView::onSourceFilePreview);
        menu.addAction(pSourceFilePreviewAction);

        QAction* pContentPreviewAction = new QAction("Content Preview", this);
        connect(pContentPreviewAction, &QAction::triggered, this, &LogTableView::onContentPreview);
        menu.addAction(pContentPreviewAction);

        QAction* pLogFilePreviewAction = new QAction("Log File Preview", this);
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
        emit extractContent(selected->currentIndex());
    }
}

void LogTableView::onLogFilePreview()
{
    QItemSelectionModel* selected = m_logsTableView->selectionModel();
    if (selected && selected->hasSelection())
    {
        emit gotoLogLine(selected->currentIndex());
    }
}

void LogTableView::onCbKeywordEditTextChanged(const QString &text)
{
    m_keywordChangedTimer->disconnect();
    connect(m_keywordChangedTimer, &QTimer::timeout,
            [this](){
        filter(m_cbSearchKeyword->lineEdit()->text().trimmed());
    });
    m_keywordChangedTimer->start();
}

void LogTableView::onCbKeywordCurrentIndexChanged(const QString &text)
{
    filter(text.trimmed());
}

void LogTableView::onReloadSearchResult()
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
