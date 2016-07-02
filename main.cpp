#include <QtCore>
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDateTime>
#include <QMessageBox>
#include "mainwindow.h"
#include "settings.h"

#if defined(Q_OS_WIN)
void launchEverything();
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Cisco Jabber Log Viewer");
    QCoreApplication::setApplicationVersion("1.0");

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

    MainWindow w;
    w.showMaximized();
    w.openLogs(logs);

#if defined(Q_OS_WIN)
    launchEverything();
#endif

    return a.exec();
}
