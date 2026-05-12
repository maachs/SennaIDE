#include "main_window.hpp"
#include "code_editor.hpp"
#include <QTreeWidget>
#include <QTextEdit>
#include <QSplitter>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QFile>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_codeEditor(new CodeEditor(this)),
      m_projectTree(new QTreeWidget(this)),
      m_outputLog(new QTextEdit(this)),
      m_compilerProcess(new QProcess(this))
{
    Q_ASSERT(m_codeEditor != nullptr);
    Q_ASSERT(m_outputLog != nullptr);
    Q_ASSERT(m_compilerProcess != nullptr);

    setupUI();

    connect(m_compilerProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::handleCompilerFinished);

    connect(m_compilerProcess, &QProcess::errorOccurred,
            this, &MainWindow::handleCompilerError);
}

MainWindow::~MainWindow() {
    if (m_compilerProcess->state() == QProcess::Running) {
        m_compilerProcess->kill();
        m_compilerProcess->waitForFinished();
    }
}

void MainWindow::setupUI() {
    m_outputLog->setReadOnly(true);
    m_projectTree->setHeaderLabel("Project Explorer");

    QSplitter* hSplitter = new QSplitter(Qt::Horizontal, this);

    QSplitter* vSplitter = new QSplitter(Qt::Vertical, hSplitter);

    hSplitter->addWidget(m_projectTree);

    vSplitter->addWidget(m_codeEditor);

    vSplitter->addWidget(m_outputLog);

    setCentralWidget(hSplitter);

    hSplitter->setSizes({200, 800});
    vSplitter->setSizes({600, 200});

    resize(1024, 768);

    setupToolBar();
}

void MainWindow::setupToolBar() {

    QToolBar* toolbar = addToolBar("Main Toolbar");

    QAction* runAction = new QAction("Run Senna", this);

    connect(runAction, &QAction::triggered, this, &MainWindow::handleRunCompiler);

    toolbar->addAction(runAction);
}

void MainWindow::handleRunCompiler() {

    m_outputLog->clear();
    m_outputLog->append("Starting SennaC compilation...");

    QString tempFileName = "temp_source.sn";
    QFile file(tempFileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_outputLog->append("[-] OS Error: Cannot create temporary file.");
        return;
    }

    file.write(m_codeEditor->toPlainText().toUtf8());
    file.close();

    QStringList arguments;
    arguments << tempFileName << "--emit=ast";

    m_compilerProcess->start("./senna", arguments);
}

void MainWindow::handleCompilerFinished(int exitCode) {
    QString stdOut = m_compilerProcess->readAllStandardOutput();

    QString stdErr = m_compilerProcess->readAllStandardError();

    if (!stdOut.isEmpty()) {
        m_outputLog->append("Output:\n" + stdOut);
    }

    if (!stdErr.isEmpty()) {
        m_outputLog->append("Errors:\n" + stdErr);
    }

    if (exitCode == 0) {
        m_outputLog->append("[+] Finished successfully.");
    } else {
        m_outputLog->append("[-] Finished with code " + QString::number(exitCode));
    }
}

void MainWindow::handleCompilerError(QProcess::ProcessError error) {
    switch (error) {
        case QProcess::FailedToStart:
            m_outputLog->append("[-] OS Error: Process failed to start. Is 'senna' compiled and in the right directory?");
            break;
        case QProcess::Crashed:
            m_outputLog->append("[-] OS Error: Compiler process crashed unexpectedly.");
            break;
        default:
            m_outputLog->append("[-] OS Error: An unknown error occurred with the system call.");
            break;
    }
}
