#include "DataBlockBroadcast.h"
#include <fstream>
#include <string>
#include <stdlib.h>
//#include <windows.h>
#include <sys/stat.h>

DataBlockBroadcast::DataBlockBroadcast()
{
	SUM = 0x00;
	iCount = 0;
	allProgramData = NULL;
/*
	BYTE dataTemp[6] = {0x14,0x56,0xf8,0x9A,0x00,0x01};
	vector<BYTE> veda;
	for (int i = 0; i < 6; i++)
	{
		veda.push_back(dataTemp[i]);
	}
    BYTE a,b;
	GetTestISO(a,b,veda);
	int aa = 0;
	*/
}

DataBlockBroadcast::~DataBlockBroadcast()
{
	if (allProgramData != NULL)
	{
		delete [] allProgramData;
		allProgramData = NULL;
	}
}

void DataBlockBroadcast::readBroadcastFile(string strFileName)
{
	//allParamData.clear();
	mapAllData.clear();
	ifstream infile;
	infile.open(strFileName.c_str(), ios::in);   //将文件流对象与文件连接起来 
	if (!infile.is_open())
	{
		return;
	}
	string strOneLine;
	//string strName;
	vector<string> vecDataTemp;
	oneBroadCastData oneData;

	while (getline(infile, strOneLine)) //第一行是属性列表
	{
		string::size_type idx;
		idx = strOneLine.find('*');
		if (idx != string::npos)//存在'*'
		{
			continue;
		}
		if (strOneLine == "")
		{
			continue;
		}

		vecDataTemp.clear();
		vecDataTemp = split(strOneLine, "=");
		if (strOneLine == "")
		{
			continue;
		}
		else
		{
			if (strOneLine == "param_End")
			{
				//allParamData.push_back(oneData);
				mapAllData.insert(std::pair<string, oneBroadCastData>(oneData.param_Name, oneData));
				continue;
			}
			vecDataTemp = split(strOneLine, "=");
			if (vecDataTemp[0] == "param_Name")
			{
				oneData.param_Name = vecDataTemp[1]; //参数名称
			}
			if (vecDataTemp[0] == "param_Head")
			{
				oneData.param_Head = vecDataTemp[1]; //帧头       
			}
			if (vecDataTemp[0] == "param_I")
			{
				oneData.param_I = vecDataTemp[1];    //帧序号
			}
			if (vecDataTemp[0] == "param_L")
			{
				oneData.param_L = vecDataTemp[1];    //有效数据长度
			}
			if (vecDataTemp[0] == "param_T")
			{
				oneData.param_T = vecDataTemp[1];//标识(数据类型+站地址标识)
			}
			if (vecDataTemp[0] == "param_PerBagSize")
			{
				oneData.param_PerBagSize = atoi(vecDataTemp[1].c_str());    //每个包的大小
			}
			if (vecDataTemp[0] == "param_AllBagCount")
			{
				oneData.param_AllBagCount = atoi(vecDataTemp[1].c_str());   //包的个数
			}
			if (vecDataTemp[0] == "param_BagCount")
			{
				oneData.param_BagCount = atoi(vecDataTemp[1].c_str());    //包帧数
			}
			if (vecDataTemp[0] == "param_BagLength")
			{
				oneData.param_BagLength = atoi(vecDataTemp[1].c_str());//帧长度
			}
		}
	}

	infile.close();             //关闭文件输入流 
}

std::vector<std::string> DataBlockBroadcast::split(string str, string pattern)
{
	string::size_type pos;
	vector<string> result;

	str += pattern;

	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}

	return result;
}

