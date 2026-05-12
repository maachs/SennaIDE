#pragma once

#include <QTreeWidget>
#include <QDir>

class ProjectTree : public QTreeWidget {
    Q_OBJECT

public:

    explicit ProjectTree(QWidget *parent = nullptr);

    void setProjectDir(const QString& path);

private:

    void populateTree(const QDir& directory, QTreeWidgetItem* parentItem);
};
