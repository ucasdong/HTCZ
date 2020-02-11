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


//������������
#define IMAGE_WIDHT         1024
#define GRAB_HEIGHT         102400
#define CCSDS_PKGSIZE       1024        //CCSDSÿ������
#define CCSDS_PKGDATASIZE   1012        //CCSDS��Ч���ݳ���
#define CCSDS_PKGSTARTSIZE  10          //CCSDS����ͷ����
#define BUFFER_COUNT        5           //�������ݻ���������
#define GRAPHIC_DATAHORL    0           //ͼ�������Ǹ�8λ���ߵ�8λ
#define FZDATA_SHOWTIME     5           //����������ʾ���
#define GRAPHIC_SHOWTIME    5           //ͼ��ˢ����ʾ���

//�߷ֿɼ�
#define GFKJ_ARCVDATA_COUNT      1024                  //ÿ�δ�����֡��
#define GFKJ_PIXLENNTH           2                     //ÿ�����ص�ռ���ֽ���	
#define GFKJ_ALLDATA_COUNT       5168                  //ÿ�δ�����һ֡���ݳ���
#define GFKJ_FZ_COUNT            48                    //ÿ֡�������ݳ���
#define GFKJ_PERSENDLINE         1                     //ÿ֡���ݷ�����ͼ������
#define GFKJ_P_PERLINECOUNT      5120                  //ÿ��ͼ�����ݳ���
#define GFKJ_B_PERLINECOUNT      1280                  //B�׶�ÿ��ͼ�����ݳ���                

//ң��ң��
#define CmdLength 10     //ң��ָ����Ч����
#define BAUDRATE  115200 
//#define HIGHTORLOW 1


//��ť�洢��Ϣ
struct User : QObjectUserData
{
	int nID;          // ID
	int iFlag;        // ��ǰ״̬ 0:����״̬��1�����״̬
	QString strName;  // ����
	QString strParentName;
	int iStartBit;    // ң��ָ������ʼλ
	int iValidBits;   // ң��ָ������Чλ
	int base;		  // ����
};

struct OneData  //����չ�����������
{
	BYTE* dataShow;  //��������
	int iLen;        //���ݳ���
};

class Commonh : public QObject
{
	Q_OBJECT

public:
	Commonh(QObject *parent);
	~Commonh();
};
