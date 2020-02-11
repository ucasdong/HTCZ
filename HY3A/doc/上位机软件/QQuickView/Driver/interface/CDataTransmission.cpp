#include "CDataTransmission.h"

CDataTransferAnalysis::CDataTransferAnalysis()
{
}

CDataTransferAnalysis::~CDataTransferAnalysis()
{
}


void CDataTransferAnalysis::AnalysisData(unsigned char *pDestData, unsigned char *pSrcData)
{
	int nPackageLen = m_pPackageInfo->GetPackageLen();		//���ܳ���
	int nHeadLen = m_pPackageInfo->GetHeadLen();			//��ͷ����
	int nErrorInfo = m_pPackageInfo->GetErrorInfo();		//������Ϣ
	int nValidLen = m_pPackageInfo->GetValidLen();			//��Ч���ݳ���
	int nRowLen = m_pPackageInfo->GetRowLen();				//�г���
	int nColLen = m_pPackageInfo->GetColLen();				//�г���
	int nRowValidLen = m_pPackageInfo->GetRowValidLen();	//����Ч����
	int nColValidLen = m_pPackageInfo->GetColValidLen();	//����Ч����
	bool bRowAnalysis = m_pPackageInfo->IsRowAnalysis();	//�����н������ǿ����

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