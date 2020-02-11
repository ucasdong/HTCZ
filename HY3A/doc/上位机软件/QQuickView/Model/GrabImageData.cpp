#include "GrabImageData.h"

DWORD WINAPI GrabData(void* grabData)
{
	GrabImageData* grabDataTemp = static_cast<GrabImageData*>(grabData);
	if (grabDataTemp != NULL)
	{
		while (1)
		{
			if (grabDataTemp->getGrabFlag())
			{
				grabDataTemp->createGraphicData();
				Sleep(1000);
			}
			if (grabDataTemp->getStopThreadFlag())
			{
				break;
			}
		}
	}
	return 0;
}

GrabImageData::GrabImageData(QObject *parent)
	: QObject(parent)
{
	bStartGrab = false;
	bStopThread = false;	
	iIndex = 0;

	iFrameCount = 3534;
	iFrameLength = 430;
	iPixelLength = 2;
	originalDataAll = new BYTE[iFrameCount * iFrameLength * iPixelLength];
	//hDBThread = CreateThread(NULL, 0, GrabData, this, 0, NULL);
}

GrabImageData::~GrabImageData()
{
	bStopThread = true;
	StopGrab();
	WaitForSingleObject(hDBThread, INFINITE);
	delete originalDataAll;
	originalDataAll = NULL;
}


void GrabImageData::createGraphicData()
{
	if (iIndex > 6000)
	{
		iIndex = 0;
	}
	//memset(originalDataAll, 0x00, iFrameCount * iFrameLength * iPixelLength);
	//for (int iFram = 0; iFram < iFrameCount; iFram++)
	{
		//memset(originalData, iIndex % 255, 200 * iPixelLength);

		originalDataAll[0] = 0xFA;
		originalDataAll[1] = 0x00;
		originalDataAll[2] = 0xF3;
		originalDataAll[3] = 0x00;
		originalDataAll[4] = 0x34;
		originalDataAll[5] = 0x00;
		originalDataAll[6] = 0x14;
		originalDataAll[7] = 0x00;
		originalDataAll[8] = 0x00;
		originalDataAll[9] = 0x00;
		originalDataAll[10] = 0x01;
		originalDataAll[11] = 0x00;
		originalDataAll[12] = 0x0a;
		originalDataAll[13] = 0x00;
		originalDataAll[14] = 0x55;
		originalDataAll[15] = 0x00;

		originalDataAll[20] = iIndex / 256;
		originalDataAll[21] = 0;
		originalDataAll[22] = iIndex % 256;
		originalDataAll[23] = 0;
		iIndex++;

		for (int j = 6020 * iPixelLength; j < 27520 * iPixelLength; j++)
		{
			originalDataAll[j] = 50;
		}
		for (int j = 27520 * iPixelLength; j < 49020 * iPixelLength; j++)
		{
			originalDataAll[j] = 100;
		}
		for (int j = 49020 * iPixelLength; j < 1476620 * iPixelLength; j++)
		{
			originalDataAll[j] = 150;
		}
		for (int j = 1476620 * iPixelLength; j < 1498120 * iPixelLength; j++)
		{
			originalDataAll[j] = 200;
		}
		for (int j = 1476620 * iPixelLength; j < 1519620 * iPixelLength; j++)
		{
			originalDataAll[j] = 250;
		}
		int indexTemp = 6020 * iPixelLength;
		originalDataAll[indexTemp] = 0xb9;
		originalDataAll[indexTemp + 1] = 0x60;
		originalDataAll[indexTemp + 2] = 0x5c;
		originalDataAll[indexTemp + 3] = 0x40;
		originalDataAll[indexTemp + 4] = 0x63;
		originalDataAll[indexTemp + 5] = 0xa0;
		originalDataAll[indexTemp + 6] = 0xdd;
		originalDataAll[indexTemp + 7] = 0xd0;
		indexTemp = 27520 * iPixelLength;
		originalDataAll[indexTemp] = 0xb9;
		originalDataAll[indexTemp + 1] = 0x60;
		originalDataAll[indexTemp + 2] = 0x5c;
		originalDataAll[indexTemp + 3] = 0x40;
		originalDataAll[indexTemp + 4] = 0x63;
		originalDataAll[indexTemp + 5] = 0xa0;
		originalDataAll[indexTemp + 6] = 0xaa;
		originalDataAll[indexTemp + 7] = 0xa0;
		indexTemp = 49020 * iPixelLength;
		originalDataAll[indexTemp] = 0xb9;
		originalDataAll[indexTemp + 1] = 0x60;
		originalDataAll[indexTemp + 2] = 0x5c;
		originalDataAll[indexTemp + 3] = 0x40;
		originalDataAll[indexTemp + 4] = 0x63;
		originalDataAll[indexTemp + 5] = 0xa0;
		originalDataAll[indexTemp + 6] = 0xee;
		originalDataAll[indexTemp + 7] = 0xe0;
		indexTemp = 1476620 * iPixelLength;
		originalDataAll[indexTemp] = 0xb9;
		originalDataAll[indexTemp + 1] = 0x60;
		originalDataAll[indexTemp + 2] = 0x5c;
		originalDataAll[indexTemp + 3] = 0x40;
		originalDataAll[indexTemp + 4] = 0x63;
		originalDataAll[indexTemp + 5] = 0xa0;
		originalDataAll[indexTemp + 6] = 0xcc;
		originalDataAll[indexTemp + 7] = 0xc0;
		indexTemp = 1498120 * iPixelLength;
		originalDataAll[indexTemp] = 0xb9;
		originalDataAll[indexTemp + 1] = 0x60;
		originalDataAll[indexTemp + 2] = 0x5c;
		originalDataAll[indexTemp + 3] = 0x40;
		originalDataAll[indexTemp + 4] = 0x63;
		originalDataAll[indexTemp + 5] = 0xa0;
		originalDataAll[indexTemp + 6] = 0xbb;
		originalDataAll[indexTemp + 7] = 0xb0;
	}
	m_pFunc(m_param);
}

bool GrabImageData::getGrabFlag()
{
	return bStartGrab;
} 

bool GrabImageData::getStopThreadFlag()
{
	return bStopThread;
}

void GrabImageData::GetAddress(void** pData)
{
	*pData = originalDataAll;
	int a = 0;
}

void GrabImageData::RegisterCallBack(CallbackFunc func, void* param)
{
	m_pFunc = func;
	m_param = param;
}

void GrabImageData::StartGrab()
{
	bStartGrab = true;
	bStopThread = false;
}

void GrabImageData::StopGrab()
{
	bStartGrab = false;
	//bStopThread = true;
}

void GrabImageData::StartSave(int iSaveType, char* cPath)
{

}

void GrabImageData::StopSave()
{

}

void GrabImageData::StartPlaybackGrab(char* cPath)
{

}
