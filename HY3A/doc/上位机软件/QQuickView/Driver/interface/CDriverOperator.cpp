#include <stdio.h>
#include <tchar.h>
#include <process.h>
#include "CDriverOperator.h"
#include "../common/cJSON.h"
#include "CDriverDefine.h"

CDriverOperator* g_pDriverOperator = NULL;

void BufferNotifyFunc()
{
	g_pDriverOperator->m_pfRevcBufOperator(g_pDriverOperator->GetDriverBuffer(), g_pDriverOperator->m_pCallbackPoint);
}

CDriverOperator::CDriverOperator()
	: m_nDriverType(0)
	, m_strCcfFilePath(_T(""))
	, m_bPlaybackGrab(false)
	, m_pDALSADriver(NULL)
	, m_p1500Driver(NULL)
	, m_pByPlaybackData(NULL)
	, m_strPlaybackPath(_T(""))
	, m_bSaving(false)
	, m_pSaveFile(NULL)
	, m_nSaveIndex(0)
	, m_pCallbackPoint(NULL)
{
	g_pDriverOperator = this;
	m_pDriverBuffer = new CDriverBuffer;
	m_pConfigData = new CConfigData;
	m_pDataAnalysis = new CDataTransferAnalysis;
}

CDriverOperator::~CDriverOperator()
{
	StopSave(SAVE_ORIGINAL);
	StopSave(SAVE_DATA_TRANSFER);
	StopPlaybackGrab();
	StopGrab();
	DestroyDriver();

	if (m_pDriverBuffer != NULL)
	{
		delete m_pDriverBuffer;
		m_pDriverBuffer = NULL;
	}
	if (m_pByPlaybackData != NULL)
	{
		delete m_pByPlaybackData;
		m_pByPlaybackData = NULL;
	}
	if (m_pConfigData != NULL)
	{
		delete m_pConfigData;
		m_pConfigData = NULL;
	}
	if (m_pDataAnalysis != NULL)
	{
		delete m_pDataAnalysis;
		m_pDataAnalysis = NULL;
	}
	
}

void CDriverOperator::RegisterRecvBufferCallback(pRecvBufferOperator pFunc, void *pVoid)
{
	m_pfRevcBufOperator = pFunc;
	m_pCallbackPoint = pVoid;

	std::string strType(_T(""));
	int nType = m_pConfigData->GetDriverType(strType);
	switch (nType)
	{
	case DRIVER_TYPE_DALSA:
		{
			if (m_pDALSADriver == NULL)
			{
				return;
			}
			m_pDALSADriver->RegRecvBufNotifyCallback(BufferNotifyFunc);
		}
		break;
	case DRIVER_TYPE_1500:
		{

		}
		break;
	default:
		{
		}
	}
}

int CDriverOperator::InitOperator(std::string strJson)
{	
	if (!InitConfig(strJson))
	{
		return -1;
	}

	int nWidth = m_pDriverBuffer->GetWidth();
	int nHeight = m_pDriverBuffer->GetHeight();
	int nBytes = m_pDriverBuffer->GetBytesPerPixel();

	if (!InitBuffer(nWidth*nHeight*nBytes))
	{
		return -2;
	}
	if (!InitDriver())
	{
		return -3;
	}

	return 1;
}

