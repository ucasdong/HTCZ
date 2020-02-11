#include "CodeDataAnalysis.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

CodeDataAnalysis::CodeDataAnalysis()
{
}

CodeDataAnalysis::~CodeDataAnalysis()
{
}

void CodeDataAnalysis::readDataFile(QString strfileName)
{
	allParamData.clear();
	QFile file(strfileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}

	QString strOneLine;
	QStringList vecDataTemp;
	oneParamData oneData;

	while (!file.atEnd()) //表头
	{
		strOneLine = file.readLine().trimmed();//第一行是属性列表
		if (strOneLine.isEmpty() || strOneLine.contains('*'))
		{
			continue;
		}
		vecDataTemp = strOneLine.split(";");
		for (int i = 0; i < vecDataTemp.size(); i++)
		{
			QStringList strHeader = vecDataTemp[i].split(":");
			if (strHeader.size() == 2)
			{
				tableHeader.push_back(strHeader[1]);
			}
		}
		break;
	}

	while (!file.atEnd())
	{
		vecDataTemp.clear();
		strOneLine = file.readLine().trimmed();

		if (strOneLine == "")
		{
			continue;
		}
		else
		{
			if (strOneLine == "param_End")
			{
				allParamData.push_back(oneData);
				continue;
			}
			vecDataTemp = strOneLine.split("=");
			if (vecDataTemp[0] == "param_Name")
			{
				oneData.strName = vecDataTemp[1];                    //参数名称
			}
			if (vecDataTemp[0] == "param_Type")
			{
				oneData.iType = vecDataTemp[1].toInt();             //数据类型
			}
			if (vecDataTemp[0] == "param_BelongGroup")
			{
				oneData.strBelongGroup = vecDataTemp[1];             //所属组
			}
			if (vecDataTemp[0] == "param_StartBit")
			{
				oneData.iStartBit = vecDataTemp[1].toInt();         //起始字节（行数据之后参数动态设置）
			}
			if (vecDataTemp[0] == "param_AnalysisBits")
			{
				oneData.iAnalysisBits = vecDataTemp[1].toInt();     //读取字节
			}
		}
	}
	file.close();             //关闭文件输入流 
}

//读取数据解析配置文件
void CodeDataAnalysis::readJsonDataFile(QString strfileName)
{
	allParamData.clear();
	tableHeader.clear();
	QFile file(strfileName);
	if (!file.open(QFile::ReadOnly)) {
		qDebug() << "No File";
		return;
	}
	QString json = file.readAll();
	file.close();

	QJsonParseError error;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError) {
		if (jsonDocument.isArray()) {
			QVariantList results = jsonDocument.toVariant().toList();
			for (auto header : results[0].toList()) {
				tableHeader.push_back(header.toString());
			}
			for (auto onedata : results[1].toList()) {
				oneParamData t;
				t.strName = onedata.toMap().value("param_Name").toString();
				t.iType = onedata.toMap().value("param_Type").toInt();
				t.strBelongGroup = onedata.toMap().value("param_BelongGroup").toString();
				t.iStartBit = onedata.toMap().value("param_StartBit").toInt();
				t.iAnalysisBits = onedata.toMap().value("param_AnalysisBits").toInt();
				allParamData.push_back(t);
			}
		}
	}
}

void CodeDataAnalysis::writeJsonDataFile(QString strfileName, vector<oneParamData>& vecAllParam)
{
	QVariantList dataJson;
	QVariantList dataV;
	QVariantList dataHeader;
	for (auto h : tableHeader) {
		dataHeader.append(h);
	}
	for (auto onedata : allParamData) {
		QVariantMap onedataV;
		onedataV.insert("param_Name", onedata.strName);
		onedataV.insert("param_Type", onedata.iType);
		onedataV.insert("param_BelongGroup", onedata.strBelongGroup);
		onedataV.insert("param_StartBit", onedata.iStartBit);
		onedataV.insert("param_AnalysisBits", onedata.iAnalysisBits);
		dataV.append(onedataV);
	}
	dataJson.insert(0, dataHeader);
	dataJson.insert(1, dataV);

	QJsonDocument jsonDocument = QJsonDocument::fromVariant(dataJson);

	//qDebug() << jsonDocument.toJson();


	QFile data(strfileName);
	if (data.open(QFile::WriteOnly | QIODevice::Truncate)) {
		data.write(jsonDocument.toJson());
		data.close();
	}
}

std::vector<oneParamData>* CodeDataAnalysis::getAllParamsData()
{
	return &allParamData;
}

int CodeDataAnalysis::getParamsCount()
{
	int iCount = allParamData.size();
	return iCount;
}

std::vector<QString> CodeDataAnalysis::getTableHeader()
{
	return tableHeader;
}

void CodeDataAnalysis::setTableHeader(vector<QString> vecHeader)
{
	tableHeader = vecHeader;
}