BYTE DataBlockBroadcast::TranslateCharToHex(char data)
{
	BYTE dataTemp = 0x00;
	switch (data)
	{
	case '0':
		dataTemp = 0x00;
		break;
	case '1':
		dataTemp = 0x01;
		break;
	case '2':
		dataTemp = 0x02;
		break;
	case '3':
		dataTemp = 0x03;
		break;
	case '4':
		dataTemp = 0x04;
		break;
	case '5':
		dataTemp = 0x05;
		break;
	case '6':
		dataTemp = 0x06;
		break;
	case '7':
		dataTemp = 0x07;
		break;
	case '8':
		dataTemp = 0x08;
		break;
	case '9':
		dataTemp = 0x09;
		break;
	case 'a':
	case 'A':
		dataTemp = 0x0a;
		break;
	case 'b':
	case 'B':
		dataTemp = 0x0b;
		break;
	case 'c':
	case 'C':
		dataTemp = 0x0c;
		break;
	case 'd':
	case 'D':
		dataTemp = 0x0d;
		break;
	case 'e':
	case 'E':
		dataTemp = 0x0e;
		break;
	case 'f':
	case 'F':
		dataTemp = 0x0f;
		break;
	default:
		break;
	}
	return dataTemp;
}

std::vector<BYTE*> DataBlockBroadcast::GetOnePagData(string strName, BYTE type, BYTE VERSION, int iPag)
{
	vecData.clear();
	map<string, oneBroadCastData>::iterator itFind = mapAllData.find(strName);
	if (itFind == mapAllData.end())
	{
		return vecData;
	}

	BYTE head1 = TranslateCharToHex(itFind->second.param_Head[0]) * 16 + TranslateCharToHex(itFind->second.param_Head[1]);
	BYTE head2 = TranslateCharToHex(itFind->second.param_Head[2]) * 16 + TranslateCharToHex(itFind->second.param_Head[3]);
	BYTE head_L = 0x00;
	if (itFind->second.param_L != "FALSE")
	{
		head_L = TranslateCharToHex(itFind->second.param_L[0]) * 16 + TranslateCharToHex(itFind->second.param_L[1]);
	}
	BYTE head_T = 0x00;
	if (itFind->second.param_T != "FALSE")
	{
		head_T = TranslateCharToHex(itFind->second.param_T[0]) * 16 + TranslateCharToHex(itFind->second.param_T[1]);
	}
	int i = 0;          //用于统计当前文件数据字符位置

	SUM = 0x00;         //用于包结尾赋值
	bool bBegin = false;//用于判断是否开始累加

	BYTE w2 = 0x00;
	BYTE w3 = 0x00;
	BYTE w4 = 0x00;
	BYTE w5 = 0x00;
	BYTE w6 = ((iPag + 1) / 256) % 256;//包序
	BYTE w7 = (iPag + 1) % 256;        //包序
	vector<BYTE> TestISO;
	TestISO.clear();
	BYTE w246 = 0x00;
	BYTE w247 = 0x00;
	if (iPag == 0)
	{
		w2 = 0x00;
		w3 = 0x00;
		w4 = 0x00;
		w5 = 0x01;
	}
	else
	{
		int iPagIndex = 119 * iPag + 2;
		w5 = iPagIndex % 256;
		w4 = (iPagIndex / 256) % 256;
		w3 = ((iPagIndex / 256) / 256) % 256;
		w2 = (((iPagIndex / 256) / 256) / 256) % 256;
	}
	for (int iFrameCount = 0; iFrameCount < itFind->second.param_BagCount; iFrameCount++)//包帧数
	{
		BYTE* data = new BYTE[itFind->second.param_BagLength];   //一帧的数据
		for (int iFrameWidth = 0; iFrameWidth < itFind->second.param_BagLength; iFrameWidth++)//每帧长度
		{
			if (iFrameWidth == 0)
			{
				*(data + iFrameWidth) = head1;
				continue;
			}
			else if (iFrameWidth == 1)
			{
				*(data + iFrameWidth) = head2;
				continue;
			}
			else if (iFrameWidth == 2)
			{
				if (itFind->second.param_I == "TRUE")
				{
					*(data + iFrameWidth) = iFrameCount;
				}
				continue;
			}
			if (iFrameCount == 0)//第一帧
			{
				if (iFrameWidth == 3)
				{
					if (itFind->second.param_L != "FALSE")
					{
						*(data + iFrameWidth) = head_L;
					}
					continue;
				}
				else if (iFrameWidth == 4)
				{
					if (itFind->second.param_T != "FALSE")
					{
						bBegin = true;
						*(data + iFrameWidth) = head_T;
						if (bBegin == true)
						{
							Sum(*(data + iFrameWidth));
						}
						continue;
					}
				}
				else if (iFrameWidth == 5)//类型w0
				{
					if (iPag == 0)//第一包
					{
					}
					*(data + iFrameWidth) = type;
					if (bBegin == true)
					{
						Sum(*(data + iFrameWidth));
					}
					continue;
				}
				else if (iFrameWidth == 6)//版本号w1
				{
					*(data + iFrameWidth) = VERSION;
					if (bBegin == true)
					{
						Sum(*(data + iFrameWidth));
					}
					continue;
				}
				else if (iFrameWidth == 7)//w2
				{
					*(data + iFrameWidth) = w2;
					if (bBegin == true)
					{
						Sum(*(data + iFrameWidth));
					}
					continue;
				}
				else if (iFrameWidth == 8)//w3
				{
					*(data + iFrameWidth) = w3;
					if (bBegin == true)
					{
						Sum(*(data + iFrameWidth));
					}
					continue;
				}
				else if (iFrameWidth == 9)//w4
				{
					*(data + iFrameWidth) = w4;
					if (bBegin == true)
					{
						Sum(*(data + iFrameWidth));
					}
					continue;
				}
			}
			if (iFrameCount == 1)//第二帧特殊情况是w5和包序号（w6,w7)
			{
				if (iFrameWidth == 3)//w5
				{
					*(data + iFrameWidth) = w5;
					if (bBegin == true)
					{
						Sum(*(data + iFrameWidth));
					}
					continue;
				}
				else if (iFrameWidth == 4)//w6
				{
					*(data + iFrameWidth) = w6;
					if (bBegin == true)
					{
						TestISO.push_back(*(data + iFrameWidth));
						Sum(*(data + iFrameWidth));
					}
					continue;
				}
				else if (iFrameWidth == 5)//w7
				{
					*(data + iFrameWidth) = w7;
					if (bBegin == true)
					{
						TestISO.push_back(*(data + iFrameWidth));
						Sum(*(data + iFrameWidth));
					}
					continue;
				}
			}
			if (iFrameCount == itFind->second.param_BagCount - 1)//最后一帧
			{
				if (iFrameWidth == itFind->second.param_BagLength - 4)//倒数第四个个字节w246
				{
					*(data + iFrameWidth) = 0x00;//暂时填0
					continue;
				}
				if (iFrameWidth == itFind->second.param_BagLength - 3)//倒数第三个字节w247
				{
					*(data + iFrameWidth) = 0x00;//暂时填0
					continue;
				}
				if (iFrameWidth == itFind->second.param_BagLength - 2)//倒数第二个字节
				{
					*(data + iFrameWidth) = 0x00;
					if (bBegin == true)
					{
						Sum(*(data + iFrameWidth));
					}
					continue;
				}
				else if (iFrameWidth == itFind->second.param_BagLength - 1)//最后一个字节
				{	
					//ISO校验赋值
					GetTestISO(w246, w247, TestISO);
					*(data + itFind->second.param_BagLength - 4) = w246;
					*(data + itFind->second.param_BagLength - 3) = w247;
					Sum(w246);
					Sum(w247);
					*(data + iFrameWidth) = SUM;
					bBegin = false;
					continue;
				}
			}
			//BYTE bb = *(allProgramData + iPag * 238 + i);
			if (iPag * 238 + i < iCount)
			{
				*(data + iFrameWidth) = *(allProgramData + iPag * 238 + i);
				i++;
			}
			else
			{
				*(data + iFrameWidth) = 0x00;
			}

			if (bBegin == true)
			{
				TestISO.push_back(*(data + iFrameWidth));
				Sum(*(data + iFrameWidth));
			}
		}
		vecData.push_back(data);
	}
	/*
	isoTest2(w246,w247,vecData);
	BYTE* bC = vecData[vecData.size() - 1];
	*(bC + 6) = w246;
	*(bC + 7) = w247;
	Sum(w246);
	Sum(w247);
	*(bC + 9) = SUM;
	*/
	return vecData;
}

