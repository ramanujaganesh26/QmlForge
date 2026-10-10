#include "ConsolePane.h"
#include <QHeaderView>
#include <QTime>

ConsolePane::ConsolePane(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);

    // Header layout
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);

    m_statusLabel = new QLabel(tr("Ready"), this);
    m_statusLabel->setStyleSheet("font-weight: bold; font-size: 13px; color: #a6e3a1;");

    m_clearButton = new QPushButton(tr("Clear Console"), this);
    m_clearButton->setFixedWidth(110);
    connect(m_clearButton, &QPushButton::clicked, this, &ConsolePane::clear);

    headerLayout->addWidget(m_statusLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_clearButton);

    mainLayout->addLayout(headerLayout);

    // Table widget for log and error entries
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(4);
    m_tableWidget->setHorizontalHeaderLabels({tr("Time"), tr("Type"), tr("Line:Col"), tr("Message")});
    m_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->setShowGrid(false);

    connect(m_tableWidget, &QTableWidget::itemDoubleClicked, this, &ConsolePane::onItemDoubleClicked);

    mainLayout->addWidget(m_tableWidget);
}

void ConsolePane::clear()
{
    m_tableWidget->setRowCount(0);
    m_statusLabel->setText(tr("Ready"));
    m_statusLabel->setStyleSheet("font-weight: bold; font-size: 13px; color: #89b4fa;");
}

void ConsolePane::showSuccessStatus()
{
    m_tableWidget->setRowCount(0);
    m_statusLabel->setText(tr("✔ Run OK"));
    m_statusLabel->setStyleSheet("font-weight: bold; font-size: 13px; color: #40c057;");

    int row = m_tableWidget->rowCount();
    m_tableWidget->insertRow(row);
    QString timeStr = QTime::currentTime().toString("hh:mm:ss");

    auto *itemTime = new QTableWidgetItem(timeStr);
    auto *itemType = new QTableWidgetItem(tr("INFO"));
    auto *itemLoc = new QTableWidgetItem(tr("-"));
    auto *itemMsg = new QTableWidgetItem(tr("QML loaded successfully."));

    itemType->setForeground(QBrush(QColor("#40c057")));
    itemMsg->setForeground(QBrush(QColor("#40c057")));

    m_tableWidget->setItem(row, 0, itemTime);
    m_tableWidget->setItem(row, 1, itemType);
    m_tableWidget->setItem(row, 2, itemLoc);
    m_tableWidget->setItem(row, 3, itemMsg);
}

void ConsolePane::showQmlErrors(const QList<QQmlError> &errors)
{
    m_tableWidget->setRowCount(0);

    if (errors.isEmpty()) {
        showSuccessStatus();
        return;
    }

    m_statusLabel->setText(tr("✖ %1 Error(s)").arg(errors.size()));
    m_statusLabel->setStyleSheet("font-weight: bold; font-size: 13px; color: #f38ba8;");

    QString timeStr = QTime::currentTime().toString("hh:mm:ss");

    for (const QQmlError &err : errors) {
        int row = m_tableWidget->rowCount();
        m_tableWidget->insertRow(row);

        int line = err.line();
        int col = err.column();
        QString locStr = (line > 0) ? QString("%1:%2").arg(line).arg(col > 0 ? col : 1) : tr("-");

        auto *itemTime = new QTableWidgetItem(timeStr);
        auto *itemType = new QTableWidgetItem(tr("ERROR"));
        auto *itemLoc = new QTableWidgetItem(locStr);
        auto *itemMsg = new QTableWidgetItem(err.description());

        itemTime->setForeground(QBrush(QColor("#f38ba8")));
        itemType->setForeground(QBrush(QColor("#f38ba8")));
        itemLoc->setForeground(QBrush(QColor("#f38ba8")));
        itemMsg->setForeground(QBrush(QColor("#f38ba8")));

        itemLoc->setData(Qt::UserRole, line);
        itemLoc->setData(Qt::UserRole + 1, col > 0 ? col : 1);

        m_tableWidget->setItem(row, 0, itemTime);
        m_tableWidget->setItem(row, 1, itemType);
        m_tableWidget->setItem(row, 2, itemLoc);
        m_tableWidget->setItem(row, 3, itemMsg);
    }
}

void ConsolePane::appendLog(const QString &type, const QString &message, const QString &source, int line)
{
    int row = m_tableWidget->rowCount();
    m_tableWidget->insertRow(row);

    QString timeStr = QTime::currentTime().toString("hh:mm:ss");
    QString locStr = (line > 0) ? QString("%1:1").arg(line) : tr("-");

    auto *itemTime = new QTableWidgetItem(timeStr);
    auto *itemType = new QTableWidgetItem(type);
    auto *itemLoc = new QTableWidgetItem(locStr);
    auto *itemMsg = new QTableWidgetItem(message);

    QColor color = (type == "WARNING" || type == "WARN") ? QColor("#fab387") :
                   (type == "ERROR" || type == "CRITICAL") ? QColor("#f38ba8") : QColor("#cdd6f4");

    itemType->setForeground(QBrush(color));
    itemMsg->setForeground(QBrush(color));

    if (line > 0) {
        itemLoc->setData(Qt::UserRole, line);
        itemLoc->setData(Qt::UserRole + 1, 1);
    }

    m_tableWidget->setItem(row, 0, itemTime);
    m_tableWidget->setItem(row, 1, itemType);
    m_tableWidget->setItem(row, 2, itemLoc);
    m_tableWidget->setItem(row, 3, itemMsg);
}

void ConsolePane::onItemDoubleClicked(QTableWidgetItem *item)
{
    if (!item) return;
    int row = item->row();
    QTableWidgetItem *locItem = m_tableWidget->item(row, 2);
    if (!locItem) return;

    int line = locItem->data(Qt::UserRole).toInt();
    int col = locItem->data(Qt::UserRole + 1).toInt();

    if (line > 0) {
        emit errorDoubleClicked(line, col);
    }
}
