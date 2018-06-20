
win32-*msvc*: {
    INCLUDEPATH += $$PWD
    SOURCES += $$PWD/sqlite3.c
} else {
    LIBS += -lsqlite3
}
