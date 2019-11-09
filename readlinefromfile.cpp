#include "stdafx.h"

#include "readlinefromfile.h"

ReadLineFromFile::ReadLineFromFile(const QString &fileName)
    : m_file(fileName)
    , m_offset(0)
    , m_fileSize(0)
    , m_lineStartPos(nullptr)
    , m_mapStartPos(nullptr)
    , m_mapEndPos(nullptr)
{
    if (m_file.open(QIODevice::ReadOnly))
    {
        m_fileSize = m_file.size();
        if (m_fileSize)
        {
            m_lineStartPos = m_mapStartPos = m_file.map(m_offset, qMin(mapSize, m_fileSize - m_offset));
            m_mapEndPos                    = m_mapStartPos + qMin(mapSize, m_fileSize - m_offset);
        }
    }
}

ReadLineFromFile::~ReadLineFromFile()
{
    if (m_mapStartPos)
        m_file.unmap(m_mapStartPos);
    m_file.close();
}

QByteArray ReadLineFromFile::readLine()
{
    for (;;)
    {
        uchar *p = m_lineStartPos;
        while (p < m_mapEndPos && *p != '\n')
            p++;
        if (p < m_mapEndPos)
        {
            QByteArray b(reinterpret_cast<const char *>(m_lineStartPos), p - m_lineStartPos);
            m_lineStartPos = p + 1;
            return b;
        }

        // re-map
        m_file.unmap(m_mapStartPos);
        m_offset += m_lineStartPos - m_mapStartPos;
        if (m_offset == m_fileSize)
            break;
        m_lineStartPos = m_mapStartPos = m_file.map(m_offset, qMin(mapSize, m_fileSize - m_offset));
        m_mapEndPos                    = m_mapStartPos + qMin(mapSize, m_fileSize - m_offset);
    }
    return QByteArray();
}

bool ReadLineFromFile::atEnd() const
{
    return m_offset == m_fileSize;
}
