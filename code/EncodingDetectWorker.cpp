#include "EncodingDetectWorker.hpp"
#include "CharsetDetector.hpp"

#include <QFile>
#include <QDebug>

EncodingDetectWorker::EncodingDetectWorker(const QString& filePath)
    : m_filePath(filePath)
{
    setAutoDelete(true);
}

EncodingDetectWorker::~EncodingDetectWorker()
{
}

void EncodingDetectWorker::run()
{
    QFile file(m_filePath);

    if (!file.open(QFile::ReadOnly)) {
        emit errorOccurred(m_filePath, QString("Cannot open file: %1").arg(file.errorString()));
        return;
    }

    qint64     filesize = file.size();
    QString    encoding;
    CharsetDetector det;

    try {
        encoding = det.detectFile(file);
    }
    catch (const std::exception& e) {
        emit errorOccurred(m_filePath, QString("Detection failed: %1").arg(e.what()));
        return;
    }

    FileEncodingResult result;
    result.filePath = m_filePath;
    result.encoding = encoding;
    result.fileSize = filesize;

    emit resultReady(result);
}