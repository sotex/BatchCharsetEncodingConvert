#include "Widget.hpp"
#include "CharsetDetector.hpp"
#include "ComboxDelegate.hpp"
#include "FileScanWorker.hpp"
#include "EncodingDetectWorker.hpp"
#include "FileConvertWorker.hpp"

#include <QHeaderView>
#include <QStandardItemModel>
#include <QStringList>
#include <QTableView>
#include <QAbstractItemView>
#include <QMenu>
#include <QComboBox>
#include <QLabel>
#include <QAction>
#include <QCheckBox>
#include <set>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextStream>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialog>
#include <QListWidget>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QVector>
#include <QPair>
#include <QSet>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QModelIndexList>
#include <algorithm>


Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui_fileTable(nullptr)
    , ui_fileModel(nullptr)
    , m_contextMenu(nullptr)
    , m_applyToAllUnknown(false)
    , m_threadPool(nullptr)
    , m_cancelFlag(0)
    , m_progressDialog(nullptr)
    , m_totalFilesToProcess(0)
    , m_processedFiles(0)
    , m_conversionSuccessCount(0)
    , m_conversionFailCount(0)
{
    this->setWindowTitle(tr("Batch Charset Encoding Convert"));

    // 注册FileEncodingResult到元类型系统
    qRegisterMetaType<FileEncodingResult>("FileEncodingResult");

    // 初始化线程池
    m_threadPool = new QThreadPool(this);
    m_threadPool->setMaxThreadCount(qMin(4, QThread::idealThreadCount()));

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

    // 启动后台文件扫描任务
    startFileScanning(dirPath, selectedExtensions);
}

void Widget::convertToSlot()
{
    // 启动后台文件转换任务
    startFileConversion();
}

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
        QString               encode = det.detectFile(file);
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QFileInfo(filename).fileName()));
        items.back()->setToolTip(filename);

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

        // 保存完整路径到隐藏列
        items.append(new QStandardItem(filename));

        ui_fileModel->appendRow(items);
    }
}

void Widget::initFileTable()
{
    ui_fileModel            = new QStandardItemModel;
    QStringList tableheader = {tr("File Name"), tr("Size"), tr("Encode"), tr("Out Encode"), tr("Path")};
    for (int i = 0; i < tableheader.size(); ++i) {
        QStandardItem* item = new QStandardItem(tableheader[i]);
        ui_fileModel->setHorizontalHeaderItem(i, item);
    }
    ui_fileTable = new QTableView;
    ui_fileTable->setModel(ui_fileModel);

    // 启用排序功能
    ui_fileTable->setSortingEnabled(true);

    ui_fileTable->setItemDelegateForColumn(2, new ComboxDelegate());
    ui_fileTable->setItemDelegateForColumn(3, new ComboxDelegate());

    // 设置表头右键菜单（仅针对"Out Encode"列，即第3列）
    QHeaderView* headerView = ui_fileTable->horizontalHeader();
    headerView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(headerView, &QHeaderView::customContextMenuRequested, this, &Widget::onHeaderContextMenu);

    // 设置数据行右键菜单
    ui_fileTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui_fileTable, &QTableView::customContextMenuRequested, this, &Widget::onTableContextMenu);

    // 隐藏完整路径列（第5列）
    ui_fileTable->setColumnHidden(COL_FULLPATH, true);

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

void Widget::onHeaderContextMenu(const QPoint& pos)
{
    QHeaderView* header = ui_fileTable->horizontalHeader();
    int column = header->logicalIndexAt(pos);

    // 只在"Out Encode"列（第3列）显示右键菜单
    if (column == 3) {
        QMenu menu(this);
        QAction* action = menu.addAction(tr("统一设置输出编码"));
        connect(action, &QAction::triggered, this, &Widget::onSetUniformOutEncode);
        menu.exec(header->mapToGlobal(pos));
    }
}

