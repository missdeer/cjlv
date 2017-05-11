#include "stdafx.h"
#include "shortcutedit.h"

ShortcutEdit::ShortcutEdit(QWidget *parent)
    : QLineEdit (parent)
{

}

bool ShortcutEdit::event(QEvent *e)
{
    if (e->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);

        int keyInt = keyEvent->key();
        Qt::Key key = static_cast<Qt::Key>(keyInt);
        if(key == Qt::Key_unknown)
        {
            qDebug() << "Unknown key from a macro probably";
            return false;
        }
        // the user have clicked just and only the special keys Ctrl, Shift, Alt, Meta.
        if(key == Qt::Key_Control ||
                key == Qt::Key_Shift ||
                key == Qt::Key_Alt ||
                key == Qt::Key_Meta)
        {
            qDebug() << "Single click of special key: Ctrl, Shift, Alt or Meta";
            return false;
        }

        // check for a combination of user clicks
        Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
        QString keyText = keyEvent->text();
        // if the keyText is empty than it's a special key like F1, F5, ...
        qDebug() << "Pressed Key:" << keyText;

        std::map<Qt::KeyboardModifier, Qt::Modifier> m = {
            { Qt::ShiftModifier, Qt::SHIFT},
            { Qt::ControlModifier, Qt::CTRL},
            { Qt::AltModifier, Qt::ALT},
            { Qt::MetaModifier, Qt::META},
        };

        for (auto k : m)
        {
            if (modifiers & k.first)
                keyInt += k.second;
        }
        QString shortcut = text();
        if (shortcut.contains(", ") || shortcut.isEmpty())
            setText( QKeySequence(keyInt).toString());
        else
            setText( shortcut % ", " % QKeySequence(keyInt).toString());
        return true;
    }
    return QLineEdit::event(e);
}
