#include "MainWindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QColorDialog>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>
#include <QStyle>
#include <QApplication>
#include <QKeySequence>

#ifndef QMLFORGE_VERSION
#define QMLFORGE_VERSION "1.0.0"
#endif

MainWindow *MainWindow::s_instance = nullptr;

static const char *STARTER_QML = R"(import QtQuick
import QtQuick.Controls

Rectangle {
    width: 400
    height: 400
    color: "#1e1e2e"
}
)";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    s_instance = this;
    setAcceptDrops(true);
    setWindowTitle(tr("QmlForge v%1 - QML Live Playground").arg(QMLFORGE_VERSION));
    setMinimumSize(900, 600);

    m_autoRunTimer = new QTimer(this);
    m_autoRunTimer->setSingleShot(true);
    m_autoRunTimer->setInterval(300);
    connect(m_autoRunTimer, &QTimer::timeout, this, &MainWindow::runQml);

    setupUi();
    createActions();
    createMenus();
    createToolbar();
    createStatusBar();

    m_highlighter = new QmlHighlighter(m_codeEditor->document());

    // Connect editor signals
    connect(m_codeEditor, &CodeEditor::textChanged, this, &MainWindow::onEditorTextChanged);
    connect(m_codeEditor, &CodeEditor::cursorPositionInfoChanged, this, &MainWindow::updateStatusBarCursor);

    // Connect preview and console signals
    connect(m_previewPane, &PreviewPane::errorsOccurred, m_consolePane, &ConsolePane::showQmlErrors);
    connect(m_consolePane, &ConsolePane::errorDoubleClicked, this, &MainWindow::jumpToEditorLineCol);

    loadSettings();

    // Default starter QML code if editor is empty
    if (m_codeEditor->toPlainText().trimmed().isEmpty()) {
        m_codeEditor->setPlainText(STARTER_QML);
        m_isModified = false;
        setCurrentFile("");
    }

    // Initial run
    runQml();
}

MainWindow::~MainWindow()
{
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

void MainWindow::logQtMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (!m_consolePane) return;

    QString typeStr = "INFO";
    switch (type) {
    case QtDebugMsg: typeStr = "DEBUG"; break;
    case QtInfoMsg: typeStr = "INFO"; break;
    case QtWarningMsg: typeStr = "WARN"; break;
    case QtCriticalMsg: typeStr = "CRITICAL"; break;
    case QtFatalMsg: typeStr = "FATAL"; break;
    }

    int line = context.line > 0 ? context.line : -1;
    m_consolePane->appendLog(typeStr, msg, QString::fromUtf8(context.file), line);
}

void MainWindow::setupUi()
{
    m_verticalSplitter = new QSplitter(Qt::Vertical, this);
    m_mainHorizontalSplitter = new QSplitter(Qt::Horizontal, m_verticalSplitter);

    m_codeEditor = new CodeEditor(m_mainHorizontalSplitter);
    m_previewPane = new PreviewPane(m_mainHorizontalSplitter);

    m_mainHorizontalSplitter->addWidget(m_codeEditor);
    m_mainHorizontalSplitter->addWidget(m_previewPane);
    m_mainHorizontalSplitter->setStretchFactor(0, 1);
    m_mainHorizontalSplitter->setStretchFactor(1, 1);

    m_consolePane = new ConsolePane(m_verticalSplitter);

    m_verticalSplitter->addWidget(m_mainHorizontalSplitter);
    m_verticalSplitter->addWidget(m_consolePane);
    m_verticalSplitter->setStretchFactor(0, 4);
    m_verticalSplitter->setStretchFactor(1, 1);

    setCentralWidget(m_verticalSplitter);
}

void MainWindow::createActions()
{
}

