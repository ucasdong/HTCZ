#pragma once

#include <iostream>
using namespace std;

typedef unsigned char BYTE;

struct oneBroadCastData
{
	string   param_Name;     //广播名称
	string   param_Head;     //帧头
	string   param_I;        //帧序号
	string   param_L;        //有效数据长度
	string   param_T;        //标识（数据类型+站地址标识）
	int      param_PerBagSize;//每个包的大小
	int      param_AllBagCount;//包的个数
	int      param_BagCount; //包帧数（行数）
	int      param_BagLength;//包帧长度（每行字节数）
};

//上注数据块广播类型（待扩展）
enum ShangzhuTypeEnum
{
	LEFT_PROGRAM,   //左半视场 程序上注
	RIGHT_PROGRAM,  //右半视场 程序上注
	LEFT_PARAM,     //左半视场 参数上注
	RIGHT_PARAM，   //右半视场 参数上注
};

