#ifndef CONSOLEPANE_H
#define CONSOLEPANE_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QQmlError>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>

class ConsolePane : public QWidget
{
    Q_OBJECT

public:
    explicit ConsolePane(QWidget *parent = nullptr);

    void showQmlErrors(const QList<QQmlError> &errors);
    void appendLog(const QString &type, const QString &message, const QString &source = QString(), int line = -1);
    void showSuccessStatus();
    void clear();

signals:
    void errorDoubleClicked(int line, int col);

private slots:
    void onItemDoubleClicked(QTableWidgetItem *item);

private:
    QLabel *m_statusLabel{nullptr};
    QTableWidget *m_tableWidget{nullptr};
    QPushButton *m_clearButton{nullptr};
};

#endif // CONSOLEPANE_H
