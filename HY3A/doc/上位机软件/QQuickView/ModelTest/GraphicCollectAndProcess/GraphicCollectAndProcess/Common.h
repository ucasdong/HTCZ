#pragma once

#include <QObject>

typedef unsigned char BYTE;
//����
#define IMAGE_WIDHT         1024
#define GRAB_HEIGHT         102400
#define CAMERA_ID_INDEX     0
#define CHANNEL_INDEX       5
#define HIGHTORLOW          0
#define GRAPHICSHOWTIME     2     //ͼ��ˢ�����ݴ�������ʾ

#define CmdLength 16     //ң��ָ����Ч����
#define CCSDS_PKGSIZE       1024        //CCSDSÿ������
#define CCSDS_PKGDATASIZE   1012        //CCSDS��Ч���ݳ���
#define CCSDS_PKGSTARTSIZE  10          //CCSDS����ͷ����
#define BAUDRATE  115200

#define FZDATA_SHOWTIME 2
#define PIXLENGTH 2

//��ť�洢��Ϣ
struct User : QObjectUserData
{
	QString nID;          // ID
	int iFlag;        // ��ǰ״̬ 0:����״̬��1�����״̬
	QString strName;  // ����
	QString strParentID;
	int iStartBit;    // ң��ָ������ʼλ
	int iValidBits;   // ң��ָ������Чλ
};

class Commonh : public QObject
{
	Q_OBJECT

public:
	Commonh(QObject *parent);
	~Commonh();
};
