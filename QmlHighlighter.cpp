#include "QmlHighlighter.h"

QmlHighlighter::QmlHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    setupRules();
}

void QmlHighlighter::setDarkMode(bool dark)
{
    m_isDarkMode = dark;
    setupRules();
    rehighlight();
}

void QmlHighlighter::setupRules()
{
    m_highlightingRules.clear();

    if (m_isDarkMode) {
        // Dark theme color palette (Catppuccin Mocha inspired)
        m_keywordFormat.setForeground(QColor("#cba6f7")); // Purple/Mauve
        m_keywordFormat.setFontWeight(QFont::Bold);

        m_typeFormat.setForeground(QColor("#f9e2af"));    // Yellow
        m_typeFormat.setFontWeight(QFont::Bold);

        m_propertyFormat.setForeground(QColor("#89b4fa"));// Blue

        m_numberFormat.setForeground(QColor("#fab387"));  // Peach/Orange

        m_stringFormat.setForeground(QColor("#a6e3a1"));  // Green

        m_commentFormat.setForeground(QColor("#6c7086")); // Muted Gray
        m_commentFormat.setFontItalic(true);
    } else {
        // Light theme color palette
        m_keywordFormat.setForeground(QColor("#8839ef"));
        m_keywordFormat.setFontWeight(QFont::Bold);

        m_typeFormat.setForeground(QColor("#df8e1d"));
        m_typeFormat.setFontWeight(QFont::Bold);

        m_propertyFormat.setForeground(QColor("#1e66f5"));

        m_numberFormat.setForeground(QColor("#fe640b"));

        m_stringFormat.setForeground(QColor("#40a02b"));

        m_commentFormat.setForeground(QColor("#9ca0b0"));
        m_commentFormat.setFontItalic(true);
    }

    // 1. QML & JS Keywords
    QStringList keywordPatterns = {
        "\\bimport\\b", "\\bproperty\\b", "\\bsignal\\b", "\\bfunction\\b",
        "\\bid\\b", "\\balias\\b", "\\bas\\b", "\\breadonly\\b", "\\brequired\\b",
        "\\bcomponent\\b", "\\benum\\b", "\\bvar\\b", "\\bint\\b", "\\bdouble\\b",
        "\\bstring\\b", "\\bbool\\b", "\\breal\\b", "\\burl\\b", "\\bcolor\\b",
        "\\brect\\b", "\\bpoint\\b", "\\bsize\\b", "\\bvector2d\\b", "\\bvector3d\\b",
        "\\bif\\b", "\\belse\\b", "\\bfor\\b", "\\bwhile\\b", "\\bdo\\b",
        "\\breturn\\b", "\\bbreak\\b", "\\bcontinue\\b", "\\bswitch\\b",
        "\\bcase\\b", "\\bdefault\\b", "\\btry\\b", "\\bcatch\\b", "\\bfinally\\b",
        "\\bthrow\\b", "\\bnew\\b", "\\bdelete\\b", "\\btypeof\\b", "\\binstanceof\\b",
        "\\btrue\\b", "\\bfalse\\b", "\\bnull\\b", "\\bundefined\\b", "\\bon[A-Z][a-zA-Z0-9_]*\\b"
    };

    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = m_keywordFormat;
        m_highlightingRules.append(rule);
    }

    // 2. QML Types (Capitalized identifiers)
    HighlightingRule typeRule;
    typeRule.pattern = QRegularExpression("\\b[A-Z][a-zA-Z0-9_]*\\b");
    typeRule.format = m_typeFormat;
    m_highlightingRules.append(typeRule);

    // 3. Properties before colon (e.g. anchors.centerIn:, width:, color:)
    HighlightingRule propertyRule;
    propertyRule.pattern = QRegularExpression("\\b[a-z_][a-zA-Z0-9_.]*(?=\\s*:)");
    propertyRule.format = m_propertyFormat;
    m_highlightingRules.append(propertyRule);

    // 4. Numbers (hex, float, int)
    HighlightingRule numberRule;
    numberRule.pattern = QRegularExpression("\\b(0x[0-9a-fA-F]+|\\d+(\\.\\d+)?)\\b");
    numberRule.format = m_numberFormat;
    m_highlightingRules.append(numberRule);

    // 5. Strings ("..." and '...')
    HighlightingRule doubleQuoteRule;
    doubleQuoteRule.pattern = QRegularExpression("\".*?\"");
    doubleQuoteRule.format = m_stringFormat;
    m_highlightingRules.append(doubleQuoteRule);

    HighlightingRule singleQuoteRule;
    singleQuoteRule.pattern = QRegularExpression("'.*?'");
    singleQuoteRule.format = m_stringFormat;
    m_highlightingRules.append(singleQuoteRule);

    // 6. Single line comments //
    HighlightingRule singleCommentRule;
    singleCommentRule.pattern = QRegularExpression("//[^\n]*");
    singleCommentRule.format = m_commentFormat;
    m_highlightingRules.append(singleCommentRule);

    // 7. Multiline comments /* */
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}

void QmlHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : m_highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = text.indexOf(m_commentStartExpression);
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch match = m_commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, m_commentFormat);
        startIndex = text.indexOf(m_commentStartExpression, startIndex + commentLength);
    }
}
