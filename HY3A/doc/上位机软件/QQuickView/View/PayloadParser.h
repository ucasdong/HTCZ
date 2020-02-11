#ifndef PAYLOADPARSER_H
#define PAYLOADPARSER_H

#include <QJsonArray>
#include <QByteArray>
#include <QVector>

class QJsonObject;

/**
 * @brief PayloadParser类负责解析或组合一帧载荷数据，规则按照载荷规则指定；使用时调用parse接口
 *          传入指定数据，返回解析后数据；使用combine接口组合出一帧数据。
 */
class PayloadParser
{
    using ulonglong = unsigned long long;

public:
    PayloadParser();

    /**
     * @brief PayloadParser构造函数
     * @param c为载荷规则描述
     */
    PayloadParser(const QJsonArray& c);

    /**
     * @brief setConfig设置载荷规则信息
     * @param c载荷规则信息
     */
    void setConfig(const QJsonArray& c);

    /**
     * @brief config获取载荷规则信息
     * @return 载荷规则信息
     */
    QJsonArray config() const;

    /**
     * @brief parse将data数据按照载荷规则解析出各规则对应的数据
     * @param data要解析的数据
     * @return 解析后的数据
     */
    virtual QVector<QByteArray> parse(const QByteArray& data);

    /**
     * @brief parseRaw将data数据按照载荷规则解析出各规则对应的数据，不会计算幅值和偏移
     * @param data要解析的数据
     * @return 解析后的数据
     */
    virtual QVector<QByteArray> parseRaw(const QByteArray& data);

    /**
     * @brief combine组合datas，最终长度为length，datas长度与规则描述信息长度必须一致；
     *          长度超出length的规则不会被处理;暂不支持按位组帧
     * @param datas要拼接的所有数据
     * @param length指定拼接数据的总长度
     * @return 返回拼接后的数据
     */
    virtual QByteArray combine(const QVector<QByteArray>& datas, int length);

    /**
     * @brief parse解析data数据，字段信息在segment中指定
     * @param data被解析的数据
     * @param segment数据对应offset、size等信息，大小单位为Byte
     * @return 成功，返回解析后的数据对应的字符串；失败，返回空
     */
    QByteArray parse(const QByteArray& data, const QJsonObject& segment);

    /**
     * @brief parseRaw解析data数据，字段信息在segment中指定,不会计算幅值和偏移
     * @param data被解析的数据
     * @param segment数据对应offset、size等信息，大小单位为Byte
     * @return 成功，返回解析后的数据对应的字符串；失败，返回空
     */
    QByteArray parseRaw(const QByteArray& data, const QJsonObject& segment);

    static uint swapI32(uint v);
    static uint swapI16(unsigned short v);

    static int bitStart(const QJsonObject& info);
    static int bitLength(const QJsonObject& info);

private:
    Q_DISABLE_COPY(PayloadParser);
    QByteArray readValue(const uchar* d,
                         int length,
                         const QJsonObject& info,
                         bool raw = false);
    int unsigned2SingedValue(quint32 value, int size);
    static int searchLowBit(quint32 value);
    static int searchHighBit(quint32 value);

    ulonglong swapI64(ulonglong v) const;
    float swapF32(float v) const;
    double swapF64(double v) const;

private:
    QJsonArray  _config;
};

#endif // PAYLOADPARSER_H
