#include "RecvSapData.h"

char *findStr1(char* str, int length, char* sub, int * index)
{
	char *p = NULL;
	int len = 4;
	int findIndex = 0;
	for (int i = 0; i < length - len; i++)
	{
		if (strncmp(str, sub, len) == 0)
		{
			p = str;
			break;
		}
		str++;
		findIndex++;
	}

	*index = findIndex;
	return p;
}

char header1[4] = { 0x1a,0xcf,0xfc,0x1d };
//char header1[4] = { 0xcf,0x1a,0x1d,0xfc };
//char header1[4] = { 0x1d,0xfc,0xcf,0x1a };
DWORD WINAPI GrabDataTest(void* grabData)
{
	RecvSapData* grabDataTemp = static_cast<RecvSapData*>(grabData);
	if (grabDataTemp != NULL)
	{
		while (1)
		{
			if (!grabDataTemp->bStartOrStop)
			{
				break;
			}
			QFile file(grabDataTemp->strReplayFile);
			int buffersize = file.size();
			char* pData = (char*)malloc(buffersize);
		
			file.open(QFile::ReadOnly);
			DWORD size = file.read(pData, buffersize);
			DWORD iIndex = 0;
			int nIndex = 0;
			char *phead = findStr1((char*)pData, buffersize, header1, &nIndex);
			iIndex += nIndex;
			if (phead != NULL)
			{
				while(grabDataTemp->bStartOrStop)
				{
					//传递数据
					if (iIndex > (size - 1024))//后面的数据不足1024
					{
						break;
					}
					BYTE* pDataTemp = (BYTE*)(phead + iIndex);
					iIndex += 1024;
				
					map<BYTE, CameraParams>::iterator itCameraParam = grabDataTemp->mapChannelToCamParams.find(pDataTemp[5]); //通道标识对应的相机数据
					if (itCameraParam != grabDataTemp->mapChannelToCamParams.end())
					{
						itCameraParam->second.pickPkg->PushPkg(pDataTemp, 1);
					}
					//Sleep(10);
				}
 			}
			
			file.close();
			free(pData);
			pData = NULL;
		}
	}
	return 0;
}

RecvSapData::RecvSapData()
{
	file = fopen("d:/dataTemp.raw", "w+b");

	bStartOrStop = false;
	saveWnd = NULL;

	iCurrentModel = 0;
}


RecvSapData::~RecvSapData()
{
	if (m_Xfer_2711)
	{
		Stop_2711();
		Sleep(1000);
		DestroyObjects_2711();
		Sleep(1000);
	}
//	fclose(file);
}

void RecvSapData::initGrab()
{
	for (map<BYTE, CameraParams>::iterator it = mapChannelToCamParams.begin(); it != mapChannelToCamParams.end(); it++)
	{
		delete it->second.pickPkg;
		it->second.pickPkg = NULL;
		delete it->second.recvData;
		it->second.recvData = NULL;
	}
	mapChannelToCamParams.clear();
	vecChannelAddress.clear();

	CameraParams oneCameraParam;
	for (vector<oneCameraParams>::iterator itCamera = vecAllCameraParams.begin(); itCamera != vecAllCameraParams.end(); itCamera++)
	{
		for (unordered_map<int, oneCCDNameAndSize>::iterator itCCD = itCamera->mapCCD_NamesAndSizes.begin(); itCCD != itCamera->mapCCD_NamesAndSizes.end(); itCCD++)
		{
			oneCameraParam.byteCameraAdd = itCCD->second.iCCD_Analysis_ID;
			oneCameraParam.byteCameraID = itCamera->Camera_Analysis_ID;
			oneCameraParam.dwOneDataLength = itCamera->One_Frame_LineLength;
			oneCameraParam.dwFrameCount = itCamera->One_Frame_LineCount;
			oneCameraParam.dwPerPixLength = itCamera->One_Pixel_Length;
			oneCameraParam.pickPkg = NULL;
			oneCameraParam.recvData = NULL;
			mapChannelToCamParams.insert(std::pair<BYTE, CameraParams>(oneCameraParam.byteCameraAdd, oneCameraParam));
			vecChannelAddress.push_back(oneCameraParam.byteCameraAdd);
		}		
	}
	init_grab_2711();
}

