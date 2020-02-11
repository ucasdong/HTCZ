#include <stdio.h>
#include <tchar.h>
#include <process.h>
#include "CDALSADriver.h"
#include "../common/CIniReader.h"
#include "../common/CIniWriter.h"


void CDALSADriver::XferCallback(SapXferCallbackInfo *pInfo)
{
 	CDALSADriver *pThis = (CDALSADriver *)pInfo->GetContext();
 	if (pInfo->IsTrash())
 	{
		DWORD dwError = GetLastError();
		TCHAR szOutput[255] = {0};
		_stprintf_s(szOutput, _T( "Frames acquired in trash buffer: %d\n" ), pInfo->GetEventCount());
		OutputDebugString(szOutput);
 	}
 	else  // Refresh view
 	{
#ifdef  THREAD_AFFINITY
		HANDLE hthread = ::GetCurrentThread(); // 本进程的HANDLE
		::SetThreadAffinityMask(hthread, MAIN_GRAB_BASE_THREAD1);
#endif 

		if (!pThis->m_bGrabRun)
			return;

		pThis->SplitBuffer();
 	}
}

void CDALSADriver::SplitBuffer()
{
	BYTE *pSrcData;
	pSrcData = (unsigned char* )m_pBufData;
	m_Buffers->GetAddress((void**)&pSrcData);

	BYTE *pDstData = NULL;

	int nWriteIndex = m_pDriverBuffer->GetWriteIndex();
	int nReadIndex = m_pDriverBuffer->GetReadIndex();

	m_pDriverBuffer->GetAddress(nWriteIndex, (void**)&pDstData);
//	if (!m_bFirstGrab && nWriteIndex == nReadIndex)
//	{//如果不是第一次写入数据且此时读写Index相同的话，证明写buffer已经套了读buffer一个循环，此时则不再进行写数据操作
//		return;
//	}

	m_pDataTransferAnalysis->GetPackageInfo(m_pDriverBuffer->GetUnpacker());
	m_pDataTransferAnalysis->AnalysisData(pDstData, pSrcData);
	m_pDriverBuffer->WriteNext();
	
	if (m_bFirstGrab)
	{
		m_bFirstGrab = false;
	}
}


unsigned CDALSADriver::NotifyThread(void *params)
{
	CDALSADriver *pThis = (CDALSADriver *)params;
	if (pThis == NULL)
		return 0;

	while (pThis->m_bNotifyThreadRun)
	{
		if ( pThis->m_bGrabRun && (pThis->m_pDriverBuffer->GetReadIndex() != pThis->m_pDriverBuffer->GetWriteIndex()) )
		{
			pThis->m_pfRevcBufNotify();
			pThis->m_pDriverBuffer->ReadNext();
			if (!pThis->m_bGrabRun)
			{//在ReadBuffer执行完后情况DriverBuffer
				pThis->m_pDriverBuffer->ClearBuffer();
			}
		}
		Sleep(50);
	}

	return 1;
}

unsigned CDALSADriver::SaveOrigBufferThread(void *params)
{
	CDALSADriver *pThis = (CDALSADriver *)params;
	if (pThis == NULL)
		return 0;

	while (pThis->m_bSaveOrigBufThreadRun)
	{
		if (pThis->m_bSaveOrigBufStart)
		{
			if (pThis->m_fileOrig != NULL)
			{
				fclose(pThis->m_fileOrig);
				pThis->m_fileOrig = NULL;
			}
			fopen_s(&pThis->m_fileOrig, pThis->m_strSaveOrigFilePath.c_str(), "wb");
			if (pThis->m_fileOrig == NULL)
			{
				int nError = GetLastError();
			}

			int nSize = pThis->m_Buffers->GetWidth() * pThis->m_Buffers->GetHeight() * pThis->m_Buffers->GetBytesPerPixel();
			
			BYTE *pSrcData;
			//BYTE *pSrcData = new BYTE[nSize];
			//memset(pSrcData, 0, nSize);

			while (pThis->m_bSaveOrigBufStart)
			{
				pThis->m_Buffers->GetAddress((void**)&pSrcData);
				size_t sz = fwrite(pSrcData, 1, nSize, pThis->m_fileOrig);
	
				Sleep(10);
			}
			//delete [] pSrcData;
			//pSrcData = NULL;
			fclose(pThis->m_fileOrig);
			pThis->m_fileOrig = NULL;
		}
		Sleep(10);
	}

	return 1;
}


