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
#include <QDir>
#include <QMenu>
#include <QInputDialog>
#include <QHeaderView>
#include <QPushButton>
#include <QTabBar>
#include <QStringListModel>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_fileExplorer(new QTreeView(this)),
      m_fileModel(new QFileSystemModel(this)),
      m_editorTabs(new QTabWidget(this)),
      m_outputLog(new QTextEdit(this)),
      m_compilerProcess(new QProcess(this))
{
    Q_ASSERT(m_outputLog != nullptr);
    Q_ASSERT(m_compilerProcess != nullptr);

    setupUI();
    applyDarkTheme();

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

    QString path = QDir::currentPath();
    m_fileModel->setRootPath(path);
    m_fileExplorer->setModel(m_fileModel);
    m_fileExplorer->setRootIndex(m_fileModel->index(path));

    for (int i = 1; i < 4; ++i) m_fileExplorer->hideColumn(i);
    m_fileExplorer->header()->hide();

    QSplitter* hSplitter = new QSplitter(Qt::Horizontal, this);
    QSplitter* vSplitter = new QSplitter(Qt::Vertical, hSplitter);

    m_editorTabs->setTabsClosable(true);
    m_editorTabs->setMovable(true);
    m_editorTabs->setUsesScrollButtons(true);
    connect(m_editorTabs, &QTabWidget::tabCloseRequested, [this](int index) {
        QWidget* widget = m_editorTabs->widget(index);
        QString filePath = m_openEditors.key(static_cast<CodeEditor*>(widget));
        m_openEditors.remove(filePath);
        m_editorTabs->removeTab(index);
        delete widget;
    });

    hSplitter->addWidget(m_fileExplorer);
    hSplitter->addWidget(vSplitter);

    vSplitter->addWidget(m_editorTabs);
    vSplitter->addWidget(m_outputLog);

    hSplitter->setStretchFactor(1, 1);
    vSplitter->setStretchFactor(0, 1);

    setCentralWidget(hSplitter);
    hSplitter->setSizes({200, 800});

    resize(1200, 800);
    setupToolBar();

    m_fileExplorer->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_fileExplorer, &QTreeView::customContextMenuRequested, this, &MainWindow::showExplorerContextMenu);
    connect(m_fileExplorer, &QTreeView::doubleClicked, this, &MainWindow::onFileDoubleClicked);
}

