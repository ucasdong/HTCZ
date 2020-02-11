#include "CDALSAFactory.h"

CDALSADriverFactory::CDALSADriverFactory() 
	: m_pDalsaDriver(NULL)
{

}

CDALSADriverFactory::~CDALSADriverFactory()
{

}

CDALSADriver* CDALSADriverFactory::CreateDriver() 
{
	if (m_pDalsaDriver == NULL)
	{
		m_pDalsaDriver = new CDALSADriver;
	}

	return m_pDalsaDriver;
}

void CDALSADriverFactory::DestoryDriver()
{
	if (m_pDalsaDriver != NULL)
	{
		delete m_pDalsaDriver;
		m_pDalsaDriver = NULL;
	}
}
 