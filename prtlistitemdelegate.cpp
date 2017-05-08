#include "stdafx.h"
#include "prtlistitemdelegate.h"

#if defined(Q_OS_WIN)
#define globalDefaultFontFamily "Microsoft YaHei"
#else
#define globalDefaultFontFamily "PingFang CS"
#endif

PRTListItemDelegate::PRTListItemDelegate(QListWidget *parent)
    : QAbstractItemDelegate (parent),
      m_parentListWidget(parent)
{

}

void PRTListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect r = option.rect;

    //Color: #C4C4C4
    QPen linePen(QColor::fromRgb(211, 211, 211), 1, Qt::SolidLine);

    //Color: #005A83
    QPen lineMarkedPen(QColor::fromRgb(0, 90, 131), 1, Qt::SolidLine);

    //Color: #333
    QPen fontPen(QColor::fromRgb(51, 51, 51), 1, Qt::SolidLine);

    //Color: #fff
    QPen fontMarkedPen(Qt::white, 1, Qt::SolidLine);

    if (option.state & QStyle::State_Selected)
    {
        QLinearGradient gradientSelected(r.left(), r.top(), r.left(), r.height() + r.top());
        gradientSelected.setColorAt(0.0, QColor::fromRgb(119, 213, 247));
        gradientSelected.setColorAt(0.9, QColor::fromRgb(27, 134, 183));
        gradientSelected.setColorAt(1.0, QColor::fromRgb(0, 120, 174));
        painter->setBrush(gradientSelected);
        painter->drawRect(r);

        //BORDER
        painter->setPen(lineMarkedPen);
        painter->drawLine(r.topLeft(), r.topRight());
        painter->drawLine(r.topRight(), r.bottomRight());
        painter->drawLine(r.bottomLeft(), r.bottomRight());
        painter->drawLine(r.topLeft(), r.bottomLeft());

        painter->setPen(fontMarkedPen);
    }
    else
    {
        //BACKGROUND
        //ALTERNATING COLORS
        painter->setBrush((index.row() % 2) ? Qt::white : QColor(252, 252, 252));
        painter->drawRect(r);

        //BORDER
        painter->setPen(linePen);
        painter->drawLine(r.topLeft(), r.topRight());
        painter->drawLine(r.topRight(), r.bottomRight());
        painter->drawLine(r.bottomLeft(), r.bottomRight());
        painter->drawLine(r.topLeft(), r.bottomLeft());

        painter->setPen(fontPen);
    }
    QString title = index.data(Qt::DisplayRole).toString();
    QString topic = index.data(Qt::UserRole + 1).toString();
    QString owner = index.data(Qt::UserRole + 2).toString();
    QString content = index.data(Qt::UserRole + 3).toString();

    painter->setFont(QFont(globalDefaultFontFamily, 12, QFont::Normal));
    painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, title, &r);
}

QSize PRTListItemDelegate::sizeHint(const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    return QSize(m_parentListWidget->width() - 4,
             #if defined(Q_OS_WIN)
                 105
             #else
                 60
             #endif
                 );
}
