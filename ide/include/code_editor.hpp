#pragma once

#include <QPlainTextEdit>
#include <QCompleter>

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor() override = default;

    void setCompleter(QCompleter *completer);
    QCompleter* completer() const { return m_completer; }

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private slots:
    void insertCompletion(const QString &completion);

private:
    QCompleter *m_completer = nullptr;

    QString textUnderCursor() const;
    void setupEditor();
};