void Widget::onTableContextMenu(const QPoint& pos)
{
    QModelIndex index = ui_fileTable->indexAt(pos);
    if (!index.isValid() || index.column() >= 2) {
        return;
    }

    // 获取选中的行数
    QModelIndexList selectedIndexes = ui_fileTable->selectionModel()->selectedIndexes();
    QSet<int> selectedRows;
    for(auto& index : selectedIndexes){
        selectedRows.insert(index.row());
    }
    int selectedCount = selectedRows.size();

    QMenu menu(this);

    if (selectedCount == 1) {
        // 单选：显示三个菜单项
        QAction* openLocationAction = menu.addAction(tr("打开所在位置"));
        QAction* openNotepadAction = menu.addAction(tr("使用记事本打开"));
        QAction* removeAction = menu.addAction(tr("移除"));

        connect(openLocationAction, &QAction::triggered, this, &Widget::onOpenFileLocation);
        connect(openNotepadAction, &QAction::triggered, this, &Widget::onOpenWithNotepad);
        connect(removeAction, &QAction::triggered, this, &Widget::onRemoveSelected);
    }
    else {
        // 多选：仅显示移除
        QAction* removeAction = menu.addAction(tr("移除"));
        connect(removeAction, &QAction::triggered, this, &Widget::onRemoveSelected);
    }

    menu.exec(ui_fileTable->mapToGlobal(pos));
}

void Widget::onSetUniformOutEncode()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("统一设置输出编码"));
    dialog.setMinimumSize(300, 150);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QLabel* label = new QLabel(tr("请选择输出编码："), &dialog);
    QComboBox* comboBox = new QComboBox(&dialog);

    // 添加常用编码
    QStringList encodings = {"UTF-8", "UTF-8 BOM", "UTF-16BE", "UTF-16LE", "UTF-32BE", "UTF-32LE", "ISO-2022-CN",
                             "BIG5", "EUC-TW", "GB18030", "HZ-GB-2312"};
    comboBox->addItems(encodings);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    QPushButton* okBtn = new QPushButton(tr("确定"), &dialog);
    QPushButton* cancelBtn = new QPushButton(tr("取消"), &dialog);

    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    layout->addWidget(label);
    layout->addWidget(comboBox);
    layout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString selectedEncoding = comboBox->currentText();

        // 更新所有行的"Out Encode"列
        for (int row = 0; row < ui_fileModel->rowCount(); ++row) {
            QStandardItem* item = ui_fileModel->item(row, 3);
            if (item) {
                item->setText(selectedEncoding);
            }
        }
    }
}

void Widget::onOpenFileLocation()
{
    QModelIndex index = ui_fileTable->selectionModel()->currentIndex();
    QStandardItem* fullPathItem = ui_fileModel->item(index.row(), COL_FULLPATH);
    if (!fullPathItem) {
        return;
    }

    QString filePath = fullPathItem->text();
    QFileInfo fileInfo(filePath);
    QString absolutePath = fileInfo.absolutePath();

    // 使用系统文件管理器打开文件所在位置
    QUrl url = QUrl::fromLocalFile(absolutePath);
    QDesktopServices::openUrl(url);
}

void Widget::onOpenWithNotepad()
{
    QModelIndex index = ui_fileTable->selectionModel()->currentIndex();
    QStandardItem* fullPathItem = ui_fileModel->item(index.row(), COL_FULLPATH);
    if (!fullPathItem) {
        return;
    }

    QString filePath = fullPathItem->text();

    // 使用记事本打开文件
    QProcess::startDetached("notepad.exe", QStringList() << filePath);
}

void Widget::onRemoveSelected()
{
    QModelIndexList selectedIndexes = ui_fileTable->selectionModel()->selectedIndexes();

    // 从后往前删除，避免索引变化
    std::set<int> rows;
    for (const QModelIndex& index : selectedIndexes) {
        rows.insert(index.row());
    }
    for (auto it = rows.rbegin(); it != rows.rend(); it++) {
        ui_fileModel->removeRow(*it);
    }
}