bool CDriverOperator::InitConfig(std::string strJson)
{
	//strJson = "	{ \"Type\":\"DALSA\", \"CCFPath\":\"HY3.ccf\", \"OrigWidth\":2304, \"OrigHeight\":3534, \
	//		  \"ImgWidth\":860, \"ImgHeight\":3534, \"PixelBytes\":2, \"BufferCount\":2, \"BufferCacheNum\":5, \"PackageHeadLen\":0, \"PackageTotalLen\":8142336 }  ";

	if (strJson == "")
	{
		return false;
	}

	////½âÎöjson
	cJSON* root = cJSON_Parse(strJson.c_str());
	cJSON* itemType = cJSON_GetObjectItem(root, "Type");
	cJSON* itemPath = cJSON_GetObjectItem(root, "CCFPath");
	cJSON* itemOrigWidth = cJSON_GetObjectItem(root, "OrigWidth");
	cJSON* itemOrigHeight = cJSON_GetObjectItem(root, "OrigHeight");
	cJSON* itemImgWidth = cJSON_GetObjectItem(root, "ImgWidth");
	cJSON* itemImgHeight = cJSON_GetObjectItem(root, "ImgHeight");
	cJSON* itemPixelBytes = cJSON_GetObjectItem(root, "PixelBytes");
	cJSON* itemBufferCount = cJSON_GetObjectItem(root, "BufferCount");
	cJSON* itemBufferCacheNum = cJSON_GetObjectItem(root, "BufferCacheNum");
	cJSON* itemPackageHeadLen = cJSON_GetObjectItem(root, "PackageHeadLen");
	cJSON* itemPackageTotalLen = cJSON_GetObjectItem(root, "PackageTotalLen");

	std::string str1 = itemType->valuestring;
	std::string str2 = itemPath->valuestring;
	int nnn1 = itemOrigWidth->valueint; 
	int nnn2 = itemOrigHeight->valueint; 
	int nnn3 = itemImgWidth->valueint; 
	int nnn4 = itemImgHeight->valueint; 
	int nnn5 = itemPixelBytes->valueint; 
	int nnn6 = itemBufferCount->valueint; 
	int nnn7 = itemBufferCacheNum->valueint; 
	int nnn8 = itemPackageHeadLen->valueint; 
	int nnn9 = itemPackageTotalLen->valueint; 

	if (itemType)			
	{ 
		m_pConfigData->SetDriverType(itemType->valuestring);
	}
	if (itemPath)			
	{	
		m_pConfigData->SetCCFPath(itemPath->valuestring);
	}
	if (itemOrigWidth)		
	{ 	
		m_pConfigData->SetOrigWidth(itemOrigWidth->valueint);
		m_pDriverBuffer->GetUnpacker()->SetRowLen(itemOrigWidth->valueint);
	}
	if (itemOrigHeight)		
	{ 
		m_pConfigData->SetOrigHeight(itemOrigHeight->valueint);
		m_pDriverBuffer->GetUnpacker()->SetColLen(itemOrigHeight->valueint);
	}
	if (itemImgWidth)		
	{
		m_pDriverBuffer->SetWidth(itemImgWidth->valueint/2);
		m_pDriverBuffer->GetUnpacker()->SetRowValidLen(itemImgWidth->valueint);
	}
	if (itemImgHeight)		
	{ 	
		m_pDriverBuffer->SetHeight(itemImgHeight->valueint);
		m_pDriverBuffer->GetUnpacker()->SetColValidLen(itemImgHeight->valueint);
	}
	if (itemPixelBytes)		
	{ 
		m_pDriverBuffer->SetBytesPerPixel(itemPixelBytes->valueint); 
	}
	if (itemBufferCount)	
	{ 
		m_pConfigData->SetBufferCount(itemBufferCount->valueint); 
	}
	if (itemBufferCacheNum)	
	{ 
		m_pDriverBuffer->SetBufCache(itemBufferCacheNum->valueint); 
	}
	if (itemPackageHeadLen)
	{ 	
		m_pDriverBuffer->GetUnpacker()->SetHeadLen(itemPackageHeadLen->valueint);
	}
	if (itemPackageTotalLen)
	{
		m_pDriverBuffer->GetUnpacker()->SetPackageLen(itemPackageTotalLen->valueint);
	}
	if (itemImgWidth && itemImgHeight)
	{
		m_pDriverBuffer->GetUnpacker()->SetValidLen(itemImgWidth->valueint * itemImgHeight->valueint);
	}

	//ÊÍ·Åjson
	cJSON_Delete(root);

	
	m_pDriverBuffer->GetUnpacker()->SetErrorInfo(0);
	m_pDriverBuffer->GetUnpacker()->SetRowAnalysis(true);
	
	///////////////////////////////

	//m_pConfigData->SetDriverType("DALSA");
	//m_pConfigData->SetCCFPath("HY3.ccf");
	//m_pConfigData->SetOrigWidth(BUF_ORIG_WIDTH);
	//m_pConfigData->SetOrigHeight(BUF_ORIG_HEIGHT);

	//m_pDriverBuffer->SetWidth(BUF_WIDTH/2);
	//m_pDriverBuffer->SetHeight(BUF_HEIGHT);
	//m_pDriverBuffer->SetBytesPerPixel(BUF_BYTE); 

	//m_pDriverBuffer->GetUnpacker()->SetPackageLen(PACKAGE_LEN);
	//m_pDriverBuffer->GetUnpacker()->SetHeadLen(0);
	//m_pDriverBuffer->GetUnpacker()->SetErrorInfo(0);
	//m_pDriverBuffer->GetUnpacker()->SetValidLen(BUF_LEN);
	//m_pDriverBuffer->GetUnpacker()->SetRowLen(BUF_ORIG_WIDTH);
	//m_pDriverBuffer->GetUnpacker()->SetColLen(BUF_ORIG_HEIGHT);
	//m_pDriverBuffer->GetUnpacker()->SetRowValidLen(BUF_WIDTH);
	//m_pDriverBuffer->GetUnpacker()->SetColValidLen(BUF_HEIGHT);
	//m_pDriverBuffer->GetUnpacker()->SetRowAnalysis(true);

	return true;
}

