#include <QApplication>
#include "MainWindow.h"

#ifndef QMLFORGE_VERSION
#define QMLFORGE_VERSION "1.0.0"
#endif

static void qmlForgeMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (MainWindow::instance()) {
        MainWindow::instance()->logQtMessage(type, context, msg);
    }

    // Default console output fallback
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u)\n", localMsg.constData(), context.file ? context.file : "", context.line);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u)\n", localMsg.constData(), context.file ? context.file : "", context.line);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u)\n", localMsg.constData(), context.file ? context.file : "", context.line);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u)\n", localMsg.constData(), context.file ? context.file : "", context.line);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u)\n", localMsg.constData(), context.file ? context.file : "", context.line);
        break;
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(qmlForgeMessageHandler);

    QApplication app(argc, argv);
    QApplication::setApplicationName("QmlForge");
    QApplication::setApplicationVersion(QMLFORGE_VERSION);
    QApplication::setOrganizationName("QmlForge");
    QApplication::setOrganizationDomain("qmlforge.org");

    MainWindow window;
    window.show();

    return QApplication::exec();
}
