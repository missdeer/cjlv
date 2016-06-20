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

    parser.addOptions({
        {{"t", "temporary-directory"}, "Save temporary files into <directory>.", "directory"},
    });

    // Process the actual command line arguments given by the user
    parser.process(a);

    g_settings.setTemporaryDirectory( parser.value("t"));

    MainWindow w;
    w.show();

    return a.exec();
}
