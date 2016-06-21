#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include "mainwindow.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Cisco Jabber Log Viewer");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption(QCommandLineOption(QStringList() << "t" <<  "temporary-directory", "Save temporary files into <directory>.", "directory"));
    parser.addOption(QCommandLineOption(QStringList() << "s" <<  "source-directory", "Source code stored in <directory>.", "directory"));

    // Process the actual command line arguments given by the user
    parser.process(a);

    g_settings.setTemporaryDirectory( parser.value("t"));
    g_settings.setSourceDirectory( parser.value("s"));

    MainWindow w;
    w.showMaximized();

    return a.exec();
}