void RecvSapData::setSubWnds(map<int, GraphicCollectAndProcess*>* mapTyep2Wnd, vector<oneCameraParams>* vecAllCamera)
{
	mapTypeToGraphicWnd = *mapTyep2Wnd;
	vecAllCameraParams = *vecAllCamera;
	initGrab();
}

void RecvSapData::init_grab_2711()
{
	char serverName[] = "Xtium-CL_MX4_1";

	SapLocation *m_loc;
	int port;

	m_loc = new SapLocation(serverName, 0);

	// Define on-line objects
	if (iCurrentModel == 0)
	{
		char fileName[] = "ccsds6004.ccf";
		m_Acq_2711 = new SapAcquisition(*m_loc, fileName);//获取类
	}
	else if (iCurrentModel == 1)
	{
		char fileName[] = "ccsds8.ccf";
		m_Acq_2711 = new SapAcquisition(*m_loc, fileName);//获取类
	}

	m_Buffer_2711 = new SapBufferWithTrash(BUFFER_COUNT, m_Acq_2711);//关联缓冲区

	m_Xfer_2711 = new SapAcqToBuf(m_Acq_2711, m_Buffer_2711, XferCallback_2711, this);//接收数据

	if (m_Acq_2711 && !(*m_Acq_2711) && !m_Acq_2711->Create())
	{
		//QString strErr = QStringLiteral("系统没有找到板卡,请重新启动系统");
		//ReBoot(0);
		return;
	}

	//check to see if both acquision devices support scatter gather.//检查采集器是否都支持散点采集
	BOOL acq0SupportSG = SapBuffer::IsBufferTypeSupported(m_Acq_2711->GetLocation(), SapBuffer::TypeScatterGather);

	if (!acq0SupportSG)// || !acq1SupportSG)
	{
		// check if they support scatter gather physical//检查他们是否支持物理分散采集
		BOOL acq0SupportSGP = SapBuffer::IsBufferTypeSupported(m_Acq_2711->GetLocation(), SapBuffer::TypeScatterGatherPhysical);

		if (!(!acq0SupportSG && acq0SupportSGP))
		{
			QString str = QString("The chosen acquisition devices\n\n-%1\n\ndo not support similar buffer types.\nThe demo will now close.").arg(m_Acq_2711->GetLocation().GetServerName());
			return;
		}
	}

	// Create all objects
	if (!CreateObjects_2711())
	{
		return;
	}

	_ID tmp;
	for (int i = 0; i < vecChannelAddress.size(); i++)
	{
		map<BYTE, CameraParams>::iterator itParam = mapChannelToCamParams.find(vecChannelAddress[i]);
		if (itParam == mapChannelToCamParams.end())
		{
			continue;
		}
		//PickPkgContent* m_PickPkgContentTemp = new PickPkgContent;
		PickPkg* m_PickPkgContentTemp = new PickPkg;
		tmp.id = (BYTE*)malloc(8);
		tmp.len = 4;
		tmp.position = 0;

		tmp.id[0] = 0x1A;
		tmp.id[1] = 0xCF;
		tmp.id[2] = 0xFC;
		tmp.id[3] = 0x1D;
		/*
		tmp.id[0] = 0x1d;
		tmp.id[1] = 0xfc;
		tmp.id[2] = 0xcf;
		tmp.id[3] = 0x1a;
		*/
		m_PickPkgContentTemp->SetPkgID(tmp.id, tmp.len, tmp.position);

		tmp.len = 8;
		tmp.position = 0;
// 		tmp.id[0] = 0x00;
// 		tmp.id[1] = 0xFA;
// 		tmp.id[2] = 0x00;
// 		tmp.id[3] = 0xF3;
// 		tmp.id[4] = 0x00;
// 		tmp.id[5] = 0x34;
// 		tmp.id[6] = 0x00;
// 		tmp.id[7] = 0x5A;
		
		tmp.id[0] = 0xFA;
		tmp.id[1] = 0x00;
		tmp.id[2] = 0xF3;
		tmp.id[3] = 0x00;
		tmp.id[4] = 0x34;
		tmp.id[5] = 0x00;
		tmp.id[6] = 0x5A;
		tmp.id[7] = 0x00;
		m_PickPkgContentTemp->SetRowID(tmp.id, tmp.len, tmp.position);

		m_PickPkgContentTemp->CreateBuffer(itParam->second.dwOneDataLength, itParam->second.dwFrameCount, 1, itParam->second.dwPerPixLength);
		//m_PickPkgContentTemp->SetPkgSize(CCSDS_PKGSIZE);
		m_PickPkgContentTemp->SetDataInfo(CCSDS_PKGSIZE, CCSDS_PKGSTARTSIZE, CCSDS_PKGDATASIZE);
		m_PickPkgContentTemp->m_spectrl = i;
		m_PickPkgContentTemp->RegisterCallBack(Test_2711, this);

		itParam->second.pickPkg = m_PickPkgContentTemp;

		itParam->second.recvData = new BYTE[itParam->second.dwOneDataLength * itParam->second.dwFrameCount * itParam->second.dwPerPixLength];
	}
}

