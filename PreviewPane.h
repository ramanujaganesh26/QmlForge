#ifndef PREVIEWPANE_H
#define PREVIEWPANE_H

#include <QWidget>
#include <QQuickWidget>
#include <QQmlError>
#include <QUrl>
#include <QVBoxLayout>
#include <QColor>
#include <QImage>

class PreviewPane : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewPane(QWidget *parent = nullptr);

    void loadQmlCode(const QString &code, const QUrl &baseUrl = QUrl());
    void clearPreview();
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    QList<QQmlError> errors() const;
    QQuickWidget::Status status() const;

    QImage exportToImage();
    void setImportPath(const QString &dirPath);

signals:
    void statusChanged(QQuickWidget::Status status);
    void errorsOccurred(const QList<QQmlError> &errors);

private slots:
    void onWidgetStatusChanged(QQuickWidget::Status status);

private:
    QQuickWidget *m_quickWidget{nullptr};
    QVBoxLayout *m_layout{nullptr};
    QUrl m_currentBaseUrl;
    QColor m_backgroundColor{"#181825"};
    QList<QQmlError> m_lastErrors;
    QObject *m_currentRootObj{nullptr};
};

#endif // PREVIEWPANE_H