void MainWindow::createMenus()
{
    QMenuBar *mb = menuBar();

    // File Menu
    QMenu *fileMenu = mb->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New"), QKeySequence::New, this, &MainWindow::newFile);
    fileMenu->addAction(tr("&Open..."), QKeySequence::Open, this, &MainWindow::openFile);
    fileMenu->addAction(tr("&Save"), QKeySequence::Save, this, &MainWindow::saveFile);
    fileMenu->addAction(tr("Save &As..."), QKeySequence::SaveAs, this, &MainWindow::saveFileAs);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Export Preview as PNG..."), this, &MainWindow::exportPreviewPng);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), QKeySequence::Quit, this, &QWidget::close);

    // Edit Menu
    QMenu *editMenu = mb->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Undo"), QKeySequence::Undo, m_codeEditor, &QPlainTextEdit::undo);
    editMenu->addAction(tr("&Redo"), QKeySequence::Redo, m_codeEditor, &QPlainTextEdit::redo);
    editMenu->addSeparator();
    editMenu->addAction(tr("Cu&t"), QKeySequence::Cut, m_codeEditor, &QPlainTextEdit::cut);
    editMenu->addAction(tr("&Copy"), QKeySequence::Copy, m_codeEditor, &QPlainTextEdit::copy);
    editMenu->addAction(tr("&Paste"), QKeySequence::Paste, m_codeEditor, &QPlainTextEdit::paste);
    editMenu->addSeparator();
    editMenu->addAction(tr("Reset Font Size (Ctrl+0)"), this, [this]() { m_codeEditor->resetFontSize(); });

    // View Menu
    QMenu *viewMenu = mb->addMenu(tr("&View"));
    m_themeAction = viewMenu->addAction(tr("Dark Mode"), this, &MainWindow::toggleTheme);
    m_themeAction->setCheckable(true);
    m_themeAction->setChecked(m_isDarkMode);

    viewMenu->addAction(tr("Set Preview Canvas Color..."), this, &MainWindow::pickBackgroundColor);

    // Run Menu
    QMenu *runMenu = mb->addMenu(tr("&Run"));
    QAction *runAction = runMenu->addAction(tr("&Run QML"), this, &MainWindow::runQml);
    runAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));

    QAction *f5RunAction = new QAction(this);
    f5RunAction->setShortcut(QKeySequence(Qt::Key_F5));
    connect(f5RunAction, &QAction::triggered, this, &MainWindow::runQml);
    addAction(f5RunAction);

    runMenu->addAction(tr("&Stop / Clear Preview"), this, &MainWindow::stopClearPreview);
    runMenu->addSeparator();

    m_autoRunAction = runMenu->addAction(tr("Auto-Run (Live Reload)"), this, &MainWindow::onAutoRunToggled);
    m_autoRunAction->setCheckable(true);
    m_autoRunAction->setChecked(true);

    runMenu->addAction(tr("Run qmllint Validator"), this, &MainWindow::runQmllint);

    // Help Menu
    QMenu *helpMenu = mb->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About QmlForge"), this, &MainWindow::showAboutDialog);
}

void MainWindow::createToolbar()
{
    QToolBar *tb = addToolBar(tr("Main Toolbar"));
    tb->setMovable(false);
    tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    tb->addAction(tr("New"), this, &MainWindow::newFile);
    tb->addAction(tr("Open"), this, &MainWindow::openFile);
    tb->addAction(tr("Save"), this, &MainWindow::saveFile);
    tb->addSeparator();

    QAction *runAct = tb->addAction(tr("Run (Ctrl+R / F5)"), this, &MainWindow::runQml);
    runAct->setToolTip(tr("Execute current QML script in live preview"));

    tb->addAction(tr("Stop/Clear"), this, &MainWindow::stopClearPreview);
    tb->addSeparator();

    tb->addWidget(new QLabel(tr("  Examples: "), this));
    m_examplesComboBox = new QComboBox(this);
    m_examplesComboBox->setMinimumWidth(180);
    m_examplesComboBox->addItem(tr("-- Load Example --"));
    m_examplesComboBox->addItem(tr("1. Rectangle & Colors"));
    m_examplesComboBox->addItem(tr("2. Animation"));
    m_examplesComboBox->addItem(tr("3. ListView"));
    m_examplesComboBox->addItem(tr("4. Grid Layout"));
    m_examplesComboBox->addItem(tr("5. Button & Signals"));
    m_examplesComboBox->addItem(tr("6. Timer & States"));

    connect(m_examplesComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onExampleSelected);
    tb->addWidget(m_examplesComboBox);

    tb->addSeparator();
    QAction *autoRunTb = tb->addAction(tr("Auto-Run"), m_autoRunAction, &QAction::setChecked);
    autoRunTb->setCheckable(true);

    QAction *themeTb = tb->addAction(tr("Dark Theme"), m_themeAction, &QAction::setChecked);
    themeTb->setCheckable(true);
    themeTb->setChecked(m_isDarkMode);
}

