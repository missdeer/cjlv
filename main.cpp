#include <QtCore>
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDateTime>
#include <QMessageBox>
#include "mainwindow.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const char *compilation_date = __DATE__;
    const char *months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                        "Jul","Aug","Sep","Oct","Nov","Dec",NULL};
    int     i;
    QDate rc;

    for( i = 0; months[i] != NULL; i++ )
        if( memcmp( compilation_date, months[i], 3 ) == 0 )
            break;

    if( months[i] == NULL )
        rc = QDate( 1900,01,01 );
    else
    {
        char year[5], day[3];

        memcpy(year,compilation_date+7,4);
        year[4] = 0x00;
        memcpy(day,compilation_date+4,2);
        day[2] = 0x00;

        rc = QDate(atoi(year),i+1,atoi(day));
    }

    qDebug() << __DATE__ <<rc << QDate::currentDate() << rc.daysTo(QDate::currentDate());
    if (rc.daysTo(QDate::currentDate()) > 365)
    {
        QMessageBox::critical(NULL, "Expired", "This application has been expired, please contact fyang3@cisco.com for a new build.", QMessageBox::Ok );
        return 1;
    }

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
