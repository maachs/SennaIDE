#pragma once

#include <QPlainTextEdit>

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    ~CodeEditor() override = default;

private:
    void setupEditor();
};