QString Widget::selectUnknownEncodingDialog(const QString& fileName, bool& applyToAll)
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Select Encoding"));
    dialog.setMinimumSize(350, 180);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QLabel* label = new QLabel(tr("File '%1' has unknown encoding.\nPlease select the encoding:").arg(fileName), &dialog);
    QComboBox* comboBox = new QComboBox(&dialog);

    // 添加常用编码
    QStringList encodings = {"UTF-8", "GBK", "GB18030", "GB2312", "Big5", "ISO-8859-1", "Shift_JIS", "EUC-KR", "EUC-JP"};
    comboBox->addItems(encodings);

    QCheckBox* applyToAllCheckBox = new QCheckBox(tr("Apply to all unknown encoding files"), &dialog);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    QPushButton* okBtn = new QPushButton(tr("OK"), &dialog);
    QPushButton* cancelBtn = new QPushButton(tr("Skip"), &dialog);

    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    layout->addWidget(label);
    layout->addWidget(comboBox);
    layout->addWidget(applyToAllCheckBox);
    layout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        applyToAll = applyToAllCheckBox->isChecked();
        return comboBox->currentText();
    }

    return QString();
}

bool Widget::backupFile(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString backupPath = filePath + ".bak";

    // 检查备份文件是否已存在
    if (QFile::exists(backupPath)) {
        // 删除旧的备份文件
        QFile::remove(backupPath);
    }

    return QFile::copy(filePath, backupPath);
}

bool Widget::convertFile(const QString& filePath, const QString& fromEncoding, const QString& toEncoding, bool createBackup)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Cannot open file for reading:" << filePath;
        return false;
    }

    // 读取文件内容
    QTextStream in(&file);
    in.setCodec(fromEncoding.toLatin1().constData());
    QString content = in.readAll();
    file.close();

    // 写入转换后的内容
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
        qDebug() << "Cannot open file for writing:" << filePath;
        return false;
    }

    QTextStream out(&file);
    out.setCodec(toEncoding.toLatin1().constData());
    out << content;
    file.close();

    return true;
}

// ==================== 文件扫描相关的槽 ====================

void Widget::onFileScanProgress(int currentCount, const QString& currentFile)
{
    if (m_progressDialog) {
        updateProgressDialog(currentCount, 0, tr("Scanning: %1").arg(currentFile));
    }
}

void Widget::onFileScanCompleted(const QStringList& files)
{
    if (files.isEmpty()) {
        hideProgressDialog();

        if (m_cancelFlag.load() != 0) {
            QMessageBox::information(this, tr("Info"), tr("File scanning was cancelled."));
        }
        else {
            QMessageBox::information(this, tr("Info"), tr("No files found matching the selected types."));
        }
        return;
    }

    // 重置进度对话框
    hideProgressDialog();
    resetProgressDialog();

    // 开始编码检测
    startEncodingDetection(files);
}

void Widget::onFileScanError(const QString& error)
{
    hideProgressDialog();
    QMessageBox::critical(this, tr("Error"), error);
}

// ==================== 编码检测相关的槽 ====================

void Widget::onEncodingDetected(const FileEncodingResult& result)
{
    // 更新表格模型
    QList<QStandardItem*> items;
    items.append(new QStandardItem(QFileInfo(result.filePath).fileName()));

    QString sfilesize;
    if (result.fileSize < 1024) {
        sfilesize.setNum(result.fileSize).append(" B");
    }
    else if (result.fileSize < 1024 * 1024) {
        sfilesize.setNum(result.fileSize / 1024).append(" K");
    }
    else if (result.fileSize < 1024 * 1024 * 1024) {
        sfilesize.setNum(result.fileSize / 1024 / 1024).append(" M");
    }
    else if (result.fileSize < 1024 * 1024 * 1024 * 1024) {
        sfilesize.setNum(result.fileSize / 1024 / 1024 / 1024).append(" G");
    }
    items.append(new QStandardItem(sfilesize));
    items[1]->setTextAlignment(Qt::AlignRight);

    items.append(new QStandardItem(result.encoding));
    items.append(new QStandardItem(result.encoding));
    items.append(new QStandardItem(result.filePath));

    ui_fileModel->appendRow(items);

    // 更新进度
    m_processedFiles++;
    if (m_progressDialog) {
        updateProgressDialog(m_processedFiles, m_totalFilesToProcess, tr("Detecting: %1").arg(QFileInfo(result.filePath).fileName()));
    }

    // 检查是否完成
    if (m_processedFiles >= m_totalFilesToProcess) {
        hideProgressDialog();
        m_cancelFlag.store(0);
    }
}

