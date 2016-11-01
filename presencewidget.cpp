#include "stdafx.h"
#include "presencemodel.h"
#include "presencewidget.h"

PresenceWidget::PresenceWidget(QWidget *parent)
    : QWidget(parent)
    , m_presenceTableView(new QTableView(this))
{
    QVBoxLayout* m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);

    QWidget* topBar = new QWidget(this);
    QHBoxLayout* topBarLayout = new QHBoxLayout;
    topBarLayout->setMargin(0);
    topBar->setLayout(topBarLayout);
    QLabel* label = new QLabel(topBar);
    label->setText("Buddy List:");
    topBarLayout->addWidget(label);
    m_cbBuddyList = new QComboBox(topBar);
    topBarLayout->addWidget(m_cbBuddyList);

    QPushButton* btnRefreshBuddyList = new QPushButton(topBar);
    btnRefreshBuddyList->setText("Refresh");
    connect(btnRefreshBuddyList, &QPushButton::pressed, this, &PresenceWidget::onRefreshBuddyList);
    topBarLayout->addWidget(btnRefreshBuddyList);
    topBarLayout->setStretch(1, 1);

    QPushButton* btnResizeRowsToContents = new QPushButton(topBar);
    btnResizeRowsToContents->setText("Rows Fit");
    connect(btnResizeRowsToContents, &QPushButton::pressed, [&](){m_presenceTableView->resizeRowsToContents();});
    topBarLayout->addWidget(btnResizeRowsToContents);

    m_mainLayout->addWidget(topBar);
    m_mainLayout->addWidget(m_presenceTableView);

    setLayout(m_mainLayout);

    m_model = new PresenceModel(m_presenceTableView);
    m_presenceTableView->setModel(m_model);

    connect(m_model, &PresenceModel::receivedPresenceBuddyList, this, &PresenceWidget::onReceivedPresenceBuddyList);
    connect(this, &PresenceWidget::databaseCreated, m_model, &PresenceModel::onDatabaseCreated);
    connect(m_cbBuddyList, &QComboBox::currentTextChanged, m_model, &PresenceModel::onSelectedJIDChanged);
    connect(m_model, &PresenceModel::resizeTableCells, this, &PresenceWidget::onResizeTableCells);
}

void PresenceWidget::onRefreshBuddyList()
{
    m_model->requestReceivedPresenceBuddyList();
}

void PresenceWidget::onReceivedPresenceBuddyList(QStringList bl)
{
    m_cbBuddyList->clear();
    bl.insert(0, "--NONE--");
    m_cbBuddyList->addItems(bl);
}

void PresenceWidget::onResizeTableCells(int columns, int rows)
{
    m_presenceTableView->resizeColumnToContents(0);
    m_presenceTableView->resizeColumnToContents(1);
}