vector<BYTE*> DataBlockBroadcast::GetSendData(string strName, BYTE type, BYTE VERSION)
{
	vecData.clear();
	map<string, oneBroadCastData>::iterator itFind = mapAllData.find(strName);
	if (itFind == mapAllData.end())
	{
		return vecData;
	}
	
	BYTE head1 = TranslateCharToHex(itFind->second.param_Head[0]) * 16 + TranslateCharToHex(itFind->second.param_Head[1]);
	BYTE head2 = TranslateCharToHex(itFind->second.param_Head[2]) * 16 + TranslateCharToHex(itFind->second.param_Head[3]);
	BYTE head_L = 0x00;
	if (itFind->second.param_L != "FALSE") 
	{
		head_L = TranslateCharToHex(itFind->second.param_L[0]) * 16 + TranslateCharToHex(itFind->second.param_L[1]);
	}
	BYTE head_T = 0x00;
	if (itFind->second.param_T != "FALSE")
	{
		head_T = TranslateCharToHex(itFind->second.param_T[0]) * 16 + TranslateCharToHex(itFind->second.param_T[1]);
	}
	int i = 0;          //用于统计当前文件数据字符位置
	for (int iPagCount = 0; iPagCount < itFind->second.param_AllBagCount; iPagCount++)//包的个数
	{
		SUM = 0x00;         //用于包结尾赋值
		bool bBegin = false;//用于判断是否开始累加
		int iPagIndex = 0x00;
		BYTE w2 = 0x00;
		BYTE w3 = 0x00;
		BYTE w4 = 0x00;
		BYTE w5 = 0x00;
		BYTE w6 = ((iPagCount + 1) / 256) % 256;
		BYTE w7 = (iPagCount + 1) % 256;
		vector<BYTE> TestISO;
		TestISO.clear();
		BYTE w246 = 0x00;
		BYTE w247 = 0x00;
		if (iPagCount == 0)
		{
			w2 = 0x00;
			w3 = 0x00;
			w4 = 0x00;
			w5 = 0x01;
		}
		else
		{
			iPagIndex = 119 * iPagCount + 2;
			w5 = iPagIndex % 256;
			w4 = (iPagIndex / 256) % 256;
			w3 = ((iPagIndex / 256) / 256) % 256;
			w2 = (((iPagIndex / 256) / 256)/256) % 256;
		}
		for (int iFrameCount = 0; iFrameCount < itFind->second.param_BagCount; iFrameCount++)//包帧数
		{
			BYTE* data = new BYTE[itFind->second.param_BagLength];   //一帧的数据
			for (int iFrameWidth = 0;iFrameWidth < itFind->second.param_BagLength;iFrameWidth++)//每帧长度
			{
				if (iFrameWidth == 0)
				{
					*(data + iFrameWidth) = head1;
					continue;
				}
				else if (iFrameWidth == 1)
				{
					*(data + iFrameWidth) = head2;
					continue;
				}
				else if (iFrameWidth == 2)
				{
					if (itFind->second.param_I == "TRUE")
					{
						*(data + iFrameWidth) = iFrameCount;
					}
					continue;
				}
				if (iFrameCount == 0)//第一帧
				{
					if (iFrameWidth == 3)
					{
						if (itFind->second.param_L != "FALSE")
						{
							*(data + iFrameWidth) = head_L;
						}
						continue;
					}
					else if (iFrameWidth == 4)
					{
						if (itFind->second.param_T != "FALSE")
						{
							bBegin = true;
							*(data + iFrameWidth) = head_T;
							if (bBegin == true)
							{
								Sum(*(data + iFrameWidth));
							}
							continue;
						}
					}
					else if (iFrameWidth == 5)//类型w0
					{
						if (iPagCount == 0)//第一包
						{
						}
						*(data + iFrameWidth) = type;
						if (bBegin == true)
						{
							Sum(*(data + iFrameWidth));
						}
						continue;
					}
					else if (iFrameWidth == 6)//版本号w1
					{
						*(data + iFrameWidth) = VERSION;
						if (bBegin == true)
						{
							Sum(*(data + iFrameWidth));
						}
						continue;
					}
					else if (iFrameWidth == 7)//w2
					{
						*(data + iFrameWidth) = w2;
						if (bBegin == true)
						{
							Sum(*(data + iFrameWidth));
						}
						continue;
					}
					else if (iFrameWidth == 8)//w3
					{
						*(data + iFrameWidth) = w3;
						if (bBegin == true)
						{
							Sum(*(data + iFrameWidth));
						}
						continue;
					}
					else if (iFrameWidth == 9)//w4
					{
						*(data + iFrameWidth) = w4;
						if (bBegin == true)
						{
							Sum(*(data + iFrameWidth));
						}
						continue;
					}
				}
				if (iFrameCount == 1)//第二帧特殊情况是w5和包序号（w6,w7)
				{
					if (iFrameWidth == 3)//w5
					{
						*(data + iFrameWidth) = w5;
						if (bBegin == true)
						{
							Sum(*(data + iFrameWidth));
						}
						continue;
					}
					else if (iFrameWidth == 4)//w6
					{
						*(data + iFrameWidth) = w6;
						if (bBegin == true)
						{
							TestISO.push_back(*(data + iFrameWidth));
							Sum(*(data + iFrameWidth));
						}
						continue;
					}
					else if (iFrameWidth == 5)//w7
					{
						*(data + iFrameWidth) = w7;
						if (bBegin == true)
						{
							TestISO.push_back(*(data + iFrameWidth));
							Sum(*(data + iFrameWidth));
						}
						continue;
					}
				}
				if (iFrameCount == itFind->second.param_BagCount - 1)//最后一帧
				{
					if (iFrameWidth == itFind->second.param_BagLength - 4)//倒数第四个个字节w246
					{
						//*(data + iFrameWidth) = 0x00;//暂时填0
						continue;
					}
					if (iFrameWidth == itFind->second.param_BagLength - 3)//倒数第三个字节w247
					{
						//*(data + iFrameWidth) = 0x00;//暂时填0
						continue;
					}
					if (iFrameWidth == itFind->second.param_BagLength - 2)//倒数第二个字节
					{
						*(data + iFrameWidth) = 0x00;
						if (bBegin == true)
						{
							Sum(*(data + iFrameWidth));
						}
						continue;
					}
					else if (iFrameWidth == itFind->second.param_BagLength - 1)//最后一个字节
					{
						//ISO校验赋值
						GetTestISO(w246, w247, TestISO);
						*(data + itFind->second.param_BagLength - 4) = w246;
						*(data + itFind->second.param_BagLength - 3) = w247;

						*(data + iFrameWidth) = SUM;
						bBegin = false;
						continue;
					}
				}
				BYTE bb = *(allProgramData + i);
				if (i < iCount)
				{
					*(data + iFrameWidth) = *(allProgramData + i);
					i++;
				}
				else
				{
					*(data + iFrameWidth) = 0x00;
				}

				if (bBegin == true)
				{
					TestISO.push_back(*(data + iFrameWidth));
					Sum(*(data + iFrameWidth));
				}
			}
			vecData.push_back(data);
		}
	}
	return vecData;
}

