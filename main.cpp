#include "stdafx.h"
#include <QFontDatabase>
#include <boost/scope_exit.hpp>
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

    QPixmap pixmap(":/cjlv.png");
    QSplashScreen splash(pixmap.scaled(500, 500));
    splash.show();
    a.processEvents();

    splash.showMessage("Loading external fonts...");
    QStringList fonts {
        ":/Fonts/SourceCodePro-Regular.ttf",
        ":/Fonts/SourceCodePro-Black.ttf",
        ":/Fonts/SourceCodePro-Bold.ttf",
        ":/Fonts/SourceCodePro-ExtraLight.ttf",
        ":/Fonts/SourceCodePro-Light.ttf",
        ":/Fonts/SourceCodePro-Medium.ttf",
        ":/Fonts/SourceCodePro-Semibold.ttf"
    };

    foreach(const QString& f, fonts)
    {
        QFontDatabase::addApplicationFont(f);
    }

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
	
    QCoreApplication::setApplicationName("Cisco Jabber Log Viewer");
    QCoreApplication::setApplicationVersion("1.1");

    splash.showMessage("Checking evaluating date...");
    QDate d =  QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy"));
    if (d.daysTo(QDate::currentDate()) > 365)
    {
        QMessageBox::critical(NULL, "Expired", "This application has been expired, please contact fyang3@cisco.com for a new build.", QMessageBox::Ok );
        return 1;
    }

    splash.showMessage("Parsing command line options...");
    QCommandLineParser parser;
    parser.setApplicationDescription("Cisco Jabber Log Viewer");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption(QCommandLineOption(QStringList() << "f" << "file", "Save temporary database into local file system."));
    parser.addOption(QCommandLineOption(QStringList() << "t" << "temporary-directory", "Save temporary files into <directory>.", "directory"));
    parser.addOption(QCommandLineOption(QStringList() << "s" << "source-directory", "Source code stored in <directory>.", "directory"));
    parser.addOption(QCommandLineOption(QStringList() << "m" << "disable-multi-monitor", "Don't show log view and source code view in different monitors."));
    parser.addPositionalArgument("logs", "logs files, zip packages or folders.");
    // Process the actual command line arguments given by the user
    parser.process(a);

    splash.showMessage("Applying global configurations...");
    g_settings = new Settings;
    BOOST_SCOPE_EXIT(g_settings) {
        delete g_settings;
    } BOOST_SCOPE_EXIT_END
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
    g_settings->setMultiMonitorEnabled(!parser.isSet("m") && QApplication::desktop()->screenCount() > 1);

    const QStringList logs = parser.positionalArguments();

    if (a.isRunning())
    {
        if (!logs.isEmpty())
        {
            a.sendMessage(logs.join("\n"));
        }
        return 0;
    }

    splash.showMessage("Creating main window...");
    MainWindow w(splash);
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

    splash.finish(&w);
    return a.exec();
}
