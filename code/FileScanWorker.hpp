#ifndef FILESCANWORKER_HPP
#define FILESCANWORKER_HPP

#include <QObject>
#include <QString>
#include <QStringList>
#include <QRunnable>
#include <QAtomicInt>
#include <QDir>

class FileScanWorker : public QObject, public QRunnable
{
    Q_OBJECT

public:
    FileScanWorker(const QString& dirPath, const QStringList& extensions, QAtomicInt* cancelFlag);
    ~FileScanWorker();

    void run() override;

signals:
    void progressUpdated(int currentCount, const QString& currentFile);
    void scanCompleted(const QStringList& files);
    void scanError(const QString& error);

private:
    void scanFilesRecursively(QDir& dir, const QStringList& extensions, QStringList& fileList, int& count);

private:
    QString        m_dirPath;
    QStringList    m_extensions;
    QAtomicInt*    m_cancelFlag;
    static const int PROGRESS_INTERVAL = 100;  // 每扫描100个文件发送一次进度
};

#endif   // FILESCANWORKER_HPP
