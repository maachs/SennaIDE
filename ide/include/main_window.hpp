#pragma once

#include <QMainWindow>
#include <QProcess>

class CodeEditor;
class QTreeWidget;
class QTextEdit;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private slots:

    void handleRunCompiler();

    void handleCompilerFinished(int exitCode);

    void handleCompilerError(QProcess::ProcessError error);

private:
    CodeEditor* m_codeEditor;

    QTreeWidget* m_projectTree;

    QTextEdit* m_outputLog;

    QProcess* m_compilerProcess;

    void setupUI();

    void setupToolBar();
};
