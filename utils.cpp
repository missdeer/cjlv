#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
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
//        QString xmlOut;

//        QDomDocument doc;
//        doc.setContent(xmlIn, false);
//        QTextStream writer(&xmlOut);
//        doc.save(writer, 4);

        using namespace rapidxml;
        xml_document<> doc;
        std::istringstream in(xmlIn.toStdString());
        std::vector<char> buffer((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        buffer.push_back('\0');
        doc.parse<0>(&buffer[0]);

        QString xmlOut;
        print(std::back_inserter(xmlOut), doc, 0);

        header.append("\n");
        header.append(xmlOut);
        if (header.length() > text.length())
            return header;
    }

    return text;
}
}
