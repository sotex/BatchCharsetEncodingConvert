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
        m_charset = "UTF-8"; //"unknow";
    }
    return m_charset;
}

void CharsetDetector::Report(const char *aCharset)
{
    m_charset = aCharset;
}