bool CDriverOperator::InitDriver()
{
	std::string strType(_T(""));
	int nType = m_pConfigData->GetDriverType(strType);
	switch (nType)
	{
	case DRIVER_TYPE_DALSA:
		{
			m_pDALSADriver = m_dalsaFactory.CreateDriver();
			if (m_pDALSADriver == NULL)
			{
				return false;
			}
			std::string strCCF = m_pConfigData->GetCCFPath();
			m_pDALSADriver->SetCCF(strCCF.c_str());
			int nCount = m_pConfigData->GetBufferCount();
			m_pDALSADriver->SetBufferCount(nCount);
			if (!m_pDALSADriver->InitDriver())
			{
				return false;
			}
			if (1 != m_pDALSADriver->InitThread())
			{
				return false;
			}
		}
		break;
	case DRIVER_TYPE_1500:
		{

		}
		break;
	default:
		{
			return false;
		}
	}
	return true;
}

bool CDriverOperator::DestroyDriver()
{
	std::string strType(_T(""));
	int nType = m_pConfigData->GetDriverType(strType);
	switch (nType)
	{
	case DRIVER_TYPE_DALSA:
		{
			m_dalsaFactory.DestoryDriver();
		}
		break;
	case DRIVER_TYPE_1500:
		{

		}
		break;
	default:
		{
			return false;
		}
	}
	return true;
}


bool CDriverOperator::StartGrab()
{
	std::string strType(_T(""));
	int nType = m_pConfigData->GetDriverType(strType);
	switch (nType)
	{
	case DRIVER_TYPE_DALSA:
		{
			if (m_pDALSADriver == NULL)
			{
				return false;
			}
			m_pDALSADriver->SetBufData(m_pDriverBuffer);
			m_pDALSADriver->StartGrab();
		}
		break;
	case DRIVER_TYPE_1500:
		{

		}
		break;
	default:
		{
			return false;
		}
	}
	return true;

}

bool CDriverOperator::StopGrab()
{
	std::string strType(_T(""));
	int nType = m_pConfigData->GetDriverType(strType);
	switch (nType)
	{
	case DRIVER_TYPE_DALSA:
		{
			if (m_pDALSADriver == NULL)
			{
				return false;
			}
			m_pDALSADriver->StopGrab();
			m_pDALSADriver->SetBufData(NULL);
		}
		break;
	case DRIVER_TYPE_1500:
		{

		}
		break;
	default:
		{
			return false;
		}
	}
	return true;
}

