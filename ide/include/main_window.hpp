#pragma once

#include <QMainWindow>
#include <QProcess>
#include <QFileSystemModel>
#include <QTreeView>
#include <QTabWidget>
#include <QMap>

class CodeEditor;
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

    void onFileDoubleClicked(const QModelIndex &index);
    void showExplorerContextMenu(const QPoint &pos);
    void createNewFile();
    void deleteSelectedFile();

private:
    QTreeView* m_fileExplorer;
    QFileSystemModel* m_fileModel;
    QTabWidget* m_editorTabs;
    QMap<QString, CodeEditor*> m_openEditors;

    QTextEdit* m_outputLog;
    QProcess* m_compilerProcess;

    void setupUI();
    void setupToolBar();
    void applyDarkTheme();
};
