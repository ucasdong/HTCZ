#pragma once

#include "CDriverDefine.h"
#include "../interface/CDriverDefine.h"

#define BUF_CACHE_NUM	5

class CConfigData
{
public:
	CConfigData();
	~CConfigData();

private:
	int m_nOrigWidth;
	int m_nOrigHeight;
	int m_nBufferCount;
	std::string m_strCCFPath;
	std::string m_strDriverType;

public:
	int				GetOrigWidth()				{ return m_nOrigWidth; }
	int				GetOrigHeight()				{ return m_nOrigHeight; }
	int				GetBufferCount()			{ return m_nBufferCount; }
	std::string		GetCCFPath()				{ return m_strCCFPath; }
	int				GetDriverType(std::string &strType);

	void			SetOrigWidth(int nWidth)				{ m_nOrigWidth = nWidth; }
	void			SetOrigHeight(int nHeight)				{ m_nOrigHeight = nHeight; }
	void			SetBufferCount(int nCount)				{ m_nBufferCount = nCount; }
	void			SetCCFPath(std::string strPath)			{ m_strCCFPath = strPath; }
	void			SetDriverType(std::string strType)		{ m_strDriverType = strType; }
};


class CUnpacker
{
public:
	CUnpacker();
	virtual ~CUnpacker();

private:
	int m_nPackageLen;		//包总长度
	int m_nHeadLen;			//包头长度
	int m_nErrorInfo;		//错误信息
	int m_nValidLen;		//有效数据长度
	int m_nRowLen;			//行宽度(包含无效数据)(字节)
	int m_nColLen;			//列长度(包含无效数据)
	int m_nRowValidLen;		//行有效宽度(字节)
	int m_nColValidLen;		//列有效长度
	bool m_bRowAnalysis;	//按照行解析还是块解析

public:
	int				GetPackageLen()				{ return m_nPackageLen; }
	int				GetHeadLen()				{ return m_nHeadLen; }
	int				GetErrorInfo()				{ return m_nErrorInfo; }
	int				GetValidLen()				{ return m_nValidLen; }
	int				GetRowLen()					{ return m_nRowLen; }
	int				GetColLen()					{ return m_nColLen; }
	int				GetRowValidLen()			{ return m_nRowValidLen; }
	int				GetColValidLen()			{ return m_nColValidLen; }
	bool			IsRowAnalysis()				{ return m_bRowAnalysis; }

	void			SetPackageLen(int nLen)		{ m_nPackageLen = nLen; }
	void			SetHeadLen(int nLen)		{ m_nHeadLen = nLen; }
	void			SetErrorInfo(int nInfo)		{ m_nErrorInfo = nInfo; }
	void			SetValidLen(int nLen)		{ m_nValidLen = nLen; }
	void			SetRowLen(int nLen)			{ m_nRowLen = nLen; }
	void			SetColLen(int nLen)			{ m_nColLen = nLen; }
	void			SetRowValidLen(int nLen)	{ m_nRowValidLen = nLen; }
	void			SetColValidLen(int nLen)	{ m_nColValidLen = nLen; }
	void			SetRowAnalysis(bool bOK)	{ m_bRowAnalysis = bOK; }
};


class CDriverBuffer
{
public:
	CDriverBuffer();
	virtual ~CDriverBuffer();

private:	
	std::vector<BYTE*> m_vecBuffer;
	///BYTE*			m_pByBuffer[BUF_CACHE_NUM];
	int				m_nTotalLen;
	int				m_nWidth;
	int				m_nHeight;
	int				m_nBytesPerPixel;
	int				m_nIndex;
	int				m_nBufCache;		//buf缓冲数量
	CUnpacker*		m_pUnPancker;
	int				m_nReadIndex;
	int				m_nWriteIndex;

public:
	void			InitBuffer(int nWidth, int nHeight, int nPerPixel);	 
	void			InitBuffer(unsigned int nLen);	 
	void			ClearBuffer();
	void			DestroyBuffer();

	bool			GetAddress(void **pData);
	bool			GetAddress(int nIndex, void** pData);
	void			Next();


	void			WriteNext();
	void			ReadNext();
	int				GetWriteIndex();
	int				GetReadIndex();
	//bool			GetWriteAddress(void **pData);
	//bool			GetReadAddress(void** pData);


	bool			Copy(CDriverBuffer* pSrc);

	int				GetWidth();
	int				GetHeight();
	int				GetBytesPerPixel();
	int				GetIndex();
	int				GetTotalLen();
	int				GetBufCache();

	void			SetWidth(int nWidth);
	void			SetHeight(int nHeight);
	void			SetBytesPerPixel(int nPerBytes);
	void			SetIndex(int nPos);
	void			SetTotalLen(int nLen);
	void			SetBufCache(int nNum);

	CUnpacker*		GetUnpacker()	{ return m_pUnPancker; }
};