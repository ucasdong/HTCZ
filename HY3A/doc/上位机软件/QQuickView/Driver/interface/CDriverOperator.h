#pragma once

#include "CDriverData.h"
#include "../factory/CDALSAFactory.h"
#include "../factory/C1500Factory.h"
 

typedef void(*pRecvBufferOperator)(CDriverBuffer *pBuffer, void *pVoid);

enum 
{
	DRIVER_TYPE_DALSA = 0,
	DRIVER_TYPE_1500
};

enum emSaveType
{
	SAVE_ORIGINAL = 0,
	SAVE_DATA_TRANSFER
};

class CDriverOperator
{
public:
	CDriverOperator();
	virtual ~CDriverOperator();
	 
/////�����ӿ�/////	
public:
	void			RegisterRecvBufferCallback(pRecvBufferOperator pFunc, void *pVoid);

/////���ܽӿ�/////
	int				InitOperator(std::string strJson);
	bool			StartGrab();
	bool			StopGrab();
	bool			GetGrabStatus();

	bool			StartPlaybackGrab(char *cPath);
	bool			StopPlaybackGrab();
	bool			StartSave(emSaveType nSaveType, const char* szSaveFilePath);
	bool			StopSave(emSaveType nSaveType);

	bool			GetCardStatus(int &nStatus); //λ1�������ź�  λ2������ʱ��1�ź�   λ3������ʱ��2�ź�   λ4������ʱ��3�ź�  
												 //λ5��������Ƿ��е�Դ����   λ6���������Դ�Ƿ����   λ7����Ԫ�����Ƿ���Ч


	bool			SetMode(int nCropWidth, int nCropHeight);
	bool			ResetConfig(int nCropWidth, int nCropHeight);

	CDriverBuffer*	GetDriverBuffer();

/////���ýӿ�/////
public:
	void			PlaybackGrabThreadDispose();

private:
	bool			InitDriver();
	bool			DestroyDriver();
	bool			InitConfig(std::string strJson);
	bool			InitBuffer(unsigned int nLen);		
	bool			DestroyBuffer();	

private:
	int						m_nDriverType;
	std::string				m_strCcfFilePath;
	bool					m_bPlaybackGrab;
	std::string				m_strPlaybackPath;
	CDriverBuffer*			m_pDriverBuffer;
	CConfigData*			m_pConfigData;	
	BYTE*					m_pByPlaybackData;
	FILE*					m_pSaveFile;
	int						m_nSaveIndex;

public:
	bool					m_bSaving;
	void*					m_pCallbackPoint;

private: 
	CDALSADriverFactory		m_dalsaFactory;
	C1500DriverFactory		m_1500Factory; 

	CDALSADriver			*m_pDALSADriver;
	C1500Driver				*m_p1500Driver; 
	
	CDataTransferAnalysis	*m_pDataAnalysis;

public:
	pRecvBufferOperator		m_pfRevcBufOperator; 

};