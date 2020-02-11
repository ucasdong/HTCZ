#pragma once

#include "CDriver.h"

class C1500Driver : public CDriver
{
public:
	C1500Driver();
	virtual ~C1500Driver();

public:
	virtual bool InitDriver();
	virtual void DestroyDriver();
	virtual int  InitThread();
	virtual void SetCCF(const char *pPath);
	virtual void SetBufferCount(int nCount);
	 
	void CreateCCF();

private:
	std::string			m_strCCFPath;
	bool				m_bGrabRun;
	int					m_nBufferCount;
};

