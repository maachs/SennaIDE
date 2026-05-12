#include "project_tree.hpp"
#include <QFileInfo>
#include <QtGlobal>

ProjectTree::ProjectTree(QWidget *parent) : QTreeWidget(parent) {

    Q_ASSERT(this != nullptr);

    this->setHeaderLabel("Project Explorer");
}

void ProjectTree::setProjectDir(const QString& path) {

    QDir rootDir(path);
    if (!rootDir.exists()) {
        return;
    }

    this->clear();

    QTreeWidgetItem* rootItem = new QTreeWidgetItem(this);
    rootItem->setText(0, rootDir.dirName());

    populateTree(rootDir, rootItem);

    rootItem->setExpanded(true);
}

void ProjectTree::populateTree(const QDir& directory, QTreeWidgetItem* parentItem) {

    QFileInfoList entries = directory.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst);

    for (const QFileInfo& info : entries) {
        QTreeWidgetItem* item = new QTreeWidgetItem(parentItem);
        item->setText(0, info.fileName());

        if (info.isDir()) {
            populateTree(QDir(info.absoluteFilePath()), item);
        } else {
            if (info.suffix() == "sn") {
                item->setForeground(0, Qt::darkBlue);
            }
        }
    }
}
