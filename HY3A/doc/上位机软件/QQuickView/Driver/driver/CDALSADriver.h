#pragma once

#include "CDriver.h"
#include "../interface/CDriverData.h"
#include "../interface/CDataTransmission.h"
#include <SapClassBasic.h>


typedef void(*pRecvBufferNotify)();

//֮��д�����ļ���
enum emBufCount
{
	DEFAULT_COUNT = 2,
	BUFFER_COUNT1 = 10,
	BUFFER_COUNT2 = 20
};

class CDALSADriver : public CDriver
{
public:
	CDALSADriver();
	virtual ~CDALSADriver();

public:
	virtual bool		InitDriver();
	virtual void		DestroyDriver();
	virtual int 		InitThread();
	virtual bool		DestroyThread();
	virtual void		SetCCF(const char *pPath);
	virtual void		SetBufferCount(int nCount);

/////����/////
	void				RegRecvBufNotifyCallback(pRecvBufferNotify pFunc);
	void				SetBufData(CDriverBuffer *pBuffer);
	void				SplitBuffer();

/////�ɼ�/////
	bool				StartGrab();
	bool				StopGrab();
	bool				GetGrabStatus();
	int					GetCardStatus();

/////�ط�/////
	void				StartPlayback(BYTE* byData);

/////�洢/////
	bool				StartSaveOrig();
	bool				StopSaveOrig();
	bool				StartSaveDataTrans();
	bool				StopSaveDataTrans();
	void				SetSaveOrigFilePath(const char *pPath);
	void				SetSaveDataTransFilePath(const char *pPath);

/////ģʽ����/////
	bool				SwitchMode(int nCropWidth, int nCropHeight);

private:
	static void			XferCallback(SapXferCallbackInfo *pInfo);
	bool				CreateObjects();
	bool				DestroyObjects();

/////�߳�/////
	static unsigned __stdcall  StatusThread(void *params);
	static unsigned __stdcall  NotifyThread(void *params);
	static unsigned __stdcall  SaveOrigBufferThread(void *params);
	static unsigned __stdcall  SaveDataTransBufferThread(void *params);

private:
//DALSA����
	SapAcquisition		*m_Acq;
	SapBuffer			*m_Buffers;
	SapTransfer			*m_Xfer;
	SapView				*m_View;

	 
private:
	std::string			m_strCCFPath;
	bool				m_bGrabRun;
	bool				m_bFirstGrab;
	bool				m_bGrabStatus;
	int					m_nBufferCount;	 
	int					m_nPlaybackDataCount;

//�߳����
	bool				m_bNotifyThreadRun;						//֪ͨ�߳��Ƿ�����
	bool				m_bSaveOrigBufThreadRun;				//�洢ԭʼ�ļ��߳��Ƿ�����
	bool				m_bSaveDataTransBufThreadRun;			//�洢�����ļ��߳��Ƿ�����
	bool				m_bStatusThreadRun;						//״̬�߳��Ƿ�����
	pRecvBufferNotify	m_pfRevcBufNotify;						//�������ݻص�

//�洢���
	FILE				*m_fileOrig;							//�洢ԭʼ�ļ�
	FILE				*m_fileDataTrans;						//�洢�����ļ�
	std::string			m_strSaveOrigFilePath;					//ԭʼ�ļ�·��
	std::string			m_strSaveDataTransFilePath;				//�洢�ļ�·��
	bool				m_bSaveOrigBufStart;					//�Ƿ�ʼ�洢ԭʼ�ļ�
	bool				m_bSaveDataTransBufStart;				//�Ƿ�ʼ�洢�����ļ�
	int					m_nSaveType;							//�洢����   0:ԭʼ   //1:������ʽ


//Buffer���
	CDriverBuffer		*m_pDriverBuffer;
	CDataTransferAnalysis	*m_pDataTransferAnalysis;
	char				*m_pBufData;
};
