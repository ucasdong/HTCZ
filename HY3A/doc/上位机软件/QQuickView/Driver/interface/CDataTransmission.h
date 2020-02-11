#pragma once
 
#include "CDriverData.h"

class CDataTransferAnalysis
{
public:
	CDataTransferAnalysis();
	virtual ~CDataTransferAnalysis();

public:
	void	AnalysisData(unsigned char *pDestData, unsigned char *pSrcData);
	void	GetPackageInfo(CUnpacker* pInfo);
private:
	CUnpacker* m_pPackageInfo;
};