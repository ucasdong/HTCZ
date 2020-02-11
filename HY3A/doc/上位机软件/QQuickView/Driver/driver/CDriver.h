#pragma once

#include <string>

class CDriver
{
public:
	CDriver(void){}
	virtual ~CDriver(void){}

public:
	virtual bool InitDriver() = 0;
	virtual void DestroyDriver() = 0;
	virtual int  InitThread() = 0;
	virtual void SetCCF(const char *pPath) = 0;
	virtual void SetBufferCount(int nCount) = 0;
};