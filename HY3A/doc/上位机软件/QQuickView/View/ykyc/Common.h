#pragma once

#include <QObject>
typedef unsigned char BYTE;
using namespace std;

struct CmpByKeyLength {
	bool operator()(const string& k1, const string& k2)const {
		return true;
	}
};


#define   THREAD_AFFINITY
#define   MAIN_PROCESSING_CPU        1
#define   MAIN_GRAB_BASE_THREAD1     1<<1
#define   MAIN_GRAB_BASE_THREAD2     1<<2
#define   MAIN_GRAB_SPLITTER_THREAD1 1<<3
#define   MAIN_GRAB_SPLITTER_THREAD2 1<<4
#define   MAIN_COMM_THREAD           1<<5
#define   MAIN_OTHER_THREAD          1<<6
#define   MAIN_STATUS_THREAD         1<<7
#define   MTF_THREAD                 1<<8


//公共部分配置
#define IMAGE_WIDHT         1024
#define GRAB_HEIGHT         102400
#define CCSDS_PKGSIZE       1024        //CCSDS每包长度
#define CCSDS_PKGDATASIZE   1012        //CCSDS有效数据长度
#define CCSDS_PKGSTARTSIZE  10          //CCSDS数据头长度
#define BUFFER_COUNT        5           //接收数据缓冲区个数
#define GRAPHIC_DATAHORL    0           //图像数据是高8位或者低8位
#define FZDATA_SHOWTIME     5           //辅助数据显示间隔
#define GRAPHIC_SHOWTIME    5           //图像刷新显示间隔

//高分可见
#define GFKJ_ARCVDATA_COUNT      1024                  //每次传来的帧数
#define GFKJ_PIXLENNTH           2                     //每个像素点占的字节数	
#define GFKJ_ALLDATA_COUNT       5168                  //每次传来的一帧数据长度
#define GFKJ_FZ_COUNT            48                    //每帧辅助数据长度
#define GFKJ_PERSENDLINE         1                     //每帧数据发来的图像行数
#define GFKJ_P_PERLINECOUNT      5120                  //每行图像数据长度
#define GFKJ_B_PERLINECOUNT      1280                  //B谱段每行图像数据长度                

//遥控遥测
#define CmdLength 10     //遥控指令有效长度
#define BAUDRATE  115200 
//#define HIGHTORLOW 1


//按钮存储信息
struct User : QObjectUserData
{
	int nID;          // ID
	int iFlag;        // 当前状态 0:正常状态，1：最大化状态
	QString strName;  // 名称
	QString strParentName;
	int iStartBit;    // 遥控指令中起始位
	int iValidBits;   // 遥控指令中有效位
	int base;		  // 进制
};

struct OneData  //可扩展添加其他内容
{
	BYTE* dataShow;  //数据内容
	int iLen;        //数据长度
};

class Commonh : public QObject
{
	Q_OBJECT

public:
	Commonh(QObject *parent);
	~Commonh();
};
