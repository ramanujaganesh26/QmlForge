#include "PreviewPane.h"
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QQuickItem>
#include <QDir>
#include <QFileInfo>

PreviewPane::PreviewPane(QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_quickWidget = new QQuickWidget(this);
    m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_quickWidget->setClearColor(m_backgroundColor);

    connect(m_quickWidget, &QQuickWidget::statusChanged, this, &PreviewPane::onWidgetStatusChanged);

    m_layout->addWidget(m_quickWidget);
}

void PreviewPane::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    if (m_quickWidget) {
        m_quickWidget->setClearColor(color);
    }
}

QColor PreviewPane::backgroundColor() const
{
    return m_backgroundColor;
}

void PreviewPane::setImportPath(const QString &dirPath)
{
    if (m_quickWidget && m_quickWidget->engine()) {
        if (!dirPath.isEmpty()) {
            m_quickWidget->engine()->addImportPath(dirPath);
        }
    }
}

void PreviewPane::loadQmlCode(const QString &code, const QUrl &baseUrl)
{
    if (!m_quickWidget || !m_quickWidget->engine()) return;

    // Delete previous root object if dynamically created
    if (m_currentRootObj) {
        delete m_currentRootObj;
        m_currentRootObj = nullptr;
    }

    // Clear engine component cache so edits always show immediately
    m_quickWidget->engine()->clearComponentCache();

    QUrl resolvedUrl = baseUrl;
    if (!resolvedUrl.isValid() || resolvedUrl.isEmpty()) {
        resolvedUrl = QUrl::fromLocalFile(QDir::currentPath() + "/");
    }

    if (resolvedUrl.isLocalFile()) {
        QFileInfo fi(resolvedUrl.toLocalFile());
        QString dirPath = fi.isDir() ? fi.absoluteFilePath() : fi.absolutePath();
        m_quickWidget->engine()->addImportPath(dirPath);
        m_quickWidget->engine()->setBaseUrl(QUrl::fromLocalFile(dirPath + "/"));
    }

    m_currentBaseUrl = resolvedUrl;

    QQmlComponent *component = new QQmlComponent(m_quickWidget->engine(), this);
    component->setData(code.toUtf8(), resolvedUrl);

    if (component->isError()) {
        m_lastErrors = component->errors();
        emit errorsOccurred(m_lastErrors);
        delete component;
        return;
    }

    QObject *rootObj = component->create();
    if (!rootObj || component->isError()) {
        m_lastErrors = component->errors();
        emit errorsOccurred(m_lastErrors);
        delete component;
        return;
    }

    m_currentRootObj = rootObj;

    // If root object is a top-level Window / ApplicationWindow, prevent popup OS window
    QQuickWindow *win = qobject_cast<QQuickWindow*>(rootObj);
    if (win) {
        win->hide();
        win->setVisible(false);

        QVariant colorVar = win->property("color");
        if (colorVar.isValid() && colorVar.canConvert<QColor>()) {
            QColor c = colorVar.value<QColor>();
            if (c.isValid()) {
                m_quickWidget->setClearColor(c);
            }
        }

        QQuickItem *contentItem = win->contentItem();
        if (contentItem) {
            m_quickWidget->setContent(resolvedUrl, component, contentItem);
        } else {
            m_quickWidget->setContent(resolvedUrl, component, rootObj);
        }
    } else {
        m_quickWidget->setContent(resolvedUrl, component, rootObj);
    }

    m_lastErrors.clear();
}

void PreviewPane::clearPreview()
{
    if (!m_quickWidget || !m_quickWidget->engine()) return;

    if (m_currentRootObj) {
        delete m_currentRootObj;
        m_currentRootObj = nullptr;
    }

    m_quickWidget->engine()->clearComponentCache();
    m_quickWidget->setContent(QUrl(), nullptr, nullptr);
    m_lastErrors.clear();
}

QList<QQmlError> PreviewPane::errors() const
{
    if (!m_lastErrors.isEmpty()) {
        return m_lastErrors;
    }
    return m_quickWidget ? m_quickWidget->errors() : QList<QQmlError>();
}

QQuickWidget::Status PreviewPane::status() const
{
    return m_quickWidget ? m_quickWidget->status() : QQuickWidget::Null;
}

void PreviewPane::onWidgetStatusChanged(QQuickWidget::Status status)
{
    emit statusChanged(status);
    if (status == QQuickWidget::Error) {
        emit errorsOccurred(m_quickWidget->errors());
    }
}

QImage PreviewPane::exportToImage()
{
    if (!m_quickWidget) return QImage();
    return m_quickWidget->grabFramebuffer();
}
