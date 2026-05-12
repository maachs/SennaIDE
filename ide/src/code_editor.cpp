#include "code_editor.hpp"
#include <QFont>
#include <QFontDatabase>
#include <QtGlobal>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
    Q_ASSERT(this != nullptr);

    setupEditor();
}

void CodeEditor::setupEditor() {
    QFont font;

    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setPointSize(10);

    this->setFont(font);

    const int tabStop = 4;

    QFontMetrics metrics(font);

    this->setTabStopDistance(tabStop * metrics.horizontalAdvance(' '));
}
