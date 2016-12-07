#include "Widget.hpp"
#include "CharsetDetector.hpp"
#include "ComboxDelegate.hpp"

#include <QStringList>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>

#include <QPushButton>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QFile>
#include <QDir>
#include <QDebug>


Widget::Widget(QWidget *parent)
    : QWidget(parent),ui_fileTable(nullptr),
      ui_fileModel(nullptr)
{
    this->setWindowTitle(tr("Batch Charset Encoding Convert"));
    //
    initFileTable();
    ui_fileSelect = new QPushButton("...");
    connect(ui_fileSelect,&QPushButton::clicked,
            this,&Widget::fileSelectSlot);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(ui_fileTable);
    layout->addWidget(ui_fileSelect);
}

Widget::~Widget()
{

}

void Widget::fileSelectSlot()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this);
    if(filenames.isEmpty()){
        return;
    }

    CharsetDetector det;
    for(auto& filename:filenames){
        QFile file(filename);
        if(!file.open(QFile::ReadOnly)){
            qDebug()<<"Open file failed "<<filename;
            continue;
        }
        qint64 filesize = file.size();
        QByteArray filedata;
        if(filesize < 1024*1024){
            filedata = file.readAll();
        }else{
            filedata = file.read(1024*1024);
        }
        QString encode = det.detectData(filedata);
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QDir(filename).dirName()));
        QString sfilesize;
        if(filesize < 1024){
            sfilesize.setNum(filesize).append(" B");
        }else if(filesize < 1024*1024){
            sfilesize.setNum(filesize/1024).append(" K");
        }else if(filesize < 1024*1024*1024){
            sfilesize.setNum(filesize/1024/1024).append(" M");
        }else if(filesize < 1024*1024*1024*1024){
            sfilesize.setNum(filesize/1024/1024/1024).append(" G");
        }
        items.append(new QStandardItem(sfilesize));
        items[1]->setTextAlignment(Qt::AlignRight);

        items.append(new QStandardItem(encode));
        items.append(new QStandardItem(encode));
        ui_fileModel->appendRow(items);
    }

    // 根据内容调整列宽
    //ui_fileTable->resizeColumnsToContents();
}

void Widget::initFileTable()
{
    ui_fileModel = new QStandardItemModel;
    QStringList tableheader = {
        tr("File Name"),
        tr("Size"),
        tr("Encode"),
        tr("Out Encode")
    };
    for(int i = 0; i < tableheader.size(); ++i){
        QStandardItem* item = new QStandardItem(tableheader[i]);
        ui_fileModel->setHorizontalHeaderItem(i,item);
    }
    ui_fileTable = new QTableView;
    ui_fileTable->setModel(ui_fileModel);
    ui_fileTable->setItemDelegateForColumn(2,new ComboxDelegate());
    ui_fileTable->setItemDelegateForColumn(3,new ComboxDelegate());
    //ui_fileTable->setH
    //设置充满表宽度(现有列占据整个表格)
    ui_fileTable->horizontalHeader()->setStretchLastSection(true);
    ui_fileTable->setColumnWidth(0,this->width() - 256);
}
