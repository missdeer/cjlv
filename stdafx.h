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
#include <QtGui>
#include <QSettings>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QFileDialog>
#include <QTemporaryFile>
#include <QDate>
#include <QDesktopServices>
#include <QDateTime>
#include <QThreadPool>
#include <QCryptographicHash>
#include <QProgressDialog>
#include <QStandardPaths>
#include <QDomDocument>
#include <QTextStream>
#include <QStringBuilder>
#include <QApplication>
#include <QMenu>
#include <QClipboard>
#include <QSplitter>
#include <QHeaderView>
#include <QtConcurrent>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QSharedPointer>
#include <QTabWidget>
#include <QDialog>
#include <QItemSelection>
#include <QAbstractTableModel>
#include <QList>
#include <QFuture>
#include <QMainWindow>
#include <QPoint>
#include <QTableView>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlDriver>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QLineEdit>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QHBoxLayout>

#include <QtCharts>

#if defined(Q_OS_WIN)
#include <QWinThumbnailToolBar>
#include <QWinThumbnailToolButton>
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif

#endif

#endif // STDAFX_H
