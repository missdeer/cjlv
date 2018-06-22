#include "stdafx.h"
#if defined(Q_OS_WIN)
#include "ShellContextMenu.h"
#endif
#include "codeeditor.h"
#include "codeeditortabwidget.h"

CodeEditorTabWidget::CodeEditorTabWidget(QWidget *parent)
    : QTabWidget (parent)
{
    setTabPosition(QTabWidget::South);
    setTabsClosable(true);
    setDocumentMode(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTabWidget::tabCloseRequested, this, &CodeEditorTabWidget::onTabCloseRequested);
    connect(this, &QTabWidget::tabBarDoubleClicked, this, &CodeEditorTabWidget::onTabCloseRequested);
    connect(this, &QTabWidget::currentChanged, this, &CodeEditorTabWidget::onCurrentChanged);
    connect(this, &QWidget::customContextMenuRequested, this, &CodeEditorTabWidget::onCustomContextMenuRequested);
}

void CodeEditorTabWidget::setContent(const QString &content)
{
    for (int index = 0; index < count(); index++)
    {
        QWidget* w = widget(index);
        CodeEditor* v = qobject_cast<CodeEditor*>(w);
        if (v->isContent())
        {
            setCurrentIndex(index);
            v->setContent(content);
            return;
        }
    }

    // create a new tab and set content
    CodeEditor* v = createCodeEditor("Content", "Content Field Value");
    v->setContent(content);
}

void CodeEditorTabWidget::gotoLine(const QString &fileName, int line)
{
    for (int index = 0; index < count(); index++)
    {
        QWidget* w = widget(index);
        CodeEditor* v = qobject_cast<CodeEditor*>(w);
        if (v->matched(fileName))
        {
            setCurrentIndex(index);
            if (line > 0)
                v->gotoLine(line);
            return;
        }
    }

    // create a new tab and goto line
    QFileInfo fi(fileName);
    CodeEditor* v = createCodeEditor(fi.fileName(), fi.absoluteFilePath());
    v->gotoLine(fileName, line);
    m_line = line;
}

bool CodeEditorTabWidget::copyable()
{
    QWidget* w = currentWidget();
    if (w)
    {
        CodeEditor* v = qobject_cast<CodeEditor*>(w);
        return !v->selectionEmpty();
    }
    return false;
}

void CodeEditorTabWidget::copy()
{
    QWidget* w = currentWidget();
    if (w)
    {
        CodeEditor* v = qobject_cast<CodeEditor*>(w);
        v->copy();
    }
}

void CodeEditorTabWidget::onCopyFileName()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(tabText(currentIndex()));
}

void CodeEditorTabWidget::onCopyFileFullPath()
{
    QClipboard *clipboard = QApplication::clipboard();
#if defined(Q_OS_WIN)
    clipboard->setText(QDir::toNativeSeparators(tabToolTip(currentIndex())));
#else
    clipboard->setText(tabToolTip(currentIndex()));
#endif
}

void CodeEditorTabWidget::onOpenContainerFolder()
{
    QString fileFullPath = tabToolTip(currentIndex());
#if defined(Q_OS_WIN)
    QString arg = QString("/select,\"%1\"").arg(QDir::toNativeSeparators(fileFullPath));
    ::ShellExecuteW(NULL, L"open", L"explorer.exe", arg.toStdWString().c_str(), NULL, SW_SHOWNORMAL);
#endif
#if defined(Q_OS_MAC)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(fileFullPath);
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#endif
}

void CodeEditorTabWidget::onOpenFileInVS()
{
#if defined(Q_OS_WIN)
    // extract resource file
    QString localPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/open-in-vs.vbs";
    QFile::copy(":/open-in-vs.vbs" , localPath);

    // run
    QString fileFullPath = tabToolTip(currentIndex());
    QString arg = QString("\"%1\" %2 0").arg(QDir::toNativeSeparators(fileFullPath)).arg(m_line);
    ::ShellExecuteW(NULL, L"open", localPath.toStdWString().c_str(), arg.toStdWString().c_str(), NULL, SW_SHOWNORMAL);
#endif
}

