#pragma once

#include "CDriver.h"
#include "../interface/CDriverData.h"
#include "../interface/CDataTransmission.h"
#include <SapClassBasic.h>


typedef void(*pRecvBufferNotify)();

//之后写配置文件中
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

/////设置/////
	void				RegRecvBufNotifyCallback(pRecvBufferNotify pFunc);
	void				SetBufData(CDriverBuffer *pBuffer);
	void				SplitBuffer();

/////采集/////
	bool				StartGrab();
	bool				StopGrab();
	bool				GetGrabStatus();
	int					GetCardStatus();

/////回放/////
	void				StartPlayback(BYTE* byData);

/////存储/////
	bool				StartSaveOrig();
	bool				StopSaveOrig();
	bool				StartSaveDataTrans();
	bool				StopSaveDataTrans();
	void				SetSaveOrigFilePath(const char *pPath);
	void				SetSaveDataTransFilePath(const char *pPath);

/////模式设置/////
	bool				SwitchMode(int nCropWidth, int nCropHeight);

private:
	static void			XferCallback(SapXferCallbackInfo *pInfo);
	bool				CreateObjects();
	bool				DestroyObjects();

/////线程/////
	static unsigned __stdcall  StatusThread(void *params);
	static unsigned __stdcall  NotifyThread(void *params);
	static unsigned __stdcall  SaveOrigBufferThread(void *params);
	static unsigned __stdcall  SaveDataTransBufferThread(void *params);

private:
//DALSA数据
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

//线程相关
	bool				m_bNotifyThreadRun;						//通知线程是否运行
	bool				m_bSaveOrigBufThreadRun;				//存储原始文件线程是否运行
	bool				m_bSaveDataTransBufThreadRun;			//存储数传文件线程是否运行
	bool				m_bStatusThreadRun;						//状态线程是否运行
	pRecvBufferNotify	m_pfRevcBufNotify;						//接收数据回调

//存储相关
	FILE				*m_fileOrig;							//存储原始文件
	FILE				*m_fileDataTrans;						//存储数传文件
	std::string			m_strSaveOrigFilePath;					//原始文件路径
	std::string			m_strSaveDataTransFilePath;				//存储文件路径
	bool				m_bSaveOrigBufStart;					//是否开始存储原始文件
	bool				m_bSaveDataTransBufStart;				//是否开始存储数传文件
	int					m_nSaveType;							//存储类型   0:原始   //1:数传格式


//Buffer相关
	CDriverBuffer		*m_pDriverBuffer;
	CDataTransferAnalysis	*m_pDataTransferAnalysis;
	char				*m_pBufData;
};