bool CDriverOperator::GetGrabStatus()
{
	std::string strType(_T(""));
	int nType = m_pConfigData->GetDriverType(strType);
	switch (nType)
	{
	case DRIVER_TYPE_DALSA:
		{
			if (m_pDALSADriver == NULL)
			{
				return false;
			}
			return m_pDALSADriver->GetGrabStatus();
		}
		break;
	case DRIVER_TYPE_1500:
		{

		}
		break;
	default:
		{
			return false;
		}
	}
	return true;
}

bool CDriverOperator::StartSave(emSaveType nSaveType, const char* szSaveFilePath)
{
	std::string strType(_T(""));
	int nType = m_pConfigData->GetDriverType(strType);
	switch (nType)
	{
	case DRIVER_TYPE_DALSA:
		{
			if (m_pDALSADriver == NULL)
			{
				return false;
			}
			switch (nSaveType)
			{
			case SAVE_ORIGINAL:
				{
					m_pDALSADriver->SetSaveOrigFilePath(szSaveFilePath);
					m_pDALSADriver->StartSaveOrig();
				}
				break;
			case SAVE_DATA_TRANSFER:
				{
					m_pDALSADriver->SetSaveDataTransFilePath(szSaveFilePath);
					m_pDALSADriver->StartSaveDataTrans();
				}
				break;
			}
		}
		break;
	case DRIVER_TYPE_1500:
		{

		}
		break;
	default:
		{
			return false;
		}
	}

	return true;
}

bool CDriverOperator::StopSave(emSaveType nSaveType)
{
	std::string strType(_T(""));
	int nType = m_pConfigData->GetDriverType(strType);
	switch (nType)
	{
	case DRIVER_TYPE_DALSA:
		{
			if (m_pDALSADriver == NULL)
			{
				return false;
			}
			switch (nSaveType)
			{
			case SAVE_ORIGINAL:
				{
					m_pDALSADriver->StopSaveOrig();
				}
				break;
			case SAVE_DATA_TRANSFER:
				{
					m_pDALSADriver->StopSaveDataTrans();
				}
				break;
			}
		}
		break;
	case DRIVER_TYPE_1500:
		{

		}
		break;
	default:
		{
			return false;
		}
	}

	return true;
}

CDriverBuffer* CDriverOperator::GetDriverBuffer()
{
	return m_pDriverBuffer;
}


bool CDriverOperator::InitBuffer(unsigned int nLen)
{
	m_pDriverBuffer->InitBuffer(nLen);
	return true;
}

bool CDriverOperator::DestroyBuffer()
{
	m_pDriverBuffer->DestroyBuffer();
	return true;
}


UINT WINAPI PlaybackGrabThread(LPVOID lpVoid IN)
{
	CDriverOperator *pThis = (CDriverOperator*)lpVoid;

	if (NULL != pThis)
	{
		pThis->PlaybackGrabThreadDispose();
	}

	return TRUE;
}