void MainWindow::setupToolBar() {
    QToolBar* toolbar = addToolBar("Main Toolbar");

    QAction* saveAction = new QAction("Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveCurrentFile);
    toolbar->addAction(saveAction);

    toolbar->addSeparator();

    QAction* runAction = new QAction("Run Senna", this);
    connect(runAction, &QAction::triggered, this, &MainWindow::handleRunCompiler);
    toolbar->addAction(runAction);
}

void MainWindow::handleRunCompiler() {
    CodeEditor* currentEditor = qobject_cast<CodeEditor*>(m_editorTabs->currentWidget());

    if (!currentEditor) {
        m_outputLog->append("[-] No file is open.");
        return;
    }

    m_outputLog->clear();
    m_outputLog->append("Starting full compilation and execution...");

    QString currentPath = m_openEditors.key(currentEditor);
    if (currentPath.isEmpty()) return;

    QFile file(currentPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(currentEditor->toPlainText().toUtf8());
        file.close();
    } else {
        m_outputLog->append("[-] OS Error: Cannot save file.");
        return;
    }

    QString fullCommand = QString(
        "./build/compiler_build/src/senna %1 --emit=llvm && "
        "clang -O0 output/out.ll -o program && "
        "echo '' && ./program"
    ).arg(currentPath);

    m_compilerProcess->start("/bin/sh", QStringList() << "-c" << fullCommand);
}

void MainWindow::handleCompilerFinished(int exitCode) {
    QString stdOut = m_compilerProcess->readAllStandardOutput();
    QString stdErr = m_compilerProcess->readAllStandardError();

    QStringList lines = stdOut.split('\n');

    for (const QString& line : lines) {
        if (line.trimmed().isEmpty()) continue;

        if (line.startsWith("[+]") || line.startsWith("[*]")) {
            m_outputLog->append("<span style='color: #6a9955;'>" + line + "</span>");
        } else {
            m_outputLog->append("<b style='color: #ffffff; font-size: 14px;'>" + line + "</b>");
        }
    }

    if (!stdErr.isEmpty()) {
        m_outputLog->append("<span style='color: #f48771;'>Errors:</span>\n" + stdErr);
    }

    if (exitCode == 0) {
        m_outputLog->append("<span style='color: #6a9955;'>[+] Finished successfully.</span>");
    } else {
        m_outputLog->append("<span style='color: #f48771;'>[-] Finished with code " + QString::number(exitCode) + "</span>");
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

void MainWindow::applyDarkTheme() {
    this->setStyleSheet(
        "QMainWindow { background-color: #1e1e1e; }"

        // Files panel
        "QTreeView { background-color: #252526; color: #cccccc; border: none; font-size: 13px; outline: none; }"
        "QTreeView::item:selected { background-color: #37373d; color: #ffffff; }"
        "QTreeView::item:hover { background-color: #2a2d2e; }"

        // Auto-completion style
        "QAbstractItemView { "
        "  background-color: #252526; "
        "  color: #cccccc; "
        "  border: 1px solid #454545; "
        "  selection-background-color: #37373d; "
        "  outline: none; "
        "}"
        "QAbstractItemView::item { "
        "  padding: 4px 8px; "
        "}"
        "QAbstractItemView::item:selected { "
        "  background-color: #37373d; "
        "  color: #ffffff; "
        "}"

        // Tabs panel
        "QTabWidget::pane { border-top: 1px solid #333333; background-color: #1e1e1e; }"
        "QTabBar::tab { background: #2d2d2d; color: #969696; padding: 8px 12px; border-right: 1px solid #1e1e1e; min-width: 100px; }"
        "QTabBar::tab:selected { background: #1e1e1e; color: #ffffff; }"

        "QPushButton#tabCloseButton { background: none; color: #969696; border: none; font-family: 'Arial'; font-size: 14px; font-weight: bold; padding-bottom: 2px; }"
        "QPushButton#tabCloseButton:hover { color: white; background-color: #454545; border-radius: 2px; }"

        "QToolBar { background-color: #333333; border: none; padding: 5px; spacing: 10px; }"
        "QToolButton { color: white; background-color: #0e639c; border-radius: 3px; padding: 5px 15px; font-weight: bold; }"
        "QToolButton:hover { background-color: #1177bb; }"

        "QPlainTextEdit { background-color: #1e1e1e; color: #d4d4d4; border: none; font-family: 'Monospace'; }"
        "QTextEdit { background-color: #1e1e1e; color: #858585; border-top: 1px solid #333333; }"
        "QSplitter::handle { background-color: #333333; }"
    );
}

void MainWindow::onFileDoubleClicked(const QModelIndex &index) {
    QString filePath = m_fileModel->filePath(index);
    if (QFileInfo(filePath).isDir()) return;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        CodeEditor* editor = new CodeEditor(this);
        editor->setPlainText(file.readAll());
        file.close();

        QStringList keywords = {"print", "if", "else", "while", "func", "return", "for", "break"};
        QCompleter* completer = new QCompleter(keywords, this);
        completer->setCaseSensitivity(Qt::CaseSensitive);
        editor->setCompleter(completer);

        connect(editor, &QPlainTextEdit::textChanged, [editor, keywords]() {
            QStringList dynamicWords = keywords;
            QRegularExpression rx("\\b[a-zA-Z_][a-zA-Z0-9_]*\\b");
            auto it = rx.globalMatch(editor->toPlainText());
            while (it.hasNext()) dynamicWords << it.next().captured();
            dynamicWords.removeDuplicates();

            auto model = static_cast<QStringListModel*>(editor->completer()->model());
            if (model) model->setStringList(dynamicWords);
        });

        int tabIdx = m_editorTabs->addTab(editor, QFileInfo(filePath).fileName());

        QPushButton* closeBtn = new QPushButton("x", this);
        closeBtn->setObjectName("tabCloseButton");

        QTabBar* bar = m_editorTabs->findChild<QTabBar*>();
        if (bar) {
            bar->setTabButton(tabIdx, QTabBar::RightSide, closeBtn);
        }

        connect(closeBtn, &QPushButton::clicked, [this, editor]() {
            int idx = m_editorTabs->indexOf(editor);
            if (idx != -1) {
                emit m_editorTabs->tabCloseRequested(idx);
            }
        });

        m_editorTabs->setCurrentIndex(tabIdx);
        m_openEditors[filePath] = editor;
    }
}

void MainWindow::showExplorerContextMenu(const QPoint &pos) {
    QMenu contextMenu(tr("Context menu"), this);
    QAction actionNew("New File", this);
    QAction actionDelete("Delete", this);

    connect(&actionNew, &QAction::triggered, this, &MainWindow::createNewFile);
    connect(&actionDelete, &QAction::triggered, this, &MainWindow::deleteSelectedFile);

    contextMenu.addAction(&actionNew);
    contextMenu.addAction(&actionDelete);
    contextMenu.exec(m_fileExplorer->viewport()->mapToGlobal(pos));
}

void MainWindow::createNewFile() {
    QModelIndex index = m_fileExplorer->currentIndex();
    QString path = m_fileModel->filePath(index);
    if (QFileInfo(path).isFile()) path = QFileInfo(path).absolutePath();

    bool ok;
    QString fileName = QInputDialog::getText(this, "New File", "Name:", QLineEdit::Normal, "new_file.sn", &ok);
    if (ok && !fileName.isEmpty()) {
        QFile file(path + "/" + fileName);
        if (file.open(QIODevice::WriteOnly)) file.close();
    }
}

void MainWindow::deleteSelectedFile() {
    QModelIndex index = m_fileExplorer->currentIndex();
    if (!index.isValid()) return;

    QString path = m_fileModel->filePath(index);
    if (QMessageBox::question(this, "Delete", "Are you sure you want to delete this file?") == QMessageBox::Yes) {
        QFile(path).remove();
    }
}

void MainWindow::saveCurrentFile() {
    CodeEditor* currentEditor = qobject_cast<CodeEditor*>(m_editorTabs->currentWidget());
    if (!currentEditor) return;

    QString filePath = m_openEditors.key(currentEditor);
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(currentEditor->toPlainText().toUtf8());
        file.close();
        m_outputLog->append("[+] Saved: " + QFileInfo(filePath).fileName());
    } else {
        m_outputLog->append("[-] OS Error: Failed to save " + filePath);
    }
}