void DataBlockBroadcast::DestorySendData()
{
	if (vecData.size() != 0)
	{
		for (int i = 0;i < vecData.size();i++)
		{
			delete[] vecData[i];
		}
	}
	vecData.clear();
}

void DataBlockBroadcast::Sum(BYTE num)
{
	BYTE Num = SUM + num;//用于累加
	if (Num > 255)
	{
		SUM = Num - 256;
	}
	else
	{
		SUM = Num;
	}
	return;
	SUM += num;
	
}
void DataBlockBroadcast::isoTest2(BYTE& num1, BYTE& num2,vector<BYTE*> vecTemp)
{
	vector<BYTE> temp;
	for(int i = 0;i < vecTemp.size() - 1;i++)
	{
		BYTE* data = vecTemp[i];
		if(i == 0)
		{
			continue;
		}
		else if(i == 1)
		{
			for(int j = 4;j < 10;j++)
			{
				temp.push_back(*(data + j));
			}
		}
		else 
		{
			for(int j = 3;j < 10;j++)
			{
				temp.push_back(*(data + j));
			}
		}
	}
	BYTE* data = vecTemp[vecTemp.size() - 1];
	for(int j = 3;j < 6;j++)
	{
		temp.push_back(*(data + j));
	}
	GetTestISO(num1, num2, temp);
	int a = 0;
}