void MainWindow::createStatusBar()
{
    QStatusBar *sb = statusBar();

    m_statusFileLabel = new QLabel(tr("Untitled.qml"), this);
    m_statusCursorLabel = new QLabel(tr("Line: 1, Col: 1"), this);
    m_statusResultLabel = new QLabel(tr("Ready"), this);

    m_statusFileLabel->setMinimumWidth(180);
    m_statusCursorLabel->setMinimumWidth(130);
    m_statusResultLabel->setStyleSheet("font-weight: bold; padding: 0 8px;");

    sb->addWidget(m_statusFileLabel);
    sb->addWidget(m_statusCursorLabel);
    sb->addPermanentWidget(m_statusResultLabel);
}

void MainWindow::onEditorTextChanged()
{
    if (!m_isModified) {
        m_isModified = true;
        setCurrentFile(m_currentFilePath);
    }

    if (m_autoRunAction && m_autoRunAction->isChecked()) {
        m_autoRunTimer->start();
    }
}

void MainWindow::onAutoRunToggled(bool checked)
{
    if (checked) {
        runQml();
    } else {
        m_autoRunTimer->stop();
    }
}

void MainWindow::runQml()
{
    QString code = m_codeEditor->toPlainText();
    QUrl baseUrl;

    if (!m_currentFilePath.isEmpty()) {
        baseUrl = QUrl::fromLocalFile(m_currentFilePath);
    } else {
        baseUrl = QUrl::fromLocalFile(QDir::currentPath() + "/");
    }

    m_previewPane->loadQmlCode(code, baseUrl);

    QList<QQmlError> errors = m_previewPane->errors();
    if (errors.isEmpty() && m_previewPane->status() == QQuickWidget::Ready) {
        m_consolePane->showSuccessStatus();
        m_statusResultLabel->setText(tr("Run OK"));
        m_statusResultLabel->setStyleSheet("color: #40c057; font-weight: bold;");
    } else if (!errors.isEmpty()) {
        m_consolePane->showQmlErrors(errors);
        m_statusResultLabel->setText(tr("%1 Error(s)").arg(errors.size()));
        m_statusResultLabel->setStyleSheet("color: #f38ba8; font-weight: bold;");
    } else {
        m_statusResultLabel->setText(tr("Status: Loading..."));
        m_statusResultLabel->setStyleSheet("color: #89b4fa; font-weight: bold;");
    }
}

void MainWindow::stopClearPreview()
{
    m_previewPane->clearPreview();
    m_consolePane->clear();
    m_statusResultLabel->setText(tr("Stopped"));
    m_statusResultLabel->setStyleSheet("color: #6c7086; font-weight: bold;");
}

void MainWindow::jumpToEditorLineCol(int line, int col)
{
    if (line <= 0) return;
    QTextDocument *doc = m_codeEditor->document();
    if (line > doc->blockCount()) line = doc->blockCount();

    QTextBlock block = doc->findBlockByLineNumber(line - 1);
    int pos = block.position() + qMax(0, col - 1);

    QTextCursor cursor = m_codeEditor->textCursor();
    cursor.setPosition(pos);
    m_codeEditor->setTextCursor(cursor);
    m_codeEditor->setFocus();
}

void MainWindow::updateStatusBarCursor(int line, int col)
{
    m_statusCursorLabel->setText(tr("Line: %1, Col: %2").arg(line).arg(col));
}

void MainWindow::onExampleSelected(int index)
{
    if (index <= 0) return;

    if (!maybeSave()) {
        m_examplesComboBox->setCurrentIndex(0);
        return;
    }

    QString resPath;
    switch (index) {
    case 1: resPath = ":/examples/rectangle.qml"; break;
    case 2: resPath = ":/examples/animation.qml"; break;
    case 3: resPath = ":/examples/listview.qml"; break;
    case 4: resPath = ":/examples/grid.qml"; break;
    case 5: resPath = ":/examples/button.qml"; break;
    case 6: resPath = ":/examples/timer_state.qml"; break;
    }

    QFile file(resPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_codeEditor->setPlainText(QString::fromUtf8(file.readAll()));
        m_isModified = false;
        setCurrentFile("");
        runQml();
    }

    m_examplesComboBox->setCurrentIndex(0);
}