void Widget::onEncodingDetectError(const QString& filePath, const QString& error)
{
    qDebug() << "Encoding detection error for file:" << filePath << error;

    // 继续处理下一个文件
    m_processedFiles++;
    if (m_processedFiles >= m_totalFilesToProcess) {
        hideProgressDialog();
        m_cancelFlag.store(0);
    }
}

// ==================== 文件转换相关的槽 ====================

void Widget::onFileConvertProgress(int successCount, int failCount, const QString& currentFile)
{
    m_conversionSuccessCount += successCount;
    m_conversionFailCount += failCount;

    if (m_progressDialog) {
        updateProgressDialog(m_conversionSuccessCount + m_conversionFailCount, m_totalFilesToProcess, tr("Converting: %1").arg(currentFile));
    }

    // 检查是否所有文件都已处理完成
    if (m_conversionSuccessCount + m_conversionFailCount >= m_totalFilesToProcess) {
        hideProgressDialog();

        QString message = tr("Conversion completed:\n");
        message += tr("Success: %1 file(s)\n").arg(m_conversionSuccessCount);
        message += tr("Failed: %1 file(s)").arg(m_conversionFailCount);

        QMessageBox::information(this, tr("Conversion Result"), message);
        m_cancelFlag.store(0);
    }
}

void Widget::onFileConvertCompleted(int successCount, int failCount)
{
    // 不再使用此信号，改用onFileConvertProgress中的计数器判断完成
}

void Widget::onFileConvertError(const QString& filePath, const QString& error)
{
    qDebug() << "File conversion error for file:" << filePath << error;
}

// ==================== 进度对话框相关方法 ====================

void Widget::onProgressDialogCanceled()
{
    m_cancelFlag.store(1);
}

void Widget::showProgressDialog(const QString& title)
{
    if (!m_progressDialog) {
        m_progressDialog = new QProgressDialog(this);
        m_progressDialog->setWindowModality(Qt::WindowModal);
        m_progressDialog->setCancelButtonText(tr("Cancel"));
        m_progressDialog->setMinimumDuration(0);
        m_progressDialog->setRange(0, 0);  // 初始设置为无限模式
        connect(m_progressDialog, &QProgressDialog::canceled, this, &Widget::onProgressDialogCanceled);
    }

    m_progressDialog->setWindowTitle(title);
    m_progressDialog->setValue(0);  // 重置进度值
    m_progressDialog->show();
}

void Widget::hideProgressDialog()
{
    if (m_progressDialog) {
        m_progressDialog->hide();
    }
}

void Widget::updateProgressDialog(int value, int maximum, const QString& status)
{
    if (m_progressDialog) {
        m_progressDialog->setRange(0, maximum);
        m_progressDialog->setValue(value);
        m_progressDialog->setLabelText(status);
    }
}

void Widget::resetProgressDialog()
{
    m_processedFiles = 0;
    m_conversionSuccessCount = 0;
    m_conversionFailCount = 0;
    m_cancelFlag.store(0);
}

// ==================== 多线程任务方法 ====================

void Widget::startFileScanning(const QString& dirPath, const QStringList& extensions)
{
    resetProgressDialog();
    showProgressDialog(tr("Scanning Files"));

    FileScanWorker* worker = new FileScanWorker(dirPath, extensions, &m_cancelFlag);
    connect(worker, &FileScanWorker::progressUpdated, this, &Widget::onFileScanProgress);
    connect(worker, &FileScanWorker::scanCompleted, this, &Widget::onFileScanCompleted);
    connect(worker, &FileScanWorker::scanError, this, &Widget::onFileScanError);

    m_threadPool->start(worker);
}

