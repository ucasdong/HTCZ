#pragma once

#include <QObject>

typedef unsigned char BYTE;
//公共
#define IMAGE_WIDHT         1024
#define GRAB_HEIGHT         102400
#define CAMERA_ID_INDEX     0
#define CHANNEL_INDEX       5
#define HIGHTORLOW          0
#define GRAPHICSHOWTIME     2     //图像刷新数据次数后显示

#define CmdLength 16     //遥控指令有效长度
#define CCSDS_PKGSIZE       1024        //CCSDS每包长度
#define CCSDS_PKGDATASIZE   1012        //CCSDS有效数据长度
#define CCSDS_PKGSTARTSIZE  10          //CCSDS数据头长度
#define BAUDRATE  115200

#define FZDATA_SHOWTIME 2
#define PIXLENGTH 2

//按钮存储信息
struct User : QObjectUserData
{
	QString nID;          // ID
	int iFlag;        // 当前状态 0:正常状态，1：最大化状态
	QString strName;  // 名称
	QString strParentID;
	int iStartBit;    // 遥控指令中起始位
	int iValidBits;   // 遥控指令中有效位
};

class Commonh : public QObject
{
	Q_OBJECT

public:
	Commonh(QObject *parent);
	~Commonh();
};