unsigned CDALSADriver::SaveDataTransBufferThread(void *params)
{
	CDALSADriver *pThis = (CDALSADriver *)params;
	if (pThis == NULL)
		return 0;

	while (pThis->m_bSaveDataTransBufThreadRun)
	{
		if (pThis->m_bSaveDataTransBufStart)
		{
			if (pThis->m_fileDataTrans != NULL)
			{
				fclose(pThis->m_fileDataTrans);
				pThis->m_fileDataTrans = NULL;
			}
			fopen_s(&pThis->m_fileDataTrans, pThis->m_strSaveDataTransFilePath.c_str(), "wb");
			if (pThis->m_fileDataTrans == NULL)
			{
				int nError = GetLastError();
			}

			int nSize = pThis->m_pDriverBuffer->GetWidth() * pThis->m_pDriverBuffer->GetHeight() * pThis->m_pDriverBuffer->GetBytesPerPixel();

			BYTE *pSrcData;
			bool bFirst(true);
			int nIndex = 0;
			while (pThis->m_bSaveDataTransBufStart)
			{
				if (!bFirst)
				{
					if (nIndex == pThis->m_pDriverBuffer->GetReadIndex())
						continue;
				}
				nIndex = pThis->m_pDriverBuffer->GetReadIndex();
				if (pThis->m_pDriverBuffer->GetAddress(nIndex, (void**)&pSrcData))
				{
					size_t sz = fwrite(pSrcData, 1, nSize, pThis->m_fileDataTrans);
				}
				bFirst = false;

				Sleep(10);
			}
			fclose(pThis->m_fileDataTrans);
			pThis->m_fileDataTrans = NULL;
		}
		Sleep(10);
	}

	return 1;
}


unsigned CDALSADriver::StatusThread(void *params)
{
	CDALSADriver *pThis = (CDALSADriver *)params;
	if (pThis == NULL)
		return 0;
	static int nLastWriteIndex = 0;
	static int nSameIndexTime = 0;
	while (pThis->m_bStatusThreadRun)
	{
		if ( pThis->m_bGrabRun )
		{
			int nWriteIndex = pThis->m_pDriverBuffer->GetWriteIndex();
			if (nWriteIndex == nLastWriteIndex)
			{
				nSameIndexTime++;
			}
			else
			{
				nLastWriteIndex = nWriteIndex;
				nSameIndexTime = 0;
			}

			if (nSameIndexTime > 5)
			{
				pThis->m_bGrabStatus = false;
			}
			else
			{
				pThis->m_bGrabStatus = true;
			}
		}
		else
		{
			pThis->m_bGrabStatus = false;
		}

		Sleep(1000);
	}

	return 1;
}


///////////////////////////////////////////////


CDALSADriver::CDALSADriver()
{
	m_nBufferCount			= 0;
	m_pBufData				= NULL;
	m_Acq					= NULL;
	m_Buffers				= NULL;
	m_Xfer					= NULL;
	m_View					= NULL;
	m_strCCFPath			= "";
	m_bGrabRun				= false;
	m_nPlaybackDataCount	= 0;
	m_nSaveType				= 0;
	m_strSaveOrigFilePath	= "";
	m_strSaveDataTransFilePath	= "";
	m_bFirstGrab			= true;
	m_bNotifyThreadRun		= false;
	m_bSaveOrigBufThreadRun	= false;
	m_bSaveOrigBufStart		= false;
	m_fileOrig				= NULL;
	m_fileDataTrans			= NULL;
	m_bSaveDataTransBufThreadRun = false;
	m_bSaveDataTransBufStart = false;
	m_bStatusThreadRun		= false;
	m_bGrabStatus			= false;
	m_pDataTransferAnalysis = new CDataTransferAnalysis;
	
}

