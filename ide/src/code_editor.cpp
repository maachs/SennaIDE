#include "code_editor.hpp"
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QtGlobal>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
    Q_ASSERT(this != nullptr);
    setupEditor();
}

void CodeEditor::setCompleter(QCompleter *completer) {
    if (m_completer)
        m_completer->disconnect(this);

    m_completer = completer;

    if (!m_completer)
        return;

    m_completer->setWidget(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseSensitive);

    if (m_completer->popup()) {
        m_completer->popup()->setFont(this->font());
        m_completer->popup()->setStyleSheet(
            "QAbstractItemView { "
            "  background-color: #252526; "
            "  color: #cccccc; "
            "  border: 1px solid #454545; "
            "  selection-background-color: #37373d; "
            "  font-size: 14px; "
            "  outline: none; "
            "}"
            "QAbstractItemView::item { "
            "  padding: 6px 12px; "
            "}"
            "QAbstractItemView::item:selected { "
            "  background-color: #37373d; "
            "  color: #ffffff; "
            "}"
        );
    }

    connect(m_completer, QOverload<const QString &>::of(&QCompleter::activated),
            this, &CodeEditor::insertCompletion);
}

void CodeEditor::insertCompletion(const QString &completion) {
    if (m_completer->widget() != this) return;

    QTextCursor tc = textCursor();
    int extra = completion.length() - m_completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString CodeEditor::textUnderCursor() const {
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeEditor::keyPressEvent(QKeyEvent *e) {
    if (m_completer && m_completer->popup()->isVisible()) {
        switch (e->key()) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                e->ignore();
                return;
            default: break;
        }
    }

    const bool isShortcut = (e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space;
    if (!isShortcut)
        QPlainTextEdit::keyPressEvent(e);

    if (!m_completer) return;

    QString completionPrefix = textUnderCursor();
    if (!isShortcut && (e->text().isEmpty() || completionPrefix.length() < 2)) {
        m_completer->popup()->hide();
        return;
    }

    if (completionPrefix != m_completer->completionPrefix()) {
        m_completer->setCompletionPrefix(completionPrefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }

    QRect cr = cursorRect();

    int popupWidth = m_completer->popup()->sizeHintForColumn(0) +
                     m_completer->popup()->verticalScrollBar()->sizeHint().width();

    cr.setWidth(qMax(popupWidth, 250));
    m_completer->complete(cr);
}

void CodeEditor::focusInEvent(QFocusEvent *e) {
    if (m_completer) m_completer->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void CodeEditor::setupEditor() {
    QFont font("Monospace", 14);
    this->setFont(font);
    this->setTabStopDistance(QFontMetrics(font).horizontalAdvance(' ') * 4);
}
