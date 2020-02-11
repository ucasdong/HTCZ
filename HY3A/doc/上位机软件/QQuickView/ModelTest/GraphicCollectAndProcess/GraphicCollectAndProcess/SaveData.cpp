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
	//等待线程关闭
	bStop = true;
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	//关闭文件
	CloseHandle(hFile);
	for (std::vector<std::pair<BYTE*, DWORD>>::iterator it = vecDataBuffer.begin();it != vecDataBuffer.end();it++)
	{
		delete[] it->first;
		it->first = NULL;
	}
	vecDataBuffer.clear();
}

//初始化文件
void SaveData::initFile(QString strFileName, int iFlag)
{
	dwWriteFileSize = 0;
	dwReadFileSize = 0;
	SYSTEM_INFO sinf;
	GetSystemInfo(&sinf);
	dwAllocationGranularity = sinf.dwAllocationGranularity;//获取操作系统分配粒度

	std::string strPath = strFileName.toLocal8Bit();
	if (iFlag == 1)
	{
		//创建文件
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
		//打开文件
		hFile = CreateFile(strFileName.toLocal8Bit(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,        //打开已存在的文件 
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
//插入要写的数据（先初始化文件）
void SaveData::insertData(BYTE* dataTemp, DWORD dwLen)
{
	BYTE* dataInsert = new BYTE[dwLen];
	memcpy(dataInsert, dataTemp, dwLen);
	mutexLock.lock();
	vecDataBuffer.push_back(std::pair<BYTE*, DWORD>(dataInsert, dwLen));
	mutexLock.unlock();

	//emit signal_writeData();
}

//线程调用写文件(包含写完文件后内存释放)
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
	if (dataTemp == NULL)//数据为空
	{
		return;
	}
	if (!bInitFlag)//文件初始化失败
	{
		return;
	}
	//创建文件映射，如果要创建内存页面文件的映射，第一个参数设置为INVALID_HANDLE_VALUE
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
		DWORD dwCount1 = (dwWriteFileSize - dwLen) / dwAllocationGranularity;//当前文件大小是粒度的整数倍值
		DWORD dwCount2 = (dwWriteFileSize - dwLen) % dwAllocationGranularity;//当前文件大小相对粒度的偏移

		__int64 offset = dwCount1 * dwAllocationGranularity;  //当前偏移

		DWORD offset1 = (DWORD)(offset >> 32);
		DWORD offset2 = (DWORD)(offset & 0xFFFFFFFF);
		//获得映射视图
		char* mmfm_base_address = (char*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, (DWORD)(offset >> 32), (DWORD)(offset & 0xFFFFFFFF), dwCount2 + dwLen);
		if (mmfm_base_address == NULL)
		{
			return;
		}
		else
		{
			//向内存映射视图中写数据
			CopyMemory(mmfm_base_address + dwCount2, dataTemp, dwLen);

			FlushViewOfFile(mmfm_base_address, dwCount2 + dwLen);
			//卸载映射
			UnmapViewOfFile(mmfm_base_address);
			//关闭内存映射文件
			CloseHandle(hFileMapping);
		}
	}

	delete[] dataTemp;//释放内存
	dataTemp = NULL;
}

void SaveData::writeData2()
{
	if (!bInitFlag)//文件初始化失败
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
	if (dataTemp == NULL)//数据为空
	{
		return;
	}
	
	DWORD dwBytesWrite = 0;
	DWORD dwBytesToWrite = dwLen;
	BYTE* dataBuffer = dataTemp;
	//循环写文件，确保完整的文件被写入 
	do {                                        
		WriteFile(hFile, dataBuffer, dwBytesToWrite, &dwBytesWrite, NULL);

		dwBytesToWrite -= dwBytesWrite;
		dataBuffer += dwBytesWrite;

	} while (dwBytesToWrite > 0);

	FlushFileBuffers(hFile);

	delete[] dataTemp;//释放内存
	dataTemp = NULL;
}

//读取文件数据（dataTemp外部开辟空间传入地址，iLen读取长度也是开辟空间的长度，需要提前初始化文件）
bool SaveData::readData(BYTE* dataTemp, DWORD dwLen)
{
	if (!bInitFlag)//文件初始化失败
	{
		return false;
	}
	 //返回值size_high,size_low分别表示文件大小的高32位/低32位  
	DWORD size_low = 0, size_high = 0;
	size_low = GetFileSize(hFile, &size_high);
	//创建文件映射，如果要创建内存页面文件的映射，第一个参数设置为INVALID_HANDLE_VALUE
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
		DWORD dwCount1 = dwReadFileSize / dwAllocationGranularity;//当前文件大小是粒度的整数倍值
		DWORD dwCount2 = dwReadFileSize % dwAllocationGranularity;//当前文件大小相对粒度的偏移

		__int64 offset = dwCount1 * dwAllocationGranularity;  //当前偏移

		DWORD offset1 = (DWORD)(offset >> 32);
		DWORD offset2 = (DWORD)(offset & 0xFFFFFFFF);
		//获得映射视图
		char* mmfm_base_address = (char*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, (DWORD)(offset >> 32), (DWORD)(offset & 0xFFFFFFFF), dwCount2 + dwLen);
		if (mmfm_base_address == NULL)
		{
			return false;
		}
		else
		{
			//向内存映射视图中写数据
			CopyMemory(dataTemp, mmfm_base_address + dwCount2, dwLen);

			FlushViewOfFile(mmfm_base_address, dwCount2 + dwLen);
			//卸载映射
			UnmapViewOfFile(mmfm_base_address);
			//关闭内存映射文件
			CloseHandle(hFileMapping);
		}
		dwReadFileSize += dwLen;
	}
	return true;
}

bool SaveData::readData2(BYTE* dataTemp, DWORD dwLen)
{
	if (!bInitFlag)//文件初始化失败
	{
		return false;
	}
	DWORD dwBytesRead = 0;
	DWORD dwBytesToRead = dwLen;
	BYTE* dataBuffer = dataTemp;
	do 
	{                                       //循环读文件，确保读出完整的文件    
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