CDALSADriver::~CDALSADriver()
{
	StopGrab();
	StopSaveOrig();
	StopSaveDataTrans();
	DestroyThread();
	DestroyObjects();

	// Delete all objects
	if (m_Xfer)			delete m_Xfer; 
	if (m_View)			delete m_View; 
	if (m_Buffers)		delete m_Buffers; 
	if (m_Acq)			delete m_Acq; 

	if (m_pDataTransferAnalysis != NULL)
	{
		delete m_pDataTransferAnalysis;
		m_pDataTransferAnalysis = NULL;
	}
	if (m_pBufData != NULL)
	{
		delete [] m_pBufData;
		m_pBufData = NULL;
	}
	if (m_fileOrig != NULL)
	{
		fclose(m_fileOrig);
		m_fileOrig = NULL;
	}
	if (m_fileDataTrans != NULL)
	{
		fclose(m_fileDataTrans);
		m_fileDataTrans = NULL;
	}
}

void CDALSADriver::SetCCF(const char *pPath)
{
	m_strCCFPath = pPath;
}

void CDALSADriver::SetBufferCount(int nCount)
{
	m_nBufferCount = nCount;
}

void CDALSADriver::SetBufData(CDriverBuffer *pBuffer)
{
	m_pDriverBuffer = pBuffer;
}

void CDALSADriver::RegRecvBufNotifyCallback(pRecvBufferNotify pFunc)
{
	m_pfRevcBufNotify = pFunc;
}

bool CDALSADriver::InitDriver()
{
	const char szServerName[] = "Xtium-CL_MX4_1";
	SapLocation *m_loc;
	m_loc		= new SapLocation(szServerName, 0);
	m_Acq		= new SapAcquisition(*m_loc, m_strCCFPath.c_str());
	m_Buffers	= new SapBufferWithTrash(m_nBufferCount, m_Acq);
	m_Xfer		= new SapAcqToBuf(m_Acq, m_Buffers, XferCallback, this);

	// Define other objects
	m_View		= new SapView(m_Buffers);

	// Create all objects  //创建4个对象SapAcquisition SapBuffer SapTransfer SapView
	if (!CreateObjects()) 
	{ 
		return false; 
	}

	int nWidth = m_Buffers->GetWidth();
	int nHeight = m_Buffers->GetHeight(); 
	int nBytes = m_Buffers->GetBytesPerPixel();
	int nBufferSize = nWidth * nHeight * nBytes;
	if((nBufferSize % 512) != 0)
		nBufferSize = (nBufferSize / 512) * 512 + 512;
	m_pBufData = (char*)malloc(nBufferSize);
	memset(m_pBufData, 0, nBufferSize);

	if (m_loc)			delete m_loc;

	return true;

}

bool CDALSADriver::CreateObjects()
{
	// Create acquisition object
	if (m_Acq && !*m_Acq && !m_Acq->Create())
	{
		OutputDebugStringA(_T("系统没有找到DALSA板卡!"));  //后期加入日志
		DestroyObjects();
		return false;
	}

	// Create buffer object
	if (m_Buffers && !*m_Buffers)
	{
		if(!m_Buffers->Create())
		{
			OutputDebugStringA(_T("创建DALSA数据缓存失败!")); 
			DestroyObjects();
			return false;
		}
		// Clear all buffers
		m_Buffers->Clear();
	}

	// Create view object
	if (m_View && !*m_View && !m_View->Create())
	{
		OutputDebugStringA(_T("创建DALSA View对象失败!")); 
		DestroyObjects();
		return false;
	}

	// Create transfer object
	if (m_Xfer && !*m_Xfer && !m_Xfer->Create())
	{
		OutputDebugStringA(_T("创建DALSA传输对象失败!")); 
		DestroyObjects();
		return false;
	}

	return true;
}


bool CDALSADriver::DestroyObjects()
{
	// Destroy transfer object
	if (m_Xfer && *m_Xfer) m_Xfer->Destroy();

	// Destroy view object
	if (m_View && *m_View) m_View->Destroy();

	// Destroy buffer object
	if (m_Buffers && *m_Buffers) m_Buffers->Destroy();

	// Destroy acquisition object
	if (m_Acq && *m_Acq) m_Acq->Destroy();

	return true;
}

void CDALSADriver::DestroyDriver()
{
	StopGrab();
	StopSaveOrig();
	StopSaveDataTrans();
	DestroyThread();
	DestroyObjects();

	// Delete all objects
	if (m_Xfer)			delete m_Xfer; 
	if (m_View)			delete m_View; 
	if (m_Buffers)		delete m_Buffers; 
	if (m_Acq)			delete m_Acq; 

	if (m_pBufData != NULL)
	{
		delete [] m_pBufData;
		m_pBufData = NULL;
	}
}


