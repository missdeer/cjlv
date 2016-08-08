#include "stdafx.h"
#include <QFontDatabase>
#include "qtsingleapplication.h"
#include "mainwindow.h"
#include "settings.h"

#if defined(Q_OS_WIN)
#include "everythingwrapper.h"
#endif

int main(int argc, char *argv[])
{
#if !defined(Q_OS_WIN)
    // increase the number of file that can be opened.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin((rlim_t)OPEN_MAX, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif
    SharedTools::QtSingleApplication a("Cisco Jabber Log Viewer", argc, argv);

    QStringList fonts {
        "SourceCodePro-Regular.ttf",
        "SourceCodePro-Black.ttf",
        "SourceCodePro-Bold.ttf",
        "SourceCodePro-ExtraLight.ttf",
        "SourceCodePro-Light.ttf",
        "SourceCodePro-Medium.ttf",
        "SourceCodePro-Semibold.ttf"
    };
    QString fontPath = QApplication::applicationDirPath();
    QDir dir(fontPath);
#if !defined(Q_OS_MAC)
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling, true);
#else
    QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
    dir.cdUp();
    dir.cd("Resources");
#endif
    dir.cd("Fonts");
    foreach(const QString& f, fonts)
    {
        fontPath = dir.absolutePath() + QDir::separator() + f;
        if (QFile::exists(fontPath))
            QFontDatabase::addApplicationFont(fontPath);
        else
            QMessageBox::critical(NULL, "Font missing", fontPath, QMessageBox::Ok);
    }

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
	
    QCoreApplication::setApplicationName("Cisco Jabber Log Viewer");
    QCoreApplication::setApplicationVersion("1.1");

    QDate d =  QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy"));
    if (d.daysTo(QDate::currentDate()) > 365)
    {
        QMessageBox::critical(NULL, "Expired", "This application has been expired, please contact fyang3@cisco.com for a new build.", QMessageBox::Ok );
        return 1;
    }

    QCommandLineParser parser;
    parser.setApplicationDescription("Cisco Jabber Log Viewer");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption(QCommandLineOption(QStringList() << "f" << "file", "Save temporary database into local file system."));
    parser.addOption(QCommandLineOption(QStringList() << "t" <<  "temporary-directory", "Save temporary files into <directory>.", "directory"));
    parser.addOption(QCommandLineOption(QStringList() << "s" <<  "source-directory", "Source code stored in <directory>.", "directory"));
    parser.addPositionalArgument("logs", "logs files, zip packages or folders.");
    // Process the actual command line arguments given by the user
    parser.process(a);

    g_settings = new Settings;
    g_settings->initialize();
    QString t = parser.value("t");
    if (!t.isEmpty())
        g_settings->setTemporaryDirectory( t );

    if (g_settings->temporaryDirectory().isEmpty())
    {
        t = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        t.append("/CiscoJabberLogs/database");
        g_settings->setTemporaryDirectory( t );
    }

    if (!parser.value("s").isEmpty())
        g_settings->setSourceDirectory( parser.value("s"));

    g_settings->setInMemoryDatabase(!parser.isSet("f"));

    const QStringList logs = parser.positionalArguments();

    if (a.isRunning())
    {
        if (!logs.isEmpty())
        {
            a.sendMessage(logs.join("\n"));
        }
        return 0;
    }
	
    MainWindow w;
    w.showMaximized();
    w.openLogs(logs);

#if defined(Q_OS_WIN)
    if (isEverythingRunning())
    {
        const QString& path = GetEverythingPath();
        g_settings->setEverythingPath(path);
        g_settings->save();
    }
    else
    {
        launchEverything(g_settings->everythingPath());
    }
#endif

    QObject::connect(&a, SIGNAL(messageReceived(QString,QObject*)), &w, SLOT(onIPCMessageReceived(QString,QObject*)));

    return a.exec();
}
