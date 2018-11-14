
DEFINES += SQLITE_ENABLE_FTS5=1
win32-*msvc*: {
    INCLUDEPATH += $$PWD
    SOURCES += $$PWD/sqlite3.c
} else {
    LIBS += -lsqlite3
}