void CDriverOperator::PlaybackGrabThreadDispose()
{
	std::string strType(_T(""));
	int nType = m_pConfigData->GetDriverType(strType);
	switch (nType)
	{
	case DRIVER_TYPE_DALSA:
		{
			if (m_pDALSADriver == NULL)
			{
				m_bPlaybackGrab = false;
				return;
			}
			m_pDALSADriver->SetBufData(m_pDriverBuffer);

			FILE *fpObject = NULL;
			size_t readlen;
			int nImgLen = m_pDriverBuffer->GetUnpacker()->GetPackageLen();
			if (m_pByPlaybackData == NULL)
			{
				m_pByPlaybackData = new BYTE[nImgLen];
			}
			memset(m_pByPlaybackData, 0, nImgLen);

			fopen_s(&fpObject, m_strPlaybackPath.c_str(), "rb");
			if (fpObject == NULL)
			{
				m_bPlaybackGrab = false;
				return;
			}
			while (m_bPlaybackGrab)
			{
				do{	
					if (feof(fpObject))
					{
						continue;
					}

					readlen = fread(m_pByPlaybackData, sizeof(BYTE), nImgLen, fpObject);

					BYTE *pDstData = NULL;
					int nWriteIndex = m_pDriverBuffer->GetWriteIndex();
					int nReadIndex = m_pDriverBuffer->GetReadIndex();
					m_pDriverBuffer->GetAddress(nWriteIndex, (void**)&pDstData);

					m_pDataAnalysis->GetPackageInfo(m_pDriverBuffer->GetUnpacker());
					m_pDataAnalysis->AnalysisData(pDstData, m_pByPlaybackData);
					m_pDriverBuffer->WriteNext();

					m_pfRevcBufOperator(GetDriverBuffer(), m_pCallbackPoint);

					m_pDriverBuffer->ReadNext();


				}while (!feof(fpObject));

				fseek(fpObject, 0, SEEK_SET);

				Sleep(500);
			}

			if (m_pByPlaybackData != NULL)
			{
				delete [] m_pByPlaybackData;
				m_pByPlaybackData = NULL;
			}
		}
		break;
	case DRIVER_TYPE_1500:
		{

		}
		break;
	default:
		{
			return;
		}
	}
}


bool CDriverOperator::StartPlaybackGrab(char *cPath)
{
	if (m_bPlaybackGrab)
	{
		return true;
	}
	m_strPlaybackPath = cPath;

	UINT32 threadId;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, PlaybackGrabThread, this, CREATE_SUSPENDED, &threadId);
	if (hThread == NULL){
		m_strPlaybackPath = _T("");
		m_bPlaybackGrab = false;
		return false;
	}
	SetThreadAffinityMask(hThread, 1 << PLAYBACK_THREAD);
	ResumeThread(hThread);

	m_bPlaybackGrab = true;

	return true;
}

bool CDriverOperator::StopPlaybackGrab()
{
	m_bPlaybackGrab = false;

	return true;
}


 
bool CDriverOperator::SetMode(int nCropWidth, int nCropHeight)
{
	std::string strType(_T(""));
	int nType = m_pConfigData->GetDriverType(strType);
	switch (nType)
	{
	case DRIVER_TYPE_DALSA:
		{
			m_pDALSADriver->SwitchMode(nCropWidth, nCropHeight);
			DestroyBuffer();
			m_pDALSADriver->DestroyDriver();

			ResetConfig(nCropWidth, nCropHeight);
			int nWidth = m_pDriverBuffer->GetWidth();
			int nHeight = m_pDriverBuffer->GetHeight();
			int nBytes = m_pDriverBuffer->GetBytesPerPixel();
			if (!InitBuffer(nWidth*nHeight*nBytes)){
				return false;
			}
			if (!InitDriver()){
				return false;
			}


		}
		break;
	case DRIVER_TYPE_1500:
		{

		}
		break;
	default:
		{
			return false;
		}
	}
	return true;
}


bool CDriverOperator::ResetConfig(int nCropWidth, int nCropHeight)
{
	m_pDriverBuffer->SetWidth(nCropWidth/2);
	m_pDriverBuffer->SetHeight(nCropHeight);
	m_pDriverBuffer->GetUnpacker()->SetValidLen(nCropWidth*nCropHeight);
	m_pDriverBuffer->GetUnpacker()->SetRowValidLen(nCropWidth);
	m_pDriverBuffer->GetUnpacker()->SetColValidLen(nCropHeight);

	return true;
}

bool CDriverOperator::GetCardStatus(int &nStatus)
{
	std::string strType(_T(""));
	int nType = m_pConfigData->GetDriverType(strType);
	switch (nType)
	{
	case DRIVER_TYPE_DALSA:
		{	
			nStatus = m_pDALSADriver->GetCardStatus();
		}
		break;
	case DRIVER_TYPE_1500:
		{

		}
		break;
	default:
		{
			return false;
		}
	}
	return true;
}