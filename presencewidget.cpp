#include "stdafx.h"

#include "presencewidget.h"

#include "presencemodel.h"

PresenceWidget::PresenceWidget(QWidget *parent, Sqlite3HelperPtr sqlite3Helper) : QWidget(parent), m_presenceTableView(new QTableView(this))
{
    auto *m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);

    auto *topBar       = new QWidget(this);
    auto *topBarLayout = new QHBoxLayout;
    topBarLayout->setMargin(0);
    topBar->setLayout(topBarLayout);
    auto *label = new QLabel(topBar);
    label->setText("Buddy List:");
    topBarLayout->addWidget(label);
    m_cbBuddyList = new QComboBox(topBar);
    topBarLayout->addWidget(m_cbBuddyList);

    auto *btnRefreshBuddyList = new QPushButton(topBar);
    btnRefreshBuddyList->setText("Refresh");
    connect(btnRefreshBuddyList, &QPushButton::pressed, this, &PresenceWidget::onRefreshBuddyList);
    topBarLayout->addWidget(btnRefreshBuddyList);
    topBarLayout->setStretch(1, 1);

    auto *btnResizeRowsToContents = new QPushButton(topBar);
    btnResizeRowsToContents->setText("Rows Fit");
    connect(btnResizeRowsToContents, &QPushButton::pressed, [&]() { m_presenceTableView->resizeRowsToContents(); });
    topBarLayout->addWidget(btnResizeRowsToContents);

    m_mainLayout->addWidget(topBar);
    m_mainLayout->addWidget(m_presenceTableView);

    setLayout(m_mainLayout);

    m_model = new PresenceModel(m_presenceTableView, sqlite3Helper);
    m_presenceTableView->setModel(m_model);
    m_presenceTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_presenceTableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(m_model, &PresenceModel::receivedPresenceBuddyList, this, &PresenceWidget::onReceivedPresenceBuddyList);
    connect(this, &PresenceWidget::databaseCreated, m_model, &PresenceModel::onDatabaseCreated);
    connect(m_cbBuddyList, &QComboBox::currentTextChanged, m_model, &PresenceModel::onSelectedJIDChanged);
    connect(m_model, &PresenceModel::resizeTableCells, this, &PresenceWidget::onResizeTableCells);
}

void PresenceWidget::onRefreshBuddyList()
{
    if (!m_cbBuddyList->count())
        m_model->requestReceivedPresenceBuddyList();
}

void PresenceWidget::onReceivedPresenceBuddyList(QStringList bl)
{
    m_cbBuddyList->clear();
    std::sort(bl.begin(), bl.end());
    bl.insert(0, "--NONE--");
    m_cbBuddyList->addItems(bl);
}

void PresenceWidget::onResizeTableCells(int columns, int /*rows*/)
{
    m_presenceTableView->resizeColumnToContents(0);
    m_presenceTableView->resizeColumnToContents(1);
    if (columns > 2)
    {
        QRect rc = m_presenceTableView->geometry();
        int   leftWidth =
            rc.width() - m_presenceTableView->horizontalHeader()->sectionSize(0) - m_presenceTableView->horizontalHeader()->sectionSize(1);
        int toWidth = leftWidth / (columns - 2);
        for (int i = 2; i < columns; i++)
            m_presenceTableView->setColumnWidth(i, toWidth);
    }
    m_presenceTableView->resizeRowsToContents();
}
