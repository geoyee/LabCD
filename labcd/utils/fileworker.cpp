#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include "fileworker.h"

bool FileWorker::openImageDir(
    QStringList* t1List,
    QStringList* t2List,
    QWidget* parent
)
{
    QString dirPath = QFileDialog::getExistingDirectory(
        parent,
        QObject::tr("打开图像文件夹"),
        QString(),
        QFileDialog::ShowDirsOnly
    );
    if (dirPath.isEmpty())
    {
        return false;
    }
    
    QDir dir(dirPath);
    QStringList subDirList;
    subDirList = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);
    if ((subDirList.size() != 2 && subDirList.size() != 3) || 
        (subDirList.size() == 2 && subDirList.at(0) != "A" && subDirList.at(1) != "B") || 
        (subDirList.size() == 3 && subDirList.at(0) != "A" && subDirList.at(1) != "B" && subDirList.at(2) != "GT"))
    {
        QMessageBox::critical(
            parent, 
            QObject::tr("错误"),
            QObject::tr("文件组织格式错误，请确保数据文件夹下仅存在两个（或三个）子文件夹，分别为A、B（以及GT）。")
        );
        return false;
    }
    else  // 获取所有图像
    {
        subDirList.removeOne("GT");
        QStringList nameFilters;
        nameFilters << "*.jpg" << "*.jpeg" << "*.png";
        // nameFilters << "*.jpg" << "*.jpeg" << "*.png" << "*.tif" << "*.tiff";
        QDir dirT1(dirPath + "/" + subDirList.at(0));
        QStringList t1ListTmp = (dirT1).entryList(
            nameFilters, QDir::Readable | QDir::Files, QDir::Name);
        QDir dirT2(dirPath + "/" + subDirList.at(1));
        QStringList t2ListTmp = (dirT2).entryList(
            nameFilters, QDir::Readable | QDir::Files, QDir::Name);
        if (t1ListTmp.size() != t2ListTmp.size())
        {
            QMessageBox::critical(
                parent,
                QObject::tr("错误"),
                QObject::tr("时段一数据和时段二数据数量不相等。")
            );
            return false;
        }
        for (int i = 0; i < t1ListTmp.size(); i++)
        {
            t1List->append(
                dirPath + "/" + subDirList.at(0) + "/" + t1ListTmp.at(i));
            t2List->append(
                dirPath + "/" + subDirList.at(1) + "/" + t2ListTmp.at(i));
        }
        return true;
    }
}

bool FileWorker::createFolder(QString path)
{
    QDir* folder = new QDir(path);
    if (!folder->exists())
    {
        if (folder->mkpath(path))
        {
            return true;
        }
    }
    return true;
}