void CodeEditorTabWidget::onCloseAll()
{
    for (int index = count() -1; index >=0; index--)
    {
        onTabCloseRequested(index);
    }
}

void CodeEditorTabWidget::onCloseAllButThis()
{
    for (int index = count() -1; index > currentIndex(); index--)
    {
        onTabCloseRequested(index);
    }
    while (count() > 1)
    {
        onTabCloseRequested(0);
    }
}

void CodeEditorTabWidget::onCloseCurrent()
{
    onTabCloseRequested(currentIndex());
}

void CodeEditorTabWidget::onTabCloseRequested(int index)
{
    QWidget* w = widget(index);
    removeTab(index);
    delete w;
}

void CodeEditorTabWidget::onCurrentChanged(int index)
{
    QWidget* w = widget(index);
    if (w)
    {
        CodeEditor* v = qobject_cast<CodeEditor*>(w);
        v->grabFocus();
    }
}

void CodeEditorTabWidget::onCustomContextMenuRequested(const QPoint &pos)
{    
    for (int i=0; i<count(); i++)
    {
        //! stupid!
        QRect rc = tabBar()->tabRect(i);
        QPoint pt =  mapToGlobal(pos);
        if ( pt.x() >= mapToGlobal(rc.topLeft()).x() && pt.x() <= mapToGlobal(rc.bottomRight()).x() )
        {
            qDebug() << "found tab " << i;
            tabBar()->setCurrentIndex(i);
            break;
        }
    }
    QMenu menu(this);
    QAction* pCloseAction = new QAction(QIcon(":/image/close.png"), "Close", &menu);
    menu.addAction(pCloseAction);
    connect(pCloseAction, &QAction::triggered, this, &CodeEditorTabWidget::onCloseCurrent);
    QAction* pCloseAllAction = new QAction(QIcon(":/image/closeall.png"), "Close All", &menu);
    menu.addAction(pCloseAllAction);
    connect(pCloseAllAction, &QAction::triggered, this, &CodeEditorTabWidget::onCloseAll);
    QAction* pCloseAllButThisAction = new QAction("Close All But This", &menu);
    menu.addAction(pCloseAllButThisAction);
    connect(pCloseAllButThisAction, &QAction::triggered, this, &CodeEditorTabWidget::onCloseAllButThis);

    if (tabText(currentIndex()) != "Content")
    {
        menu.addSeparator();

        QAction* pCopyFileNameAction = new QAction("Copy File Name", &menu);
        connect(pCopyFileNameAction, &QAction::triggered, this, &CodeEditorTabWidget::onCopyFileName);
        menu.addAction(pCopyFileNameAction);
        QAction* pCopyFileFullPathAction = new QAction("Copy File Full Path", &menu);
        connect(pCopyFileFullPathAction, &QAction::triggered, this, &CodeEditorTabWidget::onCopyFileFullPath);
        menu.addAction(pCopyFileFullPathAction);

        menu.addSeparator();

        QAction* pOpenContainerFolderAction = new QAction("Open Container Folder", &menu);
        connect(pOpenContainerFolderAction, &QAction::triggered, this, &CodeEditorTabWidget::onOpenContainerFolder);
        menu.addAction(pOpenContainerFolderAction);
#if defined(Q_OS_WIN)
        QAction* pOpenFileInVSAction = new QAction("Open File In Visual Studio", &menu);
        connect(pOpenFileInVSAction, &QAction::triggered, this, &CodeEditorTabWidget::onOpenFileInVS);
        menu.addAction(pOpenFileInVSAction);

        CShellContextMenu scm;
        QPoint p = mapToGlobal(pos);
        scm.ShowContextMenu(&menu, this, p, QDir::toNativeSeparators(tabToolTip(currentIndex())));
        return;
#endif
    }
    menu.exec(mapToGlobal(pos));
}

CodeEditor* CodeEditorTabWidget::createCodeEditor(const QString &text, const QString &tooltip)
{
    CodeEditor* v = new CodeEditor(this);
    v->initialize();
    int index = (text == "Content") ? QTabWidget::insertTab(0, v, text) : QTabWidget::addTab(v, text);
    setTabToolTip(index, tooltip);
    setCurrentIndex(index);
    return v;
}