int CDALSADriver::InitThread()
{
	UINT32 threadNotifyId;
	HANDLE hNotifyThread = (HANDLE)_beginthreadex(NULL, 0, NotifyThread, this, CREATE_SUSPENDED, &threadNotifyId);
	if (hNotifyThread == NULL){
		return -1;
	}
	SetThreadAffinityMask(hNotifyThread, 1 << MAIN_NOTIFY_THREAD);
	ResumeThread(hNotifyThread);
	m_bNotifyThreadRun = true;


	UINT32 threadSaveOrigBufferId;
	HANDLE hSaveOrigBufferThread = (HANDLE)_beginthreadex(NULL, 0, SaveOrigBufferThread, this, CREATE_SUSPENDED, &threadSaveOrigBufferId);
	if (hSaveOrigBufferThread == NULL){
		return -2;
	}
	SetThreadAffinityMask(hSaveOrigBufferThread, 1 << SAVE_ORIG_THREAD);
	ResumeThread(hSaveOrigBufferThread);
	m_bSaveOrigBufThreadRun = true;


	UINT32 threadSaveDataTransBufferId;
	HANDLE hSaveDataTransBufferThread = (HANDLE)_beginthreadex(NULL, 0, SaveDataTransBufferThread, this, CREATE_SUSPENDED, &threadSaveDataTransBufferId);
	if (hSaveDataTransBufferThread == NULL){
		return -3;
	}
	SetThreadAffinityMask(hSaveDataTransBufferThread, 1 << SAVE_DATA_TRANS_THREAD);
	ResumeThread(hSaveDataTransBufferThread);
	m_bSaveDataTransBufThreadRun = true;


	UINT32 threadStatusBufferId;
	HANDLE hStatusThread = (HANDLE)_beginthreadex(NULL, 0, StatusThread, this, CREATE_SUSPENDED, &threadStatusBufferId);
	if (hStatusThread == NULL){
		return -4;
	}
	SetThreadAffinityMask(hStatusThread, 1 << STATUS_THREAD);
	ResumeThread(hStatusThread);
	m_bStatusThreadRun = true;

	return 1;
}

bool CDALSADriver::DestroyThread()
{
	m_bNotifyThreadRun = false;
	m_bSaveOrigBufThreadRun = false;
	m_bSaveDataTransBufThreadRun = false;
	m_bStatusThreadRun = false;

	return true;
}

bool CDALSADriver::StartGrab() 
{
	if(!m_Xfer->Grab())
	{
		OutputDebugStringA(_T("板卡启动出错,请重新启动系统!")); 
		return false;
	}
	m_bGrabRun = true;

	return true;
}


bool CDALSADriver::StopGrab()
{
	m_bGrabRun = false;
	m_bFirstGrab = true;
	if(!m_Xfer->Freeze())
	{
		OutputDebugStringA(_T("采集停止失败!")); 
		return false;
	}
	return true;
}

bool CDALSADriver::GetGrabStatus()
{
	return m_bGrabStatus;
}

bool CDALSADriver::StartSaveOrig()
{
	m_bSaveOrigBufStart = true;
	return true;
}

bool CDALSADriver::StopSaveOrig()
{
	m_bSaveOrigBufStart = false;
	return true;
}

bool CDALSADriver::StartSaveDataTrans()
{
	m_bSaveDataTransBufStart = true;
	return true;
}

bool CDALSADriver::StopSaveDataTrans()
{
	m_bSaveDataTransBufStart = false;
	return true;
}
 
void CDALSADriver::StartPlayback(BYTE* byData)
{
	m_Buffers->WriteRect(0, 0, m_Buffers->GetWidth(), m_Buffers->GetHeight(), byData);
	SplitBuffer();
}


void CDALSADriver::SetSaveOrigFilePath(const char *pPath)
{
	m_strSaveOrigFilePath = pPath;
}

void CDALSADriver::SetSaveDataTransFilePath(const char *pPath)
{
	m_strSaveDataTransFilePath = pPath;
}

