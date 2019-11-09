#ifndef SHORTCUTEDIT_H
#define SHORTCUTEDIT_H

#include <QLineEdit>

QT_BEGIN_NAMESPACE
class QEvent;
QT_END_NAMESPACE

class ShortcutEdit : public QLineEdit
{
public:
    explicit ShortcutEdit(QWidget *parent = 0);
    bool event(QEvent *e);
};

#endif // SHORTCUTEDIT_H