void DataBlockBroadcast::GetTestISO(BYTE& num1, BYTE& num2, vector<BYTE>& vecTemp)
{
	BYTE C0 = 0x00;
	BYTE C1 = 0x00;
	for (int i = 0; i < vecTemp.size();i++)
	{
		int temp1 = C0 + vecTemp[i];
		if (temp1 > 255)
		{
			temp1 = temp1 - 255;
		}
		C0 = temp1;
		int temp2 = C1 + C0;
		if (temp2 > 255)
		{
			temp2 = temp2 - 255;
		}
		C1 = temp2;
	}
	int temp3 = C1 + C0;
	if (temp3 > 255)
	{
		temp3 = temp3 -255;
	}
	BYTE CK1 = 255 - temp3;
	BYTE CK2 = C1;
	if (CK1 == 0)
	{
		CK1 = 0xff;
	}
	if (CK2 == 0)
	{
		CK2 = 0xff;
	}
	num1 = CK1;
	num2 = CK2;
}

bool DataBlockBroadcast::ReadSendDataFile(string strFileName, string& errInfo)
{
	ifstream fin(strFileName, ios::binary);
	if (!fin) 
	{
		errInfo = "file open failed";
		return false;
	}
	struct _stat info;
	_stat(strFileName.c_str(), &info);
	iCount = info.st_size;
	if(allProgramData != NULL)
	{
		delete [] allProgramData;
		allProgramData = NULL;
	}
	allProgramData = new BYTE[iCount];
	
	memset(allProgramData, 0, iCount);
	fin.read((char*)allProgramData, iCount);
	fin.close();
	if (iCount != 6154368)//6154368
	{
		errInfo = "file's size is wrong";
		return false;
	}
	return true;
// 	int count1 = 0;
// 	while (!fin.eof())
// 	{
// 		getline(fin, d);
// 		count1 += d.size(); //累计字符数
// 	}
	
	/*
	allProgramData = new BYTE[count];
	memset(allProgramData, 0, count);
	//bool bopen = fin.is_open();
	fin.seekg(0, ios::beg);
	//long pos1 = fin.tellg();   //输出文件指针位置
	count = 0;
	while (!fin.eof()) //以行为单位读入文件
	{
		getline(fin, d);
		for (int i = 0; i < d.size(); i++)
		{
			*(allProgramData + count + i) = d[i];
		}
		count += d.size(); //累计字符数
	}

	fin.close();
	return true;*/
}
