#ifndef FILECONVERTWORKER_HPP
#define FILECONVERTWORKER_HPP

#include <QObject>
#include <QString>
#include <QRunnable>
#include <QAtomicInt>

class FileConvertWorker : public QObject, public QRunnable
{
    Q_OBJECT

public:
    FileConvertWorker(const QString& filePath,
                      const QString& fromEncoding,
                      const QString& toEncoding,
                      bool createBackup,
                      QAtomicInt* cancelFlag);
    ~FileConvertWorker();

    void run() override;

signals:
    void progressUpdated(int successCount, int failCount, const QString& currentFile);
    void conversionCompleted(int successCount, int failCount);
    void conversionError(const QString& filePath, const QString& error);

private:
    bool backupFile(const QString& filePath);
    bool convertFile(const QString& filePath, const QString& fromEncoding, const QString& toEncoding);

private:
    QString        m_filePath;
    QString        m_fromEncoding;
    QString        m_toEncoding;
    bool           m_createBackup;
    QAtomicInt*    m_cancelFlag;
};

#endif   // FILECONVERTWORKER_HPP