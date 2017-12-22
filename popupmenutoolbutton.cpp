#include "stdafx.h"
#include <QAction>
#include "popupmenutoolbutton.h"

PopupMenuToolButton::PopupMenuToolButton(QWidget *parent)
    : QToolButton (parent)
{
    setPopupMode(QToolButton::MenuButtonPopup);
    connect(this, SIGNAL(triggered(QAction*)),
                     this, SLOT(setDefaultAction(QAction*)));
}
