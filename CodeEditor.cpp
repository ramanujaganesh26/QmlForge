#include "CodeEditor.h"
#include <QPainter>
#include <QTextBlock>
#include <QFontDatabase>
#include <QTextCharFormat>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QApplication>

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    m_lineNumberArea = new LineNumberArea(this);

    // Monospace font setup
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setFamily("Consolas");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(m_defaultFontSize);
    setFont(font);

    // Tab stop distance (4 spaces)
    QFontMetrics metrics(font);
    setTabStopDistance(4 * metrics.horizontalAdvance(' '));

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLineAndBrackets);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::emitCursorPosition);

    updateLineNumberAreaWidth(0);
    highlightCurrentLineAndBrackets();
    emitCursorPosition();
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        digits++;
    }

    int space = 15 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) {
        m_lineNumberArea->scroll(0, dy);
    } else {
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::resetFontSize()
{
    QFont f = font();
    f.setPointSize(m_defaultFontSize);
    setFont(f);
    QFontMetrics metrics(f);
    setTabStopDistance(4 * metrics.horizontalAdvance(' '));
    updateLineNumberAreaWidth(0);
}

void CodeEditor::emitCursorPosition()
{
    QTextCursor cursor = textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    emit cursorPositionInfoChanged(line, col);
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    // Tab key -> 4 spaces
    if (event->key() == Qt::Key_Tab && event->modifiers() == Qt::NoModifier) {
        insertPlainText("    ");
        return;
    }

    // Shift + Tab -> Backtab (unindent 4 spaces if present)
    if (event->key() == Qt::Key_Backtab || (event->key() == Qt::Key_Tab && event->modifiers() == Qt::ShiftModifier)) {
        QTextCursor cursor = textCursor();
        int col = cursor.columnNumber();
        if (col >= 4) {
            cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 4);
            if (cursor.selectedText() == "    ") {
                cursor.removeSelectedText();
                return;
            }
        }
    }

    // Auto-indent on Enter
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QTextBlock currentBlock = textCursor().block();
        QString text = currentBlock.text();
        int indent = 0;
        for (int i = 0; i < text.length(); ++i) {
            if (text[i] == ' ') indent++;
            else if (text[i] == '\t') indent += 4;
            else break;
        }

        QString trimmed = text.trimmed();
        if (trimmed.endsWith('{') || trimmed.endsWith('[') || trimmed.endsWith('(')) {
            indent += 4;
        }

        QPlainTextEdit::keyPressEvent(event);
        insertPlainText(QString(indent, ' '));
        return;
    }

    // Ctrl + 0 -> Reset font size
    if (event->key() == Qt::Key_0 && (event->modifiers() & Qt::ControlModifier)) {
        resetFontSize();
        return;
    }

    QPlainTextEdit::keyPressEvent(event);
}

void CodeEditor::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        int delta = event->angleDelta().y();
        QFont f = font();
        int pointSize = f.pointSize();
        if (delta > 0) {
            pointSize = qMin(pointSize + 1, 32);
        } else if (delta < 0) {
            pointSize = qMax(pointSize - 1, 6);
        }
        f.setPointSize(pointSize);
        setFont(f);
        QFontMetrics metrics(f);
        setTabStopDistance(4 * metrics.horizontalAdvance(' '));
        updateLineNumberAreaWidth(0);
        event->accept();
        return;
    }
    QPlainTextEdit::wheelEvent(event);
}

void CodeEditor::highlightCurrentLineAndBrackets()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = palette().color(QPalette::Base).darker(108);
        if (lineColor == palette().color(QPalette::Base)) {
            lineColor = palette().color(QPalette::Base).lighter(108);
        }
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    // Bracket matching logic
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
    QString docText = toPlainText();

    auto matchPair = [&](int openPos, int closePos) {
        QTextEdit::ExtraSelection sel1, sel2;
        QTextCharFormat fmt;
        fmt.setBackground(QColor(100, 200, 255, 100));
        fmt.setFontWeight(QFont::Bold);

        sel1.format = fmt;
        sel1.cursor = textCursor();
        sel1.cursor.setPosition(openPos);
        sel1.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        extraSelections.append(sel1);

        sel2.format = fmt;
        sel2.cursor = textCursor();
        sel2.cursor.setPosition(closePos);
        sel2.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        extraSelections.append(sel2);
    };

    auto findMatch = [&](int currentPos, QChar target, QChar match, bool forward) {
        int depth = 1;
        int step = forward ? 1 : -1;
        int i = currentPos + step;
        while (i >= 0 && i < docText.length()) {
            if (docText[i] == target) depth++;
            else if (docText[i] == match) {
                depth--;
                if (depth == 0) return i;
            }
            i += step;
        }
        return -1;
    };

    if (pos < docText.length()) {
        QChar ch = docText[pos];
        if (ch == '{') { int m = findMatch(pos, '{', '}', true); if (m != -1) matchPair(pos, m); }
        else if (ch == '(') { int m = findMatch(pos, '(', ')', true); if (m != -1) matchPair(pos, m); }
        else if (ch == '[') { int m = findMatch(pos, '[', ']', true); if (m != -1) matchPair(pos, m); }
        else if (ch == '}') { int m = findMatch(pos, '}', '{', false); if (m != -1) matchPair(m, pos); }
        else if (ch == ')') { int m = findMatch(pos, ')', '(', false); if (m != -1) matchPair(m, pos); }
        else if (ch == ']') { int m = findMatch(pos, ']', '[', false); if (m != -1) matchPair(m, pos); }
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    QColor bgColor = palette().color(QPalette::Window);
    QColor textColor = palette().color(QPalette::WindowText);
    textColor.setAlpha(150);

    painter.fillRect(event->rect(), bgColor);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(textColor);

            if (blockNumber == textCursor().blockNumber()) {
                QFont f = painter.font();
                f.setBold(true);
                painter.setFont(f);
                painter.setPen(palette().color(QPalette::Highlight));
            }

            painter.drawText(0, top, m_lineNumberArea->width() - 5, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
