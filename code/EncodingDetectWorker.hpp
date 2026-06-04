#ifndef ENCODINGDETECTWORKER_HPP
#define ENCODINGDETECTWORKER_HPP

#include <QObject>
#include <QString>
#include <QRunnable>

struct FileEncodingResult {
    QString filePath;
    QString encoding;
    qint64  fileSize;
};

class EncodingDetectWorker : public QObject, public QRunnable
{
    Q_OBJECT

public:
    EncodingDetectWorker(const QString& filePath);
    ~EncodingDetectWorker();

    void run() override;

signals:
    void resultReady(const FileEncodingResult& result);
    void errorOccurred(const QString& filePath, const QString& error);

private:
    QString m_filePath;
};

#endif   // ENCODINGDETECTWORKER_HPP