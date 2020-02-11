#include "PayloadParser.h"

#include <QDebug>
#include <QJsonObject>

PayloadParser::PayloadParser()
{

}

PayloadParser::PayloadParser(const QJsonArray &c)
    : _config(c)
{
}

void PayloadParser::setConfig(const QJsonArray &c)
{
    _config = c;
}

QJsonArray PayloadParser::config() const
{
    return _config;
}

QVector<QByteArray> PayloadParser::parse(const QByteArray &data)
{
    int length = data.length();
    QVector<QByteArray> datas;
    auto iter = _config.begin();
    for (; iter != _config.end(); ++iter)
    {
        datas.append(readValue((uchar*)data.constData(), length, (*iter).toObject()));
    }

    return datas;
}

QVector<QByteArray> PayloadParser::parseRaw(const QByteArray &data)
{

    int length = data.length();
    QVector<QByteArray> datas;
    auto iter = _config.begin();
    for (; iter != _config.end(); ++iter)
    {
        datas.append(readValue((uchar*)data.constData(),
                               length, (*iter).toObject(), true));
    }

    return datas;
}

QByteArray PayloadParser::combine(const QVector<QByteArray> &data, int length)
{
    if (data.size() != _config.size()) return "";

    QByteArray result(length, 0);
    int offset = 0;
    int size = 0;
    int type = 0;
    int order = 0;
    quint32 mask = 0;
    char* pResult = result.data();
    for (int i=0; i<data.size(); ++i)
    {
        auto info = _config[i].toObject();
        auto segment = data[i];
        offset = info["offset"].toInt();
        size = info["size"].toInt();
        mask = info["mask"].toString().toUInt(nullptr, 0);

        //0-little,1-big
        order = info["order"].toInt();

        if (size <= 0 || offset + size > length)
            continue;

        type = info["type"].toInt();
        switch (type) {
        case 0:
        case 4:
        {
            if (size > 4) break;

            int value = segment.toInt(nullptr, 0);

            //大小端转换
            if (order == 1)
            {
                switch (size) {
                case 2:
                    value = swapI16(value);
                    break;
                case 3:
                case 4:
                    value = swapI32(value);
                    break;
                default:
                    break;
                }
            }

            value = value << searchLowBit(mask);

            //combine segment
            quint32 originValue = 0;
            memcpy(&originValue, pResult+offset, size);
            value |= originValue;

            memcpy(pResult+offset, &value, size);
        }
            break;
        case 1:
        {
            if (size != 4) break;

            float value = segment.toFloat();
            memcpy(pResult+offset, &value, size);
        }
            break;
        case 2:
        {
            if (size != 8) break;

            double value = segment.toDouble();
            memcpy(pResult+offset, &value, size);
        }
            break;
        case 3:
        {
            memcpy(pResult+offset, segment.constData(), size);
        }
            break;
        default:
            break;
        }
    }
    return result;
}

QByteArray PayloadParser::readValue(const uchar* d,
                                    int length,
                                    const QJsonObject& info,
                                    bool raw)
{
    int size = info["length"].toInt() / 8;
    int offset = info["start_bit"].toInt() / 8;
    if (offset + size > length) return "";

    //0-little,1-big
    int order = info["order"].toInt();

    //量纲和幅值
    //double dim = info.contains("dim") ? info["dim"].toDouble() : 1;
    //double amp = info["amp"].toDouble();

    //0-整数,1-单精度浮点,2-双精度浮点,3-字符串,4-Enum
    int type = info["type"].toInt();
    //quint32 mask = info["mask"].toString().toUInt(nullptr, 0);
    QByteArray result;
    switch (type) {
    case 1://整数
    case 4://Enum
    case 5://Integer
    {
        if (size > 4) break;

        //接收解析出的数据
        quint32 value = 0;
        memcpy(&value, d+offset, size);

        //大小端转换
        if (order == 0)
        {
            switch (size) {
            case 2:
                value = swapI16(value);
                break;
            case 3:
                value = swapI32(value) >> 8;
                break;
            case 4:
                value = swapI32(value);
                break;
            default:
                break;
            }
        }

        quint32 tmp = 0xffffffff;
        quint32 off = 32 - info["length"].toInt();
        value = value << (32 - info["length"].toInt() - info["start_bit"].toInt() % 8);
        value = value >> (32 - info["length"].toInt());
        result = QByteArray::number(value);
        //掩码处理
        //value &= mask;

        //value >>= searchLowBit(mask);

//        if (type == 5)
//        {
//            int tmp = this->unsigned2SingedValue(value, size);
//            result = raw ? QByteArray::number(tmp) :
//                           QByteArray::number(tmp*dim +amp, 'f');
//        }
//        else
//        {
//            result = raw ? QByteArray::number(value) :
//                           QByteArray::number(value*dim +amp, 'f');
//        }

    }
        break;

    case 3://string
    {
        result = QByteArray((char*)d+offset, size);
    }
        break;
    default:
        break;
    }

    return result;
}

int PayloadParser::unsigned2SingedValue(quint32 value, int size)
{
    int result = 0;
    switch (size) {
    case 1:
        result = (int)(char)value;
        break;
    case 2:
        result = (int)(short)value;
        break;
    case 3:
    case 4:
        result = (int)value;
        break;
    default:
        break;
    }

    return result;
}

int PayloadParser::searchLowBit(quint32 value)
{
    int position = 0;
    if ((value & 0xFFFF)  == 0) {value >>= 16;position += 16;}
    if ((value & 0xFF  )  == 0) {value >>= 8;position += 8;}
    if ((value & 0xF   )  == 0) {value >>= 4;position += 4;}
    if ((value & 0x3   )  == 0) {value >>= 2;position += 2;}
    if ((value & 0x1   )  == 0) position += 1;

    return position;
}

int PayloadParser::searchHighBit(quint32 value)
{
    int position = 0;
    if ((value & 0xFFFF0000)  == 0) {value <<= 16;position += 16;}
    if ((value & 0xFF000000)  == 0) {value <<= 8;position += 8;}
    if ((value & 0xF0000000)  == 0) {value <<= 4;position += 4;}
    if ((value & 0xC0000000)  == 0) {value <<= 2;position += 2;}
    if ((value & 0x80000000)  == 0) position += 1;

    return position;
}

uint PayloadParser::swapI32(uint v)
{
    return ((v & 0x000000FF) << 24) |
           ((v & 0x0000FF00) << 8) |
           ((v & 0x00FF0000) >> 8) |
            ((v & 0xFF000000) >> 24);
}

uint PayloadParser::swapI16(unsigned short v)
{
    return ((v & 0x00FF) << 8) |
            ((v & 0xFF00) >> 8);
}

int PayloadParser::bitStart(const QJsonObject &info)
{
    if (!info.contains("offset") || !info.contains("mask"))
    {
        qDebug("PayloadParser::bitStart no offset or mask information");
        return -1;
    }

    int offset = info["offset"].toInt();
    quint32 mask = info["mask"].toString().toUInt(nullptr, 0);

    return offset*8 + searchLowBit(mask);
}

int PayloadParser::bitLength(const QJsonObject &info)
{
    if (!info.contains("mask"))
    {
        qDebug("PayloadParser::bitLength no mask information");
        return -1;
    }

    quint32 mask = info["mask"].toString().toUInt(nullptr, 0);
    return 32 - searchLowBit(mask) - searchHighBit(mask);
}

QByteArray PayloadParser::parse(const QByteArray &data, const QJsonObject &segment)
{
    return readValue((uchar*)data.constData(), data.size(), segment);
}

QByteArray PayloadParser::parseRaw(const QByteArray &data, const QJsonObject &segment)
{
    return readValue((uchar*)data.constData(), data.size(), segment, true);
}

PayloadParser::ulonglong PayloadParser::swapI64(PayloadParser::ulonglong v) const
{
    uint tmp = v >> 32;
    v = swapI32(v & 0xFFFFFFFF);
    v <<= 32;
    v |= swapI32(tmp);

    return v;
}

float PayloadParser::swapF32(float v) const
{
    uint tmp = v;
    v = swapI32(tmp);

    return v;
}

double PayloadParser::swapF64(double v) const
{
    ulonglong tmp = v;
    v = swapI64(tmp);

    return v;
}
