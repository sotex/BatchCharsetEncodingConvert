#include "FileConvertWorker.hpp"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

FileConvertWorker::FileConvertWorker(const QString& filePath,
                                       const QString& fromEncoding,
                                       const QString& toEncoding,
                                       bool createBackup,
                                       QAtomicInt* cancelFlag)
    : m_filePath(filePath)
    , m_fromEncoding(fromEncoding)
    , m_toEncoding(toEncoding)
    , m_createBackup(createBackup)
    , m_cancelFlag(cancelFlag)
{
    setAutoDelete(true);
}

FileConvertWorker::~FileConvertWorker()
{
}

void FileConvertWorker::run()
{
    int successCount = 0;
    int failCount = 0;

    if (m_cancelFlag && m_cancelFlag->load() != 0) {
        emit conversionCompleted(0, 0);
        return;
    }

    // 备份文件
    if (m_createBackup) {
        if (!backupFile(m_filePath)) {
            emit conversionError(m_filePath, "Backup failed");
            emit progressUpdated(0, 1, m_filePath);
            return;
        }
    }

    // 转换文件
    if (convertFile(m_filePath, m_fromEncoding, m_toEncoding)) {
        successCount = 1;
    }
    else {
        failCount = 1;
    }

    emit progressUpdated(successCount, failCount, m_filePath);
}

bool FileConvertWorker::backupFile(const QString& filePath)
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

bool FileConvertWorker::convertFile(const QString& filePath, const QString& fromEncoding, const QString& toEncoding)
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