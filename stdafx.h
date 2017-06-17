#ifndef STDAFX_H
#define STDAFX_H


#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#include <Shellapi.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <Shlobj.h>
#include <strsafe.h>
#include <PSapi.h>
#include <Everything.h>
#include <everything_ipc.h>
#endif

#if defined (__cplusplus)

#include <QtCore>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QDate>
#include <QDesktopServices>
#include <QDateTime>
#include <QList>
#include <QPoint>
#include <QClipboard>
#include <QApplication>
#include <QTextStream>
#include <QStringBuilder>
#include <QStandardPaths>
#include <QCryptographicHash>

#include <QtGui>
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMenu>
#include <QSplitter>
#include <QHeaderView>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QSharedPointer>
#include <QTabWidget>
#include <QItemSelection>
#include <QAbstractTableModel>
#include <QMainWindow>
#include <QTableView>
#include <QShortcut>

#include <QTimer>
#include <QProcess>
#include <QDomDocument>

#include <QJsonDocument>
#include <QJsonObject>

#include <QtConcurrent>
#include <QFuture>
#include <QWaitCondition>
#include <QThreadPool>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlDriver>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QSslError>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QLineEdit>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QHBoxLayout>

#include <QtCharts>
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>

#if defined(Q_OS_WIN)
#include <QWinThumbnailToolBar>
#include <QWinThumbnailToolButton>
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif

#endif

#endif // STDAFX_H
