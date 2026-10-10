#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QComboBox>
#include <QTimer>
#include <QLabel>
#include <QSettings>
#include <QAction>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "CodeEditor.h"
#include "PreviewPane.h"
#include "ConsolePane.h"
#include "QmlHighlighter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    static MainWindow *instance() { return s_instance; }
    void logQtMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    void runQml();
    void stopClearPreview();
    void onExampleSelected(int index);
    void onEditorTextChanged();
    void onAutoRunToggled(bool checked);
    void toggleTheme(bool dark);
    void pickBackgroundColor();
    void runQmllint();
    void exportPreviewPng();
    void showAboutDialog();
    void jumpToEditorLineCol(int line, int col);
    void updateStatusBarCursor(int line, int col);

private:
    void setupUi();
    void createActions();
    void createMenus();
    void createToolbar();
    void createStatusBar();
    void loadSettings();
    void saveSettings();
    bool maybeSave();
    void loadFile(const QString &filePath);
    void setCurrentFile(const QString &filePath);
    void applyTheme(bool dark);

    static MainWindow *s_instance;

    // UI Widgets
    QSplitter *m_mainHorizontalSplitter{nullptr};
    QSplitter *m_verticalSplitter{nullptr};
    CodeEditor *m_codeEditor{nullptr};
    PreviewPane *m_previewPane{nullptr};
    ConsolePane *m_consolePane{nullptr};
    QmlHighlighter *m_highlighter{nullptr};

    // Controls
    QComboBox *m_examplesComboBox{nullptr};
    QAction *m_autoRunAction{nullptr};
    QAction *m_themeAction{nullptr};
    QTimer *m_autoRunTimer{nullptr};

    // Status bar labels
    QLabel *m_statusFileLabel{nullptr};
    QLabel *m_statusCursorLabel{nullptr};
    QLabel *m_statusResultLabel{nullptr};

    // File state
    QString m_currentFilePath;
    bool m_isModified{false};
    bool m_isDarkMode{true};
};

#endif // MAINWINDOW_H