bool CDALSADriver::SwitchMode(int nCropWidth, int nCropHeight)
{
	std::string strPath;
	TCHAR chPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, chPath, MAX_PATH);
	strPath = chPath;
	int len = strPath.rfind('\\');
	strPath = strPath.substr(0, len);
	strPath += "\\";
	strPath += m_strCCFPath;

	int nCropHeight1 = 0, nCropWidth1 = 0, nScaleHorizontal1 = 0, nScaleVertical1 = 0, nVerticalActive1 = 0; 
	CIniReader iniReader(strPath.c_str());
	nCropHeight1 = iniReader.ReadInteger(TEXT("Stream Conditioning"), TEXT("Crop Height"), 0);
	nCropWidth1 = iniReader.ReadInteger(TEXT("Stream Conditioning"), TEXT("Crop Width"), 0);
	nScaleHorizontal1 = iniReader.ReadInteger(TEXT("Stream Conditioning"), TEXT("Scale Horizontal"), 0);
	nScaleVertical1 = iniReader.ReadInteger(TEXT("Stream Conditioning"), TEXT("Scale Vertical"), 0);
	nVerticalActive1 = iniReader.ReadInteger(TEXT("Signal Timings"), TEXT("Vertical Active"), 0);
	
	CIniWriter iniWriter(strPath.c_str());
	iniWriter.WriteInteger(TEXT("Stream Conditioning"), TEXT("Crop Height"), nCropHeight);
	iniWriter.WriteInteger(TEXT("Stream Conditioning"), TEXT("Crop Width"), nCropWidth);
	iniWriter.WriteInteger(TEXT("Stream Conditioning"), TEXT("Scale Horizontal"), nCropWidth);
	iniWriter.WriteInteger(TEXT("Stream Conditioning"), TEXT("Scale Vertical"), nCropHeight);
	iniWriter.WriteInteger(TEXT("Signal Timings"), TEXT("Vertical Active"), nCropHeight);

	return true;
}

int CDALSADriver::GetCardStatus()
{
	//SapAcquisition::SignalHSyncPresent
	//SapAcquisition::SignalVSyncPresent
	//SapAcquisition::SignalPixelClkPresent 
	//SapAcquisition::SignalPixelClk1Present
	//SapAcquisition::SignalPixelClk2Present
	//SapAcquisition::SignalPixelClk3Present
	//SapAcquisition: SignalPixelClkAllPresent
	//SapAcquisition: SignalChromaPresent
	//SapAcquisition: SignalHSyncLock
	//SapAcquisition: SignalVSyncLock
	//SapAcquisition: SignalPowerPresent
	//SapAcquisition: SignalPoCLActive
	//SapAcquisition: SignalPixelLinkLock
	unsigned short usCardStatus[7] = {0};
	BOOL bStatus[7] = {FALSE};
	if (m_Acq)
	{
		bStatus[0] = m_Acq->IsSignalStatusAvailable(SapAcquisition::SignalNone);
		usCardStatus[0] = bStatus[0];
		bStatus[1] = m_Acq->IsSignalStatusAvailable(SapAcquisition::SignalPixelClk1Present);
		usCardStatus[1] = bStatus[1] << 1;
		bStatus[2] = m_Acq->IsSignalStatusAvailable(SapAcquisition::SignalPixelClk2Present);
		usCardStatus[2] = bStatus[2] << 2;
		bStatus[3] = m_Acq->IsSignalStatusAvailable(SapAcquisition::SignalPixelClk3Present);
		usCardStatus[3] = bStatus[3] << 3;
		bStatus[4] = m_Acq->IsSignalStatusAvailable(SapAcquisition::SignalPowerPresent);
		usCardStatus[4] = bStatus[4] << 4;
		bStatus[5] = m_Acq->IsSignalStatusAvailable(SapAcquisition::SignalPoCLActive);
		usCardStatus[5] = bStatus[5] << 5;
		bStatus[6] = m_Acq->IsSignalStatusAvailable(SapAcquisition::SignalPixelLinkLock);
		usCardStatus[6] = bStatus[6] << 6;
	}

	int nStatus = 0;
	for(int i = 0 ; i < 7; i++)
		nStatus += usCardStatus[i];

	return nStatus;
}