void RecvSapData::SplitBuffer_2711()
{
	unsigned char* pDataTemp;
	int width = m_Buffer_2711->GetWidth();
	int height = m_Buffer_2711->GetHeight();
	int buffersize = width * height * m_Buffer_2711->GetBytesPerPixel();

	m_Buffer_2711->GetAddress((void**)&pDataTemp);
	
 	for (int i = 0; i < buffersize;)
 	{
 		BYTE dataTemp = pDataTemp[i + 1];
 		pDataTemp[i + 1] = pDataTemp[i];
 		pDataTemp[i] = dataTemp;
 		i += 2;
 	}
//	fwrite((char*)(pDataTemp), 1, buffersize, file);

	//数据存储
	if (saveWnd != NULL)
	{
		saveWnd->insertSaveData("51", pDataTemp, height);
	}
	for (int i = 0; i < height; i++)
	{
		//传递数据
		BYTE index = pDataTemp[i* width + CHANNEL_INDEX];
		map<BYTE, CameraParams>::iterator itCameraParam = mapChannelToCamParams.find(index); //通道标识对应的相机数据
		if (itCameraParam != mapChannelToCamParams.end())
		{
			itCameraParam->second.pickPkg->PushPkg(&pDataTemp[i* width], 1);
		}
	}
}

void RecvSapData::XferCallback_2711(SapXferCallbackInfo *pInfo)
{
	RecvSapData *pDlg = (RecvSapData *)pInfo->GetContext();
	if (pInfo->IsTrash())
	{
		QString str = QString("Frames acquired in trash buffer:") + QString::number(pInfo->GetEventCount());
	}
	else
	{
#ifdef  THREAD_AFFINITY
		HANDLE hthread = ::GetCurrentThread(); // 本进程的HANDLE
	//	 DWORD dwSysMask, dwProcessMask; // 系统配置的所有处理器，本进程可用的处理器
		::SetThreadAffinityMask(hthread, MAIN_GRAB_BASE_THREAD1);
#endif 
		if (!pDlg->run_thread_2711)
		{
			return;
		}
		SetThreadExecutionState(ES_DISPLAY_REQUIRED);

		pDlg->SplitBuffer_2711();
	}
}

void RecvSapData::Test_2711(void *params, int index)
{
	RecvSapData *pDlg = (RecvSapData *)params;

	map<BYTE, CameraParams>::iterator itFindPick = pDlg->mapChannelToCamParams.find(pDlg->vecChannelAddress[index]);
	if (itFindPick == pDlg->mapChannelToCamParams.end())
	{
		return;
	}
	
	DWORD dwLength = itFindPick->second.dwOneDataLength * itFindPick->second.dwFrameCount * itFindPick->second.dwPerPixLength;

	itFindPick->second.pickPkg->ReadBuffer(itFindPick->second.recvData);
	map<int, GraphicCollectAndProcess*>::iterator itFindWnd = pDlg->mapTypeToGraphicWnd.find(itFindPick->second.byteCameraID);
	if (itFindWnd != pDlg->mapTypeToGraphicWnd.end())
	{
		itFindWnd->second->insertGraphicData(itFindPick->second.byteCameraID, itFindPick->second.recvData);
	}
	//fwrite((char*)(itFindPick->second.recvData), 1, dwLength, pDlg->file);
}

