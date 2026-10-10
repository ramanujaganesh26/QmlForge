#ifndef QMLHIGHLIGHTER_H
#define QMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

class QmlHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit QmlHighlighter(QTextDocument *parent = nullptr);

    void setDarkMode(bool dark);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    void setupRules();

    QVector<HighlightingRule> m_highlightingRules;

    QRegularExpression m_commentStartExpression;
    QRegularExpression m_commentEndExpression;

    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_typeFormat;
    QTextCharFormat m_propertyFormat;
    QTextCharFormat m_numberFormat;
    QTextCharFormat m_stringFormat;
    QTextCharFormat m_commentFormat;

    bool m_isDarkMode{true};
};

#endif // QMLHIGHLIGHTER_H
