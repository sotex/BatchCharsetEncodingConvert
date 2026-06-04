#include "FileScanWorker.hpp"

#include <QDir>
#include <QFileInfo>
#include <QDebug>

FileScanWorker::FileScanWorker(const QString& dirPath, const QStringList& extensions, QAtomicInt* cancelFlag)
    : m_dirPath(dirPath)
    , m_extensions(extensions)
    , m_cancelFlag(cancelFlag)
{
    setAutoDelete(true);
}

FileScanWorker::~FileScanWorker()
{
}

void FileScanWorker::run()
{
    QStringList fileList;
    QDir        dir(m_dirPath);

    if (!dir.exists()) {
        emit scanError(QString("Directory does not exist: %1").arg(m_dirPath));
        return;
    }

    int count = 0;
    scanFilesRecursively(dir, m_extensions, fileList, count);

    if (m_cancelFlag && m_cancelFlag->load() != 0) {
        emit scanCompleted(QStringList());  // 返回空列表表示取消
        return;
    }

    emit scanCompleted(fileList);
}

void FileScanWorker::scanFilesRecursively(QDir& dir, const QStringList& extensions, QStringList& fileList, int& count)
{
    if (m_cancelFlag && m_cancelFlag->load() != 0) {
        return;
    }

    // 设置过滤器，只列出文件和目录
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    // 获取当前目录下的所有条目
    QFileInfoList entries = dir.entryInfoList();

    for (const QFileInfo& entry : entries) {
        if (m_cancelFlag && m_cancelFlag->load() != 0) {
            return;
        }

        if (entry.isDir()) {
            // 递归处理子目录
            QDir subDir(entry.absoluteFilePath());
            scanFilesRecursively(subDir, extensions, fileList, count);
        }
        else if (entry.isFile()) {
            // 检查文件扩展名是否匹配（不区分大小写）
            QString ext = entry.suffix().toLower();
            if (extensions.contains(ext)) {
                fileList.append(entry.absoluteFilePath());
                count++;

                // 每隔一定数量发送一次进度信号
                if (count % PROGRESS_INTERVAL == 0) {
                    emit progressUpdated(count, entry.fileName());
                }
            }
        }
    }
}
