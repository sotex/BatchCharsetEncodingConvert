#include "CharsetDetector.hpp"

CharsetDetector::CharsetDetector(PRUint32 aLanguageFilter)
    :nsUniversalDetector(aLanguageFilter)
{
}

QString CharsetDetector::detectData(const QByteArray &data)
{
    m_charset.clear();
    this->Reset();
    this->HandleData(data.data(),data.size());
    this->DataEnd();
    if(m_charset.isEmpty()){
        m_charset = "unknow"; //"UTF-8"; //
    }
    return m_charset;
}

QString CharsetDetector::detectFile(QFile &file)
{
    qint64     filesize = file.size();
    QByteArray filedata;
    if (filesize < 1024 * 1024) {
        filedata = file.readAll();
    }
    else {
        filedata = file.read(1024 * 1024);
    }
    return detectData(filedata);
}

void CharsetDetector::Report(const char *aCharset)
{
    m_charset = aCharset;
}