void MainWindow::newFile()
{
    if (!maybeSave()) return;
    m_codeEditor->clear();
    setCurrentFile("");
    m_isModified = false;
    stopClearPreview();
}

void MainWindow::openFile()
{
    if (!maybeSave()) return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open QML File"), QString(), tr("QML Files (*.qml);;All Files (*)"));
    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

bool MainWindow::saveFile()
{
    if (m_currentFilePath.isEmpty()) {
        return saveFileAs();
    }

    QFile file(m_currentFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Save Error"), tr("Cannot write file %1:\n%2").arg(m_currentFilePath, file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << m_codeEditor->toPlainText();
    m_isModified = false;
    setCurrentFile(m_currentFilePath);
    statusBar()->showMessage(tr("File saved"), 3000);
    return true;
}

bool MainWindow::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save QML File As"), QString(), tr("QML Files (*.qml);;All Files (*)"));
    if (fileName.isEmpty()) return false;

    m_currentFilePath = fileName;
    return saveFile();
}

void MainWindow::loadFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Open Error"), tr("Cannot read file %1:\n%2").arg(filePath, file.errorString()));
        return;
    }

    m_codeEditor->setPlainText(QString::fromUtf8(file.readAll()));
    setCurrentFile(filePath);
    m_isModified = false;
    runQml();
}

void MainWindow::setCurrentFile(const QString &filePath)
{
    m_currentFilePath = filePath;
    QString shownName = filePath.isEmpty() ? tr("Untitled.qml") : QFileInfo(filePath).fileName();
    if (m_isModified) shownName += "*";

    m_statusFileLabel->setText(shownName);
    setWindowTitle(tr("%1 - QmlForge v%2").arg(shownName, QMLFORGE_VERSION));
}

bool MainWindow::maybeSave()
{
    if (!m_isModified) return true;

    QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Unsaved Changes"),
        tr("The document has been modified.\nDo you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
        return saveFile();
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

void MainWindow::toggleTheme(bool dark)
{
    m_isDarkMode = dark;
    if (m_themeAction) m_themeAction->setChecked(dark);
    applyTheme(dark);
    if (m_highlighter) m_highlighter->setDarkMode(dark);
}

void MainWindow::applyTheme(bool dark)
{
    if (dark) {
        qApp->setStyle("Fusion");
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor("#1e1e2e"));
        darkPalette.setColor(QPalette::WindowText, QColor("#cdd6f4"));
        darkPalette.setColor(QPalette::Base, QColor("#181825"));
        darkPalette.setColor(QPalette::AlternateBase, QColor("#313244"));
        darkPalette.setColor(QPalette::ToolTipBase, QColor("#cdd6f4"));
        darkPalette.setColor(QPalette::ToolTipText, QColor("#cdd6f4"));
        darkPalette.setColor(QPalette::Text, QColor("#cdd6f4"));
        darkPalette.setColor(QPalette::Button, QColor("#313244"));
        darkPalette.setColor(QPalette::ButtonText, QColor("#cdd6f4"));
        darkPalette.setColor(QPalette::Highlight, QColor("#89b4fa"));
        darkPalette.setColor(QPalette::HighlightedText, QColor("#11111b"));
        qApp->setPalette(darkPalette);
        m_previewPane->setBackgroundColor(QColor("#181825"));
    } else {
        qApp->setStyle("Fusion");
        QPalette lightPalette;
        lightPalette.setColor(QPalette::Window, QColor("#eff1f5"));
        lightPalette.setColor(QPalette::WindowText, QColor("#4c4f69"));
        lightPalette.setColor(QPalette::Base, QColor("#ffffff"));
        lightPalette.setColor(QPalette::AlternateBase, QColor("#e6e9ef"));
        lightPalette.setColor(QPalette::ToolTipBase, QColor("#4c4f69"));
        lightPalette.setColor(QPalette::ToolTipText, QColor("#4c4f69"));
        lightPalette.setColor(QPalette::Text, QColor("#4c4f69"));
        lightPalette.setColor(QPalette::Button, QColor("#e6e9ef"));
        lightPalette.setColor(QPalette::ButtonText, QColor("#4c4f69"));
        lightPalette.setColor(QPalette::Highlight, QColor("#1e66f5"));
        lightPalette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
        qApp->setPalette(lightPalette);
        m_previewPane->setBackgroundColor(QColor("#ffffff"));
    }
}

void MainWindow::pickBackgroundColor()
{
    QColor c = QColorDialog::getColor(m_previewPane->backgroundColor(), this, tr("Select Preview Canvas Background"));
    if (c.isValid()) {
        m_previewPane->setBackgroundColor(c);
    }
}

void MainWindow::runQmllint()
{
    QString qmllintPath = QCoreApplication::applicationDirPath() + "/qmllint.exe";
    if (!QFile::exists(qmllintPath)) {
        qmllintPath = "C:/Qt/6.8.3/mingw_64/bin/qmllint.exe";
    }
    if (!QFile::exists(qmllintPath)) {
        qmllintPath = "qmllint";
    }

    // Save temp qml file to run qmllint
    QString tempPath = QDir::tempPath() + "/qmlforge_temp.qml";
    QFile tempFile(tempPath);
    if (tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&tempFile);
        out << m_codeEditor->toPlainText();
        tempFile.close();
    }

    QProcess process;
    process.start(qmllintPath, QStringList() << tempPath);
    if (!process.waitForFinished(4000)) {
        m_consolePane->appendLog("WARN", tr("qmllint executable not found or timed out: %1").arg(qmllintPath));
        return;
    }

    QString output = QString::fromUtf8(process.readAllStandardOutput() + process.readAllStandardError());
    if (output.trimmed().isEmpty()) {
        m_consolePane->appendLog("INFO", tr("qmllint check passed: No warnings found."));
    } else {
        m_consolePane->appendLog("WARN", tr("qmllint output:\n") + output);
    }
}

void MainWindow::exportPreviewPng()
{
    QImage img = m_previewPane->exportToImage();
    if (img.isNull()) {
        QMessageBox::warning(this, tr("Export Failed"), tr("Could not capture preview image. Ensure QML is running."));
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Preview Image"), "preview.png", tr("PNG Images (*.png)"));
    if (!fileName.isEmpty()) {
        if (img.save(fileName, "PNG")) {
            statusBar()->showMessage(tr("Exported preview to %1").arg(fileName), 4000);
        } else {
            QMessageBox::warning(this, tr("Save Error"), tr("Failed to save image file."));
        }
    }
}

void MainWindow::showAboutDialog()
{
    QString title = tr("About QmlForge");
    QString text = tr("<h3>QmlForge v%1</h3>"
                      "<p>A modern QML Live Editor and Runner playground.</p>"
                      "<p><b>Qt Version:</b> %2</p>"
                      "<p><b>Compiler:</b> C++17</p>"
                      "<p><b>License:</b> MIT License</p>"
                      "<p>Built with Qt Quick, Qt Widgets, and CMake.</p>")
                      .arg(QMLFORGE_VERSION, QT_VERSION_STR);

    QMessageBox::about(this, title, text);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        saveSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        const QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty() && urls.first().toLocalFile().endsWith(".qml", Qt::CaseInsensitive)) {
            event->acceptProposedAction();
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (!urls.isEmpty()) {
        QString localFile = urls.first().toLocalFile();
        if (maybeSave()) {
            loadFile(localFile);
        }
    }
}

void MainWindow::loadSettings()
{
    QSettings settings("QmlForge", "QmlForge");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    if (settings.contains("horizontalSplitter")) {
        m_mainHorizontalSplitter->restoreState(settings.value("horizontalSplitter").toByteArray());
    }
    if (settings.contains("verticalSplitter")) {
        m_verticalSplitter->restoreState(settings.value("verticalSplitter").toByteArray());
    }

    bool dark = settings.value("isDarkMode", true).toBool();
    toggleTheme(dark);

    QString lastFile = settings.value("lastFile").toString();
    if (!lastFile.isEmpty() && QFile::exists(lastFile)) {
        loadFile(lastFile);
    }
}

void MainWindow::saveSettings()
{
    QSettings settings("QmlForge", "QmlForge");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("horizontalSplitter", m_mainHorizontalSplitter->saveState());
    settings.setValue("verticalSplitter", m_verticalSplitter->saveState());
    settings.setValue("isDarkMode", m_isDarkMode);
    settings.setValue("lastFile", m_currentFilePath);
}
