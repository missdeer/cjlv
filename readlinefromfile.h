#ifndef READLINEFROMFILE_H
#define READLINEFROMFILE_H

#include <QFile>

class ReadLineFromFile
{
    QFile m_file;
    qint64 m_offset;
    qint64 m_fileSize;
    uchar* m_lineStartPos;
    uchar* m_mapStartPos;
    uchar* m_mapEndPos;
    const qint64 mapSize = 1024 * 1024; // 1M
public:
    ReadLineFromFile(const QString& fileName);

    ~ReadLineFromFile();

    QByteArray readLine();

    bool atEnd() const;
};


#endif // READLINEFROMFILE_H