void Widget::startEncodingDetection(const QStringList& files)
{
    if (files.isEmpty()) {
        return;
    }

    resetProgressDialog();
    m_totalFilesToProcess = files.size();
    m_processedFiles = 0;

    // 先显示进度对话框
    showProgressDialog(tr("Detecting Encodings"));

    // 立即设置进度范围和初始值
    if (m_progressDialog) {
        m_progressDialog->setRange(0, m_totalFilesToProcess);
        m_progressDialog->setValue(0);
        m_progressDialog->setLabelText(tr("Starting detection..."));
        // 强制刷新UI
        QApplication::processEvents();
    }

    // 启动多个EncodingDetectWorker并行处理
    for (const QString& filePath : files) {
        EncodingDetectWorker* worker = new EncodingDetectWorker(filePath);
        connect(worker, &EncodingDetectWorker::resultReady, this, &Widget::onEncodingDetected);
        connect(worker, &EncodingDetectWorker::errorOccurred, this, &Widget::onEncodingDetectError);
        m_threadPool->start(worker);
    }
}

void Widget::startFileConversion()
{
    // 统计需要转换的文件
    QList<QPair<int, QString>> filesToConvert;
    int totalFiles = 0;
    int skippedFiles = 0;  // 未知编码文件数量

    for (int row = 0; row < ui_fileModel->rowCount(); ++row) {
        QString fromEncoding = ui_fileModel->item(row, 2)->text();
        QString toEncoding = ui_fileModel->item(row, 3)->text();
        QString filePath = ui_fileModel->item(row, COL_FULLPATH)->text();

        if (fromEncoding == toEncoding) {
            continue;
        }

        // 处理未知编码
        if (fromEncoding == "unknow" || fromEncoding.isEmpty()) {
            skippedFiles++;
            continue;
        }

        filesToConvert.append(qMakePair(row, filePath));
        totalFiles++;
    }

    if (totalFiles == 0 && skippedFiles > 0) {
        QMessageBox::information(this, tr("Info"), tr("All files have unknown encoding. Please select encoding manually."));
        return;
    }

    if (totalFiles == 0) {
        QMessageBox::information(this, tr("Info"), tr("No files need to be converted."));
        return;
    }

    // 询问是否备份
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Backup"),
                                 tr("Found %1 file(s) to convert.\nDo you want to create backup files (.bak) before conversion?")
                                 .arg(totalFiles),
                                 QMessageBox::Yes | QMessageBox::No);

    bool needBackup = (reply == QMessageBox::Yes);

    // 重置进度并开始转换
    resetProgressDialog();
    m_totalFilesToProcess = totalFiles;  // 设置为实际需要处理的文件数
    m_conversionSuccessCount = 0;
    m_conversionFailCount = 0;

    // 先显示进度对话框
    showProgressDialog(tr("Converting Files"));

    // 立即设置进度范围和初始值
    if (m_progressDialog) {
        m_progressDialog->setRange(0, m_totalFilesToProcess);
        m_progressDialog->setValue(0);
        m_progressDialog->setLabelText(tr("Starting conversion..."));
        // 强制刷新UI
        QApplication::processEvents();
    }

    // 启动多个FileConvertWorker并行处理
    for (const auto& filePair : filesToConvert) {
        int row = filePair.first;
        QString filePath = filePair.second;
        QString fromEncoding = ui_fileModel->item(row, 2)->text();
        QString toEncoding = ui_fileModel->item(row, 3)->text();

        FileConvertWorker* worker = new FileConvertWorker(filePath, fromEncoding, toEncoding, needBackup, &m_cancelFlag);
        connect(worker, &FileConvertWorker::progressUpdated, this, &Widget::onFileConvertProgress);
        // 不再连接conversionCompleted信号，改用progressUpdated中的计数器判断完成
        connect(worker, &FileConvertWorker::conversionError, this, &Widget::onFileConvertError);
        m_threadPool->start(worker);
    }
}
