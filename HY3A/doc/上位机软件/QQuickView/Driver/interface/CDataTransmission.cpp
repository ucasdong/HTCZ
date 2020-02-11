#include "CDataTransmission.h"

CDataTransferAnalysis::CDataTransferAnalysis()
{
}

CDataTransferAnalysis::~CDataTransferAnalysis()
{
}


void CDataTransferAnalysis::AnalysisData(unsigned char *pDestData, unsigned char *pSrcData)
{
	int nPackageLen = m_pPackageInfo->GetPackageLen();		//包总长度
	int nHeadLen = m_pPackageInfo->GetHeadLen();			//包头长度
	int nErrorInfo = m_pPackageInfo->GetErrorInfo();		//错误信息
	int nValidLen = m_pPackageInfo->GetValidLen();			//有效数据长度
	int nRowLen = m_pPackageInfo->GetRowLen();				//行长度
	int nColLen = m_pPackageInfo->GetColLen();				//列长度
	int nRowValidLen = m_pPackageInfo->GetRowValidLen();	//行有效长度
	int nColValidLen = m_pPackageInfo->GetColValidLen();	//列有效长度
	bool bRowAnalysis = m_pPackageInfo->IsRowAnalysis();	//按照行解析还是块解析

	if (bRowAnalysis)
	{
		for (int nCol = 0; nCol != nColValidLen; ++nCol)
		{
			//memcpy(pDestData, pSrcData, nRowValidLen);
			memcpy_s(pDestData, nRowValidLen, pSrcData, nRowValidLen);
			pSrcData += nRowLen;
			pDestData += nRowValidLen;
		} 
	}
	else
	{

	}
}

void CDataTransferAnalysis::GetPackageInfo(CUnpacker* pInfo)
{
	m_pPackageInfo = pInfo;
}