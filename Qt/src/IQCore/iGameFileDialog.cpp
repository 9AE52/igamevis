//
// Created by m_ky on 2024/6/14.
//

/**
 * @class   iGameFileDialog
 * @brief   iGameFileDialog's brief
 */

#include "include/IQCore/iGameFileDialog.h"

#include <QVBoxLayout>
#include <QToolButton>

iGameFileDialog::iGameFileDialog(QWidget *parent) : QFileDialog(parent){
    setOption(QFileDialog::DontUseNativeDialog, true);

    QTreeView *treeView = findChild<QTreeView *>("treeView");
    if (treeView) {
//        treeView->setRootIsDecorated(true);
        treeView->setItemsExpandable(true);
    }

    //  remove Detail and List view button
    QList<QToolButton *> buttons = this->findChildren<QToolButton *>();
    for (QToolButton *button : buttons) {
        if (button->toolTip() == "List View" || button->toolTip() == "Detail View") {
            button->hide();
        }
    }
//    connect(this, &QFileDialog::accepted, this, [](){qDebug() << " acc";});
    connect(this, &QFileDialog::directoryEntered, this, &iGameFileDialog::onDirectoryLoaded);


}

void iGameFileDialog::groupFilesByName() {


    QTreeView *treeView = findChild<QTreeView *>("treeView");
    if (!treeView) return;
    QFileSystemModel *model = qobject_cast<QFileSystemModel *>(treeView->model());
    if (!model) return;


    QModelIndex rootIndex = model->index(directory().absolutePath());
    QMap<QString, QList<QModelIndex>> groups;
//    qDebug() << directory().absolutePath();
//    model->rowC
    int rowCount = model->rowCount(rootIndex);
    qDebug() << "row Count : " << rowCount;
        for (int row = 0; row < rowCount; ++row) {
        QModelIndex index = model->index(row, 0, rootIndex);
        QString fileName = model->data(index).toString();
//        qDebug() << fileName;
        QString filePrefix = fileName.split('_').first();

        groups[filePrefix].append(index);
    }
    // Output groups for demonstration purposes
    for (auto it = groups.constBegin(); it != groups.constEnd(); ++it) {
//        qDebug() << "Group:" << it.key() << " size : " << it.value().size();
    }

    // You can add your logic here to visually group the files in the tree view
}

void iGameFileDialog::onDirectoryLoaded(const QString &path) {
//    qDebug() << "load path " << path;
    groupFilesByName();
}

QStringList iGameFileDialog::getOpenFileNames(const QString &caption, const QString &dir, const QString &filter) {
//    static iGameFileDialog dialog;
    iGameFileDialog dialog;
    dialog.setWindowTitle(caption);
    dialog.setDirectory(dir);
    dialog.setNameFilter(filter);
    // 设置文件模式为ExistingFiles，允许选择多个文件
    dialog.setFileMode(QFileDialog::ExistingFiles);
    if (dialog.exec() == QDialog::Accepted) {
        for (const QString &file : dialog.selectedFiles()) {
            qDebug() << "Selected file:" << file;
        }
        return dialog.selectedFiles();
    }
    return {};
}

void iGameFileDialog::updateFileList(const QString &path) {
//    treeWidget->clear();
//
//    QDir dir(path);
//    QFileInfoList fileList = dir.entryInfoList(QDir::Files |QDir::Dirs | QDir::NoDotAndDotDot);
//
//    QHash<QString, QTreeWidgetItem *> parentItems;
//
//    qDebug() << "=====";
//    for (const QFileInfo &fileInfo : fileList) {
//        QString baseName = fileInfo.completeBaseName();
//        qDebug() << baseName;
//        QTreeWidgetItem *parentItem;
//        if (!parentItems.contains(baseName)) {
//
//            parentItem = new QTreeWidgetItem(treeWidget);
//            parentItem->setText(0, baseName);
//            parentItem->setExpanded(true);
//            parentItems.insert(baseName, parentItem);
//        } else {
//            parentItem = parentItems.value(baseName);
//        }
//
//        QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
//        childItem->setText(0, fileInfo.fileName());
//        childItem->setText(1, QString::number(fileInfo.size()));
//        childItem->setText(2, fileInfo.suffix());
//    }
//
//    treeWidget->resizeColumnToContents(0);
}