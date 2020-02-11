#include "SaveData.h"

DWORD WINAPI WriteData(void* saveDataTemp)
{
	SaveData* saveData = static_cast<SaveData*>(saveDataTemp);
	if (saveData != NULL)
	{
		while (1)
		{
			if (saveData->bStop)
			{
				break;
			}
			saveData->writeData();
		}
	}
	return 0;
}

SaveData::SaveData()
{
	bInitFlag = false;
	bStop = false;
	hThread = CreateThread(NULL, 0, WriteData, this, 0, NULL);
	//connect(this, SIGNAL(signal_writeData()), this, SLOT(writeData()));
}

SaveData::~SaveData()
{
	//�ȴ��̹߳ر�
	bStop = true;
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	//�ر��ļ�
	CloseHandle(hFile);
	for (std::vector<std::pair<BYTE*, DWORD>>::iterator it = vecDataBuffer.begin();it != vecDataBuffer.end();it++)
	{
		delete[] it->first;
		it->first = NULL;
	}
	vecDataBuffer.clear();
}

//��ʼ���ļ�
void SaveData::initFile(QString strFileName, int iFlag)
{
	dwWriteFileSize = 0;
	dwReadFileSize = 0;
	SYSTEM_INFO sinf;
	GetSystemInfo(&sinf);
	dwAllocationGranularity = sinf.dwAllocationGranularity;//��ȡ����ϵͳ��������

	std::string strPath = strFileName.toLocal8Bit();
	if (iFlag == 1)
	{
		//�����ļ�
		hFile = CreateFile(strFileName.toLocal8Bit(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,//OPEN_ALWAYS
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN ,//| FILE_FLAG_NO_BUFFERING,
			NULL);//OPEN_ALWAYS-CREATE_ALWAYS-OPEN_EXISTING

// 		hFile = CreateFile(strFileName.toLocal8Bit(),
// 			GENERIC_READ | GENERIC_WRITE,
// 			FILE_SHARE_READ | FILE_SHARE_WRITE,
// 			NULL,
// 			CREATE_ALWAYS,//OPEN_ALWAYS
// 			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_NO_BUFFERING,
// 			NULL);//OPEN_ALWAYS-CREATE_ALWAYS-OPEN_EXISTING
	}
	else if (iFlag == 2)
	{
		//���ļ�
		hFile = CreateFile(strFileName.toLocal8Bit(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,        //���Ѵ��ڵ��ļ� 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,// | FILE_FLAG_NO_BUFFERING,
			NULL);
	}

	if (hFile == INVALID_HANDLE_VALUE)
	{
		bInitFlag = false;
		CloseHandle(hFile);
		return;
	}
	bInitFlag = true;
}
//����Ҫд�����ݣ��ȳ�ʼ���ļ���
void SaveData::insertData(BYTE* dataTemp, DWORD dwLen)
{
	BYTE* dataInsert = new BYTE[dwLen];
	memcpy(dataInsert, dataTemp, dwLen);
	mutexLock.lock();
	vecDataBuffer.push_back(std::pair<BYTE*, DWORD>(dataInsert, dwLen));
	mutexLock.unlock();

	//emit signal_writeData();
}

//�̵߳���д�ļ�(����д���ļ����ڴ��ͷ�)
void SaveData::writeData()
{
	BYTE* dataTemp = NULL;
	DWORD dwLen = 0;
	mutexLock.lock();
	std::vector<std::pair<BYTE*, DWORD>>::iterator it = vecDataBuffer.begin();
	if (it != vecDataBuffer.end())
	{
		dataTemp = it->first;
		dwLen = it->second;

		vecDataBuffer.erase(it);
	}
	else
	{
		mutexLock.unlock();
		return;
	}
	mutexLock.unlock();
	if (dataTemp == NULL)//����Ϊ��
	{
		return;
	}
	if (!bInitFlag)//�ļ���ʼ��ʧ��
	{
		return;
	}
	//�����ļ�ӳ�䣬���Ҫ�����ڴ�ҳ���ļ���ӳ�䣬��һ����������ΪINVALID_HANDLE_VALUE
	dwWriteFileSize += dwLen;

	HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, (DWORD)(dwWriteFileSize >> 32), (DWORD)(dwWriteFileSize & 0xFFFFFFFF), NULL);
	if (hFileMapping == NULL)
	{
		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return;
		}
	}
	else
	{
		DWORD dwCount1 = (dwWriteFileSize - dwLen) / dwAllocationGranularity;//��ǰ�ļ���С�����ȵ�������ֵ
		DWORD dwCount2 = (dwWriteFileSize - dwLen) % dwAllocationGranularity;//��ǰ�ļ���С������ȵ�ƫ��

		__int64 offset = dwCount1 * dwAllocationGranularity;  //��ǰƫ��

		DWORD offset1 = (DWORD)(offset >> 32);
		DWORD offset2 = (DWORD)(offset & 0xFFFFFFFF);
		//���ӳ����ͼ
		char* mmfm_base_address = (char*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, (DWORD)(offset >> 32), (DWORD)(offset & 0xFFFFFFFF), dwCount2 + dwLen);
		if (mmfm_base_address == NULL)
		{
			return;
		}
		else
		{
			//���ڴ�ӳ����ͼ��д����
			CopyMemory(mmfm_base_address + dwCount2, dataTemp, dwLen);

			FlushViewOfFile(mmfm_base_address, dwCount2 + dwLen);
			//ж��ӳ��
			UnmapViewOfFile(mmfm_base_address);
			//�ر��ڴ�ӳ���ļ�
			CloseHandle(hFileMapping);
		}
	}

	delete[] dataTemp;//�ͷ��ڴ�
	dataTemp = NULL;
}

