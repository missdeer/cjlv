#ifndef STDAFX_H
#define STDAFX_H

#if defined(_WIN32)
#    define NOMINMAX
#    include <Everything.h>
#    include <PSapi.h>
#    include <Shellapi.h>
#    include <Shlobj.h>
#    include <Windows.h>
#    include <everything_ipc.h>
#    include <shlguid.h>
#    include <shobjidl.h>
#    include <strsafe.h>
#endif

// it's needed by clang
#if defined(__cplusplus)

#    include <QAbstractTableModel>
#    include <QApplication>
#    include <QClipboard>
#    include <QCommandLineOption>
#    include <QCommandLineParser>
#    include <QCryptographicHash>
#    include <QDate>
#    include <QDateTime>
#    include <QDesktopServices>
#    include <QDesktopWidget>
#    include <QDialog>
#    include <QDir>
#    include <QDomDocument>
#    include <QDragEnterEvent>
#    include <QDropEvent>
#    include <QFile>
#    include <QFileDialog>
#    include <QFileInfo>
#    include <QFuture>
#    include <QHBoxLayout>
#    include <QHeaderView>
#    include <QInputDialog>
#    include <QItemSelection>
#    include <QJsonDocument>
#    include <QJsonObject>
#    include <QLineEdit>
#    include <QList>
#    include <QMainWindow>
#    include <QMenu>
#    include <QMessageBox>
#    include <QNetworkAccessManager>
#    include <QNetworkReply>
#    include <QNetworkRequest>
#    include <QPoint>
#    include <QProcess>
#    include <QProgressDialog>
#    include <QQmlContext>
#    include <QQmlEngine>
#    include <QQuickWidget>
#    include <QSettings>
#    include <QSharedPointer>
#    include <QShortcut>
#    include <QSplitter>
#    include <QSslError>
#    include <QStandardPaths>
#    include <QStringBuilder>
#    include <QTabWidget>
#    include <QTableView>
#    include <QTemporaryFile>
#    include <QTextStream>
#    include <QThreadPool>
#    include <QTimer>
#    include <QUrl>
#    include <QWaitCondition>
#    include <QtCharts>
#    include <QtConcurrent>
#    include <QtCore>
#    include <QtGui>
#    include <atomic>

#    if defined(Q_OS_WIN)
#        include <QWinTaskbarButton>
#        include <QWinTaskbarProgress>
#        include <QWinThumbnailToolBar>
#        include <QWinThumbnailToolButton>
#    elif defined(Q_OS_MAC)
#        include "macheader.h"
#    endif

bool QuickGetFilesByFileName(const QString &fileName, QStringList &results);
#endif

#endif // STDAFX_H
