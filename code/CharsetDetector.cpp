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

// 检查是否全部为ASCII
static bool isAllASCII(const QByteArray &data)
{
    for (int i = 0; i < data.size(); ++i) {
        if ((unsigned char)data[i] > 127) {
            return false;
        }
    }
    return true;
}

// 检查是否符合UTF-8编码规则
static bool isValidUTF8(const QByteArray &data)
{
    int i = 0;
    int n = data.size();

    while (i < n) {
        unsigned char c = (unsigned char)data[i];

        if (c < 0x80) {
            // 单字节字符 (0xxxxxxx)
            i++;
        }
        else if ((c & 0xE0) == 0xC0) {
            // 双字节字符 (110xxxxx 10xxxxxx)
            if (i + 1 >= n) return false;
            unsigned char c2 = (unsigned char)data[i + 1];
            if ((c2 & 0xC0) != 0x80) return false;
            // 检查是否是过长的编码
            if (c < 0xC2) return false;  // 过长的编码，应该用单字节表示
            i += 2;
        }
        else if ((c & 0xF0) == 0xE0) {
            // 三字节字符 (1110xxxx 10xxxxxx 10xxxxxx)
            if (i + 2 >= n) return false;
            unsigned char c2 = (unsigned char)data[i + 1];
            unsigned char c3 = (unsigned char)data[i + 2];
            if ((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80) return false;
            // 检查是否是过长的编码
            if (c == 0xE0 && c2 < 0xA0) return false;  // 过长的编码，应该用双字节表示
            i += 3;
        }
        else if ((c & 0xF8) == 0xF0) {
            // 四字节字符 (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
            if (i + 3 >= n) return false;
            unsigned char c2 = (unsigned char)data[i + 1];
            unsigned char c3 = (unsigned char)data[i + 2];
            unsigned char c4 = (unsigned char)data[i + 3];
            if ((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80) return false;
            // 检查是否是过长的编码
            if (c == 0xF0 && c2 < 0x90) return false;  // 过长的编码，应该用三字节表示
            i += 4;
        }
        else {
            // 无效的UTF-8起始字节
            return false;
        }
    }
    return true;
}

QString CharsetDetector::detectFile(QFile &file)
{
    qint64     filesize = file.size();
    QByteArray filedata;
    qint64 bytesToRead = 1024 * 1024;  // 默认读取1MB

    if (filesize < bytesToRead) {
        filedata = file.readAll();
    }
    else {
        filedata = file.read(bytesToRead);
    }

    QString result = detectData(filedata);

    // 如果检测结果为unknown，且没有读取完整个文件
    if (result == "unknow" && filedata.size() < filesize) {
        if (filesize <= (32 * 1024 * 1024)) {
            // 文件小于等于32MB，读取全部内容
            file.seek(0);
            filedata = file.readAll();
        }
        else {
            // 文件大于32MB，读取32MB
            file.seek(0);
            filedata = file.read(32 * 1024 * 1024);
        }

        result = detectData(filedata);
    }

    // 如果检测结果仍为unknown，尝试进一步判断
    if (result == "unknow") {
        if (isAllASCII(filedata)) {
            result = "UTF-8";
        }
        else if (isValidUTF8(filedata)) {
            result = "UTF-8";
        }
    }

    return result;
}

void CharsetDetector::Report(const char *aCharset)
{
    m_charset = aCharset;
}