void RecvSapData::Grab_2711()
{
	if (!m_Xfer_2711->Grab())// && m_Xfer_2711[1]->Grab())
	{
		QString message = "err";
		return;
	}
	run_thread_2711 = true;
}

void RecvSapData::Stop_2711()
{
	if (m_Xfer_2711->IsGrabbing())
	{
		run_thread_2711 = false;
		if (m_Xfer_2711 != NULL)
		{
			if (m_Xfer_2711->Freeze())// && m_Xfer_2711[1]->Freeze())
			{
				m_Xfer_2711->Abort();
			}
		}
	}
}

BOOL RecvSapData::CreateObjects_2711()
{
	UINT32 m_Buffer_2711Width = m_Acq_2711->GetXferParams().GetWidth();
	UINT32 m_Buffer_2711Height = m_Acq_2711->GetXferParams().GetHeight();
	UINT32 m_Buffer_2711Format = m_Acq_2711->GetXferParams().GetFormat();

	run_thread_2711 = false;

	// Create buffer object
	if (SapBuffer::IsBufferTypeSupported(m_Acq_2711->GetLocation(), SapBuffer::TypeScatterGather))
		m_Buffer_2711->SetParameters(BUFFER_COUNT, m_Buffer_2711Width, m_Buffer_2711Height, m_Buffer_2711Format, SapBuffer::TypeScatterGather);
	else if (SapBuffer::IsBufferTypeSupported(m_Acq_2711->GetLocation(), SapBuffer::TypeScatterGatherPhysical))
		m_Buffer_2711->SetParameters(BUFFER_COUNT, m_Buffer_2711Width, m_Buffer_2711Height, m_Buffer_2711Format, SapBuffer::TypeScatterGatherPhysical);

	m_Buffer_2711->SetPixelDepth(m_Acq_2711->GetXferParams().GetPixelDepth());

	if (m_Buffer_2711 && !*m_Buffer_2711 && !m_Buffer_2711->Create())
	{
		DestroyObjects_2711();
		return FALSE;
	}

	if (m_Xfer_2711 && !*m_Xfer_2711 && !m_Xfer_2711->Create())
	{
		DestroyObjects_2711();
		return FALSE;
	}
	return TRUE;
}

BOOL RecvSapData::DestroyObjects_2711()
{
	BOOL bResult = TRUE;

	if (m_Xfer_2711 && *m_Xfer_2711)
		bResult &= m_Xfer_2711->Destroy();

	if (m_Buffer_2711 && *m_Buffer_2711)
		bResult &= m_Buffer_2711->Destroy();

	if (m_Acq_2711 && *m_Acq_2711)
		bResult &= m_Acq_2711->Destroy();

	return bResult;
}

void RecvSapData::slot_StartOrStopGrab(bool bFlag)
{
	if (bStartOrStop)
	{
		return;
	}
	if (bFlag)
	{
		Grab_2711();
	}
	else
	{
		Stop_2711();
	}
	//bStartOrStop = bFlag;
}

void RecvSapData::slot_ReplayData(QString filePath, bool bStartOrStopTemp)
{
	strReplayFile = filePath;
	bStartOrStop = bStartOrStopTemp;
	if (bStartOrStop)
	{
		HANDLE hDBThread = CreateThread(NULL, 0, GrabDataTest, this, 0, NULL);
	}
	else
	{

	}
}

void RecvSapData::slot_RecvDataMap(map<QString, bool>* mapTypeToShow)
{
	mapTypeToShowState = *mapTypeToShow;
}

void RecvSapData::slot_SaveData()
{
	if (saveWnd == NULL)
	{
		saveWnd = new SaveSettingWnd;
	}
	saveWnd->showNormal();
}

void RecvSapData::slot_ChangedModel(int iModel)
{
	iCurrentModel = iModel;

	if (m_Xfer_2711)
	{
		Stop_2711();
		Sleep(1000);
		DestroyObjects_2711();
		Sleep(1000);
	}

	initGrab();
	for (map<BYTE, CameraParams>::iterator itCameraParam = mapChannelToCamParams.begin(); itCameraParam != mapChannelToCamParams.end(); itCameraParam++)
	{
		itCameraParam->second.pickPkg->changeModel(iModel);
	}
}

