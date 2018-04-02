#include "stdafx.h"
#include "utils.h"

namespace Utils {
QString formatXML(const QString &text)
{
    // try to format XML document
    int startPos = text.indexOf(QChar('<'));
    int endPos = text.lastIndexOf(QChar('>'));
    if (startPos > 0 && endPos > startPos)
    {
        QString header = text.mid(0, startPos);
        QString xmlIn = text.mid(startPos, endPos - startPos + 1);
#ifndef QT_NO_DEBUG
        qDebug() << "raw text:" << text;
        qDebug() << "xml in:" << xmlIn;
#endif
        QString xmlOut;

        QDomDocument doc;
        doc.setContent(xmlIn, false);
        QTextStream writer(&xmlOut);
        doc.save(writer, 4);

        header.append("\n");
        header.append(xmlOut);
        if (header.length() > text.length())
            return header;
    }

    return text;
}
}
