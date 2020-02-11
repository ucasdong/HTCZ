#include "CDriverData.h"

CDriverBuffer::CDriverBuffer()	
{
	m_nTotalLen = 0;
	m_nWidth	= 0;
	m_nHeight	= 0;
	m_nBytesPerPixel = 0;
	m_nIndex    = 0;
	m_nBufCache = 0;
	m_nReadIndex  = 0;
	m_nWriteIndex = 0;

	m_pUnPancker = new CUnpacker;
}

CDriverBuffer::~CDriverBuffer()
{
	DestroyBuffer();
	if (m_pUnPancker != NULL)
	{
		delete m_pUnPancker;
		m_pUnPancker = NULL;
	}
}


void CDriverBuffer::InitBuffer(int nWidth, int nHeight, int nPerPixel)
{

}

void CDriverBuffer::InitBuffer(unsigned int nLen)
{
	m_vecBuffer.clear();
	int nCount = GetBufCache();
	for (int i = 0; i != nCount; ++i)
	{
		BYTE *pBuffer = new BYTE[nLen];
		memset(pBuffer, 0, nLen);
		m_vecBuffer.push_back(pBuffer);
	}
	SetTotalLen(nLen);
}

void CDriverBuffer::ClearBuffer()
{
	int nLen = GetTotalLen();
	for (std::vector<BYTE*>::iterator it = m_vecBuffer.begin(); it != m_vecBuffer.end(); ++it)
	{
		memset((*it), 0, nLen);
	}
}

void CDriverBuffer::DestroyBuffer()
{
	for (std::vector<BYTE*>::iterator it = m_vecBuffer.begin(); it != m_vecBuffer.end(); ++it)
	{
		if ((*it) != NULL)
		{
			delete (*it);
			(*it) = NULL;
		}
	}
	m_vecBuffer.clear();
}

int CDriverBuffer::GetWidth()		
{ 
	return m_nWidth; 
}

int CDriverBuffer::GetHeight()		
{ 
	return m_nHeight; 
}
int	CDriverBuffer::GetBytesPerPixel()	
{ 
	return m_nBytesPerPixel; 
}

void CDriverBuffer::SetWidth(int nWidth)			
{
	m_nWidth = nWidth; 
}

void CDriverBuffer::SetHeight(int nHeight)			
{ 
	m_nHeight = nHeight; 
}

void CDriverBuffer::SetBytesPerPixel(int nPerBytes)		
{ 
	m_nBytesPerPixel = nPerBytes; 
}

void CDriverBuffer::SetBufCache(int nNum)			
{
	m_nBufCache = nNum; 
}

int CDriverBuffer::GetBufCache()
{
	return m_nBufCache;
}

void CDriverBuffer::SetIndex(int nPos)
{
	if (nPos >= m_nBufCache)
	{
		nPos = 0;
	}
	m_nIndex = nPos;
}

int CDriverBuffer::GetIndex()
{
	return m_nIndex;
}


void CDriverBuffer::SetTotalLen(int nLen)
{
	m_nTotalLen = nLen;
}

int CDriverBuffer::GetTotalLen()
{
	return m_nTotalLen;
}

void CDriverBuffer::Next()
{
	int nNum = GetBufCache();
	if(m_nIndex + 1 >= nNum) 
		m_nIndex = 0;
	else
		m_nIndex++; 
}

void CDriverBuffer::WriteNext()
{
	int nNum = GetBufCache();
	if(m_nWriteIndex + 1 >= nNum) 
		m_nWriteIndex = 0;
	else
		m_nWriteIndex++; 
}

void CDriverBuffer::ReadNext()
{
	int nNum = GetBufCache();
	if(m_nReadIndex + 1 >= nNum) 
		m_nReadIndex = 0;
	else
		m_nReadIndex++; 
}

int CDriverBuffer::GetWriteIndex()
{
	return m_nWriteIndex;
}

int CDriverBuffer::GetReadIndex()
{
	return m_nReadIndex;
}

bool CDriverBuffer::GetAddress(int nIndex, void** pData)
{
	int nNum = GetBufCache();
	if (nIndex >= nNum)
	{
		return false;
	}

	if (m_vecBuffer[nIndex] == NULL)
	{
		return false;
	}
	 
	*pData = (void*)m_vecBuffer[nIndex]; 

	return true;
}

bool CDriverBuffer::GetAddress(void **pData)
{
	int nNum = GetBufCache();

	int nIndex = /*GetIndex*/GetReadIndex();
	if (nIndex >= nNum/*-1*/)
	{
		nIndex = 0;
	}
	*pData = (void*)m_vecBuffer[nIndex]; 

	return true;
}

//bool CDriverBuffer::GetWriteAddress(void **pData)
//{
//	int nNum = GetBufCache();
//
//	int nIndex = GetWriteIndex();
//	if (nIndex >= nNum-1)
//	{
//		nIndex = 0;
//	}
//	*pData = (void*)m_vecBuffer[nIndex]; 
//
//	return true;
//}
//
//bool CDriverBuffer::GetReadAddress(void** pData)
//{
//	int nNum = GetBufCache();
//
//	int nIndex = GetReadIndex();
//	if (nIndex >= nNum-1)
//	{
//		nIndex = 0;
//	}
//	*pData = (void*)m_vecBuffer[nIndex]; 
//
//	return true;
//}


bool CDriverBuffer::Copy(CDriverBuffer* pSrc)
{
	int nSrcSize = pSrc->GetWidth() * pSrc->GetHeight() * pSrc->GetBytesPerPixel();
	int nOrigSize = GetWidth() * GetHeight() * GetBytesPerPixel();

	if(nSrcSize != nOrigSize)
	{
		return false;
	}
	else
	{
		int nNum = GetBufCache();
		for (int i = 0; i != nNum; ++i)
		{
			memcpy(&m_vecBuffer[i], &pSrc->m_vecBuffer[i], nOrigSize);
		}
		return true;
	}
}


//===================================//

CUnpacker::CUnpacker()	
{
	m_nHeadLen	= 0;
	m_nErrorInfo = 0;
	m_nPackageLen = 0;
	m_nValidLen	= 0;
	m_nRowValidLen = 0;		
	m_nColValidLen = 0;	
	m_nRowLen = 0;		
	m_nColLen = 0;	
	m_bRowAnalysis = false;

}

CUnpacker::~CUnpacker()
{
}

//===================================//


CConfigData::CConfigData()	
{
	m_nOrigWidth		= 0;
	m_nOrigHeight		= 0;
	m_nBufferCount		= 0;
	m_strCCFPath		= "";
	m_strDriverType		= "";
}

CConfigData::~CConfigData()
{

} 


int CConfigData::GetDriverType(std::string &strType)				
{ 
	strType = m_strDriverType; 
	if (m_strDriverType == "DALSA")
	{
		return 0;
	}
	else if (m_strDriverType == "1500")
	{
		return 1;
	}
	return -1;
}