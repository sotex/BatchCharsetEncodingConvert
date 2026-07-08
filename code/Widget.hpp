#ifndef WIDGET_H
#define WIDGET_H

#include "EncodingDetectWorker.hpp"

#include <QDir>
#include <QWidget>
#include <QThreadPool>
#include <QAtomicInt>
#include <QProgressDialog>

class QTableView;
class QStandardItemModel;
class QPushButton;
class QMenu;

class Widget : public QWidget
{
    Q_OBJECT

public:
    static const int COL_FULLPATH = 4;  // 隐藏的完整路径列

public:
    Widget(QWidget* parent = 0);
    ~Widget();

public slots:
    void fileSelectSlot();
    void dirSelectSlot();
    void convertToSlot();

private slots:
    void onHeaderContextMenu(const QPoint& pos);
    void onTableContextMenu(const QPoint& pos);
    void onSetUniformOutEncode();
    void onSetSelectedEncode();
    void onSetSelectedOutEncode();
    void onOpenFileLocation();
    void onOpenWithNotepad();
    void onRemoveSelected();

    // 文件扫描相关的槽
    void onFileScanProgress(int currentCount, const QString& currentFile);
    void onFileScanCompleted(const QStringList& files);
    void onFileScanError(const QString& error);

    // 编码检测相关的槽
    void onEncodingDetected(const FileEncodingResult& result);
    void onEncodingDetectError(const QString& filePath, const QString& error);

    // 文件转换相关的槽
    void onFileConvertProgress(int successCount, int failCount, const QString& currentFile);
    void onFileConvertCompleted(int successCount, int failCount);
    void onFileConvertError(const QString& filePath, const QString& error);

    // 进度对话框取消槽
    void onProgressDialogCanceled();

private:
    void addToFileModel(QStringList filenames);
    void initFileTable();
    void scanFilesRecursively(QDir dir, const QStringList& extensions, QStringList& fileList);
    QString selectUnknownEncodingDialog(const QString& fileName, bool& applyToAll);
    bool convertFile(const QString& filePath, const QString& fromEncoding, const QString& toEncoding, bool createBackup);
    bool backupFile(const QString& filePath);

    // 多线程任务方法
    void startFileScanning(const QString& dirPath, const QStringList& extensions);
    void startEncodingDetection(const QStringList& files);
    void startFileConversion();

    // UI更新方法
    void showProgressDialog(const QString& title);
    void hideProgressDialog();
    void updateProgressDialog(int value, int maximum, const QString& status);
    void resetProgressDialog();

private:
    QTableView*         ui_fileTable;
    QStandardItemModel* ui_fileModel;
    QPushButton*        ui_fileSelect;
    QPushButton*        ui_dirSelect;
    QPushButton*        ui_ConvertTo;
    QPushButton*        ui_Exit;
    QMenu*              m_contextMenu;
    QString             m_unknownEncoding;       // 全局未知编码设置
    bool                m_applyToAllUnknown;     // 是否应用到全部未知编码文件

    // 多线程相关成员
    QThreadPool*        m_threadPool;
    QAtomicInt          m_cancelFlag;
    QProgressDialog*    m_progressDialog;
    int                 m_totalFilesToProcess;
    int                 m_processedFiles;
    int                 m_conversionSuccessCount;
    int                 m_conversionFailCount;
};

#endif   // WIDGET_H
