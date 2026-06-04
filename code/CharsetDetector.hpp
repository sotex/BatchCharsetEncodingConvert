#ifndef CHARSETDETECTOR_HPP
#define CHARSETDETECTOR_HPP

#include "nscore.h"
#include "nsUniversalDetector.h"

#include <QFile>
#include <QString>


class CharsetDetector : public nsUniversalDetector
{
public:
    CharsetDetector(PRUint32 aLanguageFilter = NS_FILTER_ALL);

    QString detectData(const QByteArray& data);

    QString detectFile(QFile &file);

protected:
    virtual void Report(const char* aCharset);
    QString m_charset;
};

#endif // CHARSETDETECTOR_HPP