bool CodeDataAnalysis::findOneParamSetting(QString strParamName, oneParamData& oneParam)
{
	vector<oneParamData>::iterator itFind = find_if(allParamData.begin(), allParamData.end(),
		[strParamName](oneParamData& oneParam) {return strParamName == oneParam.strName; });

	if (itFind != allParamData.end())
	{
		oneParam = *itFind;
		return true;
	}
	return false;
}

BYTE* CodeDataAnalysis::DecodePacket(BYTE *package, QString strName, int& packageLen)
{
	oneParamData one;
	bool bHave = findOneParamSetting(strName, one);
	if (bHave)
	{
		int iLen = 0;
		BYTE* data = GetBits(package, one, iLen);
		packageLen = iLen;//获取数据大小（单位BYTE）
		return data;
	}
	return NULL;
}

bool CodeDataAnalysis::GetBit(BYTE* package, int iStart, int iCount, BYTE& data)
{
	unsigned int nBytesBegin = iStart / 8;
	unsigned int nBitsBegin = iStart % 8;
	unsigned int nBitsCount = iCount;
	data = 0x00;
	if (nBitsCount > 8)
	{
		nBitsCount = 8;
	}
	if (nBitsCount == 0)
	{
		return false;
	}
	unsigned short bytes2 = package[nBytesBegin];
	bytes2 = (bytes2 << 8) | package[nBytesBegin + 1];
	for (int i = 15; i >= 0; --i)
	{
		if (i >= (int)(16 - nBitsBegin))
		{
			continue;
		}
		if (i < (int)(16 - nBitsBegin - nBitsCount))
		{
			continue;
		}
		unsigned int bR = (bytes2 & (0x1 << i) ? 1 : 0);
		data = (data << 1) | bR;
	}
	return true;
}

BYTE* CodeDataAnalysis::GetBits(BYTE* package, int iStart, int iCount, int* packageLen)
{
	unsigned int nBytesBegin = iStart / 8;//起始字节
	unsigned int nBitsBegin = iStart % 8; //偏移位
	unsigned int nBitsCount = iCount;     //截取位数
	if (nBitsCount <= 0)//截取长度非法
	{
		return NULL;
	}
	//申请内存
	int iByteSize = nBitsCount / 8;     //截取整字节数
	int iOtherBiteSize = nBitsCount % 8;//截取整字节数外位数
	if (iOtherBiteSize > 0)
	{
		iByteSize++;
	}
	BYTE* data = new BYTE[iByteSize];
	*packageLen = iByteSize;


	if (iOtherBiteSize == 0)//1截取整字节
	{
		if (nBitsBegin == 0)//1.1偏移0位
		{
			for (int i = 0; i < iByteSize; i++)
			{
				*(data + i) = package[nBytesBegin + i];
			}
		}
		else//偏移不是0位
		{
			int iNeedBits = nBitsCount;
			for (int i = 0; i < iByteSize; i++)
			{
				BYTE lastByte = 0x00;
				GetBit(package, iStart + i * 8, iNeedBits, lastByte);
				*(data + i) = lastByte;
				iNeedBits -= 8;
			}
		}
	}
	else//2截取不是整字节
	{
		if (iByteSize == 1)//截取长度未超过一个字节
		{
			int iNeedBits = nBitsCount;
			BYTE lastByte = 0x00;
			GetBit(package, iStart, iNeedBits, lastByte);
			*(data) = lastByte;
		}
		else//截取长度超过一个字节
		{
			int iNeedBits = nBitsCount;
			for (int i = 0; i < iByteSize - 1; i++)
			{
				BYTE lastByte = 0x00;
				GetBit(package, iStart + i * 8, iNeedBits, lastByte);
				*(data + i) = lastByte;
				iNeedBits -= 8;
			}
			BYTE lastByteTemp = 0x00;//最后一个字节
			GetBit(package, iStart + (iByteSize - 1) * 8, iNeedBits, lastByteTemp);
			lastByteTemp = lastByteTemp << (8 - iNeedBits);
			*(data + iByteSize - 1) = lastByteTemp;
			//截取所有字节右移 8 - iNeedBits
			movRight(data, iByteSize, 8 - iNeedBits);
		}
	}
	return data;
}

BYTE* CodeDataAnalysis::GetBits(BYTE* package, oneParamData &one, int& packageLen)
{
	return GetBits(package, one.iStartBit, one.iAnalysisBits, &packageLen);
}

void CodeDataAnalysis::movRight(BYTE* data, int iLen, int iCount)
{
	for (int i = iLen - 1; i >= 0; --i)
	{
		if (i == 0)
		{
			BYTE dataTemp1 = data[i] >> iCount;
			*data = dataTemp1;
		}
		else
		{
			BYTE dataTemp1 = *(data + i) >> iCount;
			BYTE dataTemp2 = *(data + i - 1);
			dataTemp2 = dataTemp2 << (8 - iCount);
			*(data + i) = dataTemp1 | dataTemp2;
		}

	}
}
