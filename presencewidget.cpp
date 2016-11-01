#include "stdafx.h"
#include "presencemodel.h"
#include "presencewidget.h"

PresenceWidget::PresenceWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);

    QWidget* topBar = new QWidget(this);
    QHBoxLayout* topBarLayout = new QHBoxLayout;
    topBarLayout->setMargin(0);
    topBar->setLayout(topBarLayout);
    QComboBox* cbBuddyList = new QComboBox(topBar);
    topBarLayout->addWidget(cbBuddyList);

    m_mainLayout->addWidget(topBar);
    QTableView* presenceTableView = new QTableView(this);
    m_mainLayout->addWidget(presenceTableView);

    setLayout(m_mainLayout);

    m_model = new PresenceModel(presenceTableView);
    presenceTableView->setModel(m_model);
}