void SaveData::writeData2()
{
	if (!bInitFlag)//�ļ���ʼ��ʧ��
	{
		return;
	}
	BYTE* dataTemp = NULL;
	DWORD dwLen = 0;
	mutexLock.lock();
	std::vector<std::pair<BYTE*, DWORD>>::iterator it = vecDataBuffer.begin();
	if (it != vecDataBuffer.end())
	{
		dataTemp = it->first;
		dwLen = it->second;

		vecDataBuffer.erase(it);
	}
	else
	{
		mutexLock.unlock();
		return;
	}
	mutexLock.unlock();
	if (dataTemp == NULL)//����Ϊ��
	{
		return;
	}
	
	DWORD dwBytesWrite = 0;
	DWORD dwBytesToWrite = dwLen;
	BYTE* dataBuffer = dataTemp;
	//ѭ��д�ļ���ȷ���������ļ���д�� 
	do {                                        
		WriteFile(hFile, dataBuffer, dwBytesToWrite, &dwBytesWrite, NULL);

		dwBytesToWrite -= dwBytesWrite;
		dataBuffer += dwBytesWrite;

	} while (dwBytesToWrite > 0);

	FlushFileBuffers(hFile);

	delete[] dataTemp;//�ͷ��ڴ�
	dataTemp = NULL;
}

//��ȡ�ļ����ݣ�dataTemp�ⲿ���ٿռ䴫���ַ��iLen��ȡ����Ҳ�ǿ��ٿռ�ĳ��ȣ���Ҫ��ǰ��ʼ���ļ���
bool SaveData::readData(BYTE* dataTemp, DWORD dwLen)
{
	if (!bInitFlag)//�ļ���ʼ��ʧ��
	{
		return false;
	}
	 //����ֵsize_high,size_low�ֱ��ʾ�ļ���С�ĸ�32λ/��32λ  
	DWORD size_low = 0, size_high = 0;
	size_low = GetFileSize(hFile, &size_high);
	//�����ļ�ӳ�䣬���Ҫ�����ڴ�ҳ���ļ���ӳ�䣬��һ����������ΪINVALID_HANDLE_VALUE
	HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, size_high, size_low, NULL);
	if (hFileMapping == NULL)
	{
		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return false;
		}
	}
	else
	{
		DWORD dwCount1 = dwReadFileSize / dwAllocationGranularity;//��ǰ�ļ���С�����ȵ�������ֵ
		DWORD dwCount2 = dwReadFileSize % dwAllocationGranularity;//��ǰ�ļ���С������ȵ�ƫ��

		__int64 offset = dwCount1 * dwAllocationGranularity;  //��ǰƫ��

		DWORD offset1 = (DWORD)(offset >> 32);
		DWORD offset2 = (DWORD)(offset & 0xFFFFFFFF);
		//���ӳ����ͼ
		char* mmfm_base_address = (char*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, (DWORD)(offset >> 32), (DWORD)(offset & 0xFFFFFFFF), dwCount2 + dwLen);
		if (mmfm_base_address == NULL)
		{
			return false;
		}
		else
		{
			//���ڴ�ӳ����ͼ��д����
			CopyMemory(dataTemp, mmfm_base_address + dwCount2, dwLen);

			FlushViewOfFile(mmfm_base_address, dwCount2 + dwLen);
			//ж��ӳ��
			UnmapViewOfFile(mmfm_base_address);
			//�ر��ڴ�ӳ���ļ�
			CloseHandle(hFileMapping);
		}
		dwReadFileSize += dwLen;
	}
	return true;
}

bool SaveData::readData2(BYTE* dataTemp, DWORD dwLen)
{
	if (!bInitFlag)//�ļ���ʼ��ʧ��
	{
		return false;
	}
	DWORD dwBytesRead = 0;
	DWORD dwBytesToRead = dwLen;
	BYTE* dataBuffer = dataTemp;
	do 
	{                                       //ѭ�����ļ���ȷ�������������ļ�    
		ReadFile(hFile, dataBuffer, dwBytesToRead, &dwBytesRead, NULL);

		if (dwBytesRead == 0)
		{
			return false;
		}

		dwBytesToRead -= dwBytesRead;
		dataBuffer += dwBytesRead;

	} while (dwBytesToRead > 0);

	return true;
}

void SaveData::resetReadFileSize(__int64 startSize)
{
	dwReadFileSize = startSize;
}
