#include "Widget.hpp"
#include "CharsetDetector.hpp"
#include "ComboxDelegate.hpp"

#include <QHeaderView>
#include <QStandardItemModel>
#include <QStringList>
#include <QTableView>
#include <QAbstractItemView>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialog>
#include <QListWidget>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVector>
#include <QPair>


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

void Widget::dirSelectSlot()
{
    // 选择文件夹
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"));
    if (dirPath.isEmpty()) {
        return;
    }

    // 定义常用文本文件类型（格式：显示名称, 扩展名列表）
    QVector<QPair<QString, QStringList>> fileTypes = {
        {tr("C/C++ Source Code"), {"c", "cpp", "cc", "cxx", "h", "hpp", "hxx"}},
        {tr("HTML Source Code"), {"html", "htm"}},
        {tr("XML Files"), {"xml"}},
        {tr("CSS Files"), {"css"}},
        {tr("JavaScript"), {"js", "jsx"}},
        {tr("JSON Files"), {"json"}},
        {tr("Python"), {"py", "pyw"}},
        {tr("Java"), {"java"}},
        {tr("C# Source Code"), {"cs"}},
        {tr("PHP"), {"php"}},
        {tr("Ruby"), {"rb"}},
        {tr("Go"), {"go"}},
        {tr("Rust"), {"rs"}},
        {tr("SQL Files"), {"sql"}},
        {tr("Text Files"), {"txt", "log"}},
        {tr("Markdown"), {"md", "markdown"}},
        {tr("Configuration Files"), {"ini", "cfg", "conf", "properties"}},
        {tr("Shell Scripts"), {"sh", "bash", "bat", "cmd"}},
        {tr("YAML Files"), {"yaml", "yml"}},
        {tr("Lua"), {"lua"}},
        {tr("Perl"), {"pl", "pm"}},
        {tr("Swift"), {"swift"}},
        {tr("Kotlin"), {"kt", "kts"}},
        {tr("All Source Files"), {"c", "cpp", "cc", "cxx", "h", "hpp", "hxx", "java", "py", "js", "go", "rs", "cs", "php", "rb", "swift", "kt"}}
    };

    // 创建文件类型选择对话框
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Select File Types"));
    dialog.setMinimumSize(400, 500);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    // 创建列表框用于多选
    QListWidget* listWidget = new QListWidget(&dialog);
    listWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    for (const auto& type : fileTypes) {
        QListWidgetItem* item = new QListWidgetItem(type.first, listWidget);
        item->setData(Qt::UserRole, type.second);  // 存储扩展名列表
        item->setCheckState(Qt::Checked);  // 默认全选
    }

    // 全选/取消全选按钮
    QHBoxLayout* btnLayout = new QHBoxLayout;
    QPushButton* selectAllBtn = new QPushButton(tr("Select All"), &dialog);
    QPushButton* deselectAllBtn = new QPushButton(tr("Deselect All"), &dialog);
    QPushButton* okBtn = new QPushButton(tr("OK"), &dialog);
    QPushButton* cancelBtn = new QPushButton(tr("Cancel"), &dialog);

    btnLayout->addWidget(selectAllBtn);
    btnLayout->addWidget(deselectAllBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addWidget(listWidget);
    mainLayout->addLayout(btnLayout);

    // 连接按钮信号
    connect(selectAllBtn, &QPushButton::clicked, [&]() {
        for (int i = 0; i < listWidget->count(); ++i) {
            listWidget->item(i)->setCheckState(Qt::Checked);
        }
    });

    connect(deselectAllBtn, &QPushButton::clicked, [&]() {
        for (int i = 0; i < listWidget->count(); ++i) {
            listWidget->item(i)->setCheckState(Qt::Unchecked);
        }
    });

    connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    // 收集选中的扩展名
    QStringList selectedExtensions;
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem* item = listWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            QStringList extensions = item->data(Qt::UserRole).toStringList();
            selectedExtensions.append(extensions);
        }
    }

    if (selectedExtensions.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select at least one file type."));
        return;
    }

    qDebug() << selectedExtensions;

    // 递归扫描文件夹中的文件
    QStringList fullPaths;
    QDir dir(dirPath);
    scanFilesRecursively(dir, selectedExtensions, fullPaths);

    if (fullPaths.isEmpty()) {
        QMessageBox::information(this, tr("Info"), tr("No files found matching the selected types."));
        return;
    }

    // 添加到文件模型
    addToFileModel(fullPaths);
}

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

void Widget::scanFilesRecursively(QDir dir, const QStringList& extensions, QStringList& fileList)
{
    // 设置过滤器，只列出文件和目录
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    // 获取当前目录下的所有条目
    QFileInfoList entries = dir.entryInfoList();

    for (const QFileInfo& entry : entries) {
        if (entry.isDir()) {
            // 递归处理子目录
            QDir subDir(entry.absoluteFilePath());
            scanFilesRecursively(subDir, extensions, fileList);
        }
        else if (entry.isFile()) {
            // 检查文件扩展名是否匹配（不区分大小写）
            QString ext = entry.suffix().toLower();
            if (extensions.contains(ext)) {
                fileList.append(entry.absoluteFilePath());
            }
        }
    }
}
