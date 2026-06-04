#include "Widget.hpp"
#include "CharsetDetector.hpp"
#include "ComboxDelegate.hpp"

#include <QHeaderView>
#include <QStandardItemModel>
#include <QStringList>
#include <QTableView>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>


Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui_fileTable(nullptr)
    , ui_fileModel(nullptr)
{
    this->setWindowTitle(tr("Batch Charset Encoding Convert"));
    // 初始化文件表
    initFileTable();

    // 初始化按钮
    ui_fileSelect = new QPushButton("选择文件...");
    connect(ui_fileSelect, &QPushButton::clicked, this, &Widget::fileSelectSlot);
    ui_dirSelect = new QPushButton("选择文件夹...");
    connect(ui_dirSelect, &QPushButton::clicked, this, &Widget::dirSelectSlot);
    ui_ConvertTo = new QPushButton("转换编码...");
    connect(ui_ConvertTo, &QPushButton::clicked, this, &Widget::convertToSlot);
    ui_Exit = new QPushButton("退出");
    connect(ui_Exit, &QPushButton::clicked, this, &Widget::close);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(ui_fileSelect);
    btnLayout->addWidget(ui_dirSelect);
    btnLayout->addStretch();
    btnLayout->addWidget(ui_ConvertTo);
    btnLayout->addWidget(ui_Exit);


    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(ui_fileTable, 1);
    layout->addLayout(btnLayout);

    setLayout(layout);
}

Widget::~Widget() {}

void Widget::fileSelectSlot()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this);
    if (filenames.isEmpty()) {
        return;
    }

    addToFileModel(filenames);

    // 根据内容调整列宽
    // ui_fileTable->resizeColumnsToContents();
}

void Widget::dirSelectSlot() {}

void Widget::convertToSlot() {}

void Widget::addToFileModel(QStringList filenames)
{
    CharsetDetector det;
    for (auto& filename : filenames) {
        QFile file(filename);
        if (!file.open(QFile::ReadOnly)) {
            qDebug() << "Open file failed " << filename;
            continue;
        }
        qint64     filesize = file.size();
        QByteArray filedata;
        if (filesize < 1024 * 1024) {
            filedata = file.readAll();
        }
        else {
            filedata = file.read(1024 * 1024);
        }
        QString               encode = det.detectData(filedata);
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QDir(filename).dirName()));
        QString sfilesize;
        if (filesize < 1024) {
            sfilesize.setNum(filesize).append(" B");
        }
        else if (filesize < 1024 * 1024) {
            sfilesize.setNum(filesize / 1024).append(" K");
        }
        else if (filesize < 1024 * 1024 * 1024) {
            sfilesize.setNum(filesize / 1024 / 1024).append(" M");
        }
        else if (filesize < 1024 * 1024 * 1024 * 1024) {
            sfilesize.setNum(filesize / 1024 / 1024 / 1024).append(" G");
        }
        items.append(new QStandardItem(sfilesize));
        items[1]->setTextAlignment(Qt::AlignRight);

        items.append(new QStandardItem(encode));
        items.append(new QStandardItem(encode));
        ui_fileModel->appendRow(items);
    }
}

void Widget::initFileTable()
{
    ui_fileModel            = new QStandardItemModel;
    QStringList tableheader = {tr("File Name"), tr("Size"), tr("Encode"), tr("Out Encode")};
    for (int i = 0; i < tableheader.size(); ++i) {
        QStandardItem* item = new QStandardItem(tableheader[i]);
        ui_fileModel->setHorizontalHeaderItem(i, item);
    }
    ui_fileTable = new QTableView;
    ui_fileTable->setModel(ui_fileModel);
    ui_fileTable->setItemDelegateForColumn(2, new ComboxDelegate());
    ui_fileTable->setItemDelegateForColumn(3, new ComboxDelegate());
    // ui_fileTable->setH
    // 设置充满表宽度(现有列占据整个表格)
    ui_fileTable->horizontalHeader()->setStretchLastSection(true);
    ui_fileTable->setColumnWidth(0, this->width() - 256);
}
