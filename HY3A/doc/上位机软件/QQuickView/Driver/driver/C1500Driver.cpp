
#include "C1500Driver.h"


C1500Driver::C1500Driver()
{
	m_strCCFPath	= "";
	m_bGrabRun		= false;
	m_nBufferCount  = 2;
}

C1500Driver::~C1500Driver()
{

}

void C1500Driver::SetCCF(const char *pPath)
{
	m_strCCFPath = pPath;
}

void C1500Driver::SetBufferCount(int nCount)
{
	m_nBufferCount = nCount;
}

bool C1500Driver::InitDriver()
{
	return true;
}


void C1500Driver::DestroyDriver()
{

}

int C1500Driver::InitThread()
{
	return 1;
}
