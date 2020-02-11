#include "C1500Factory.h"


C1500DriverFactory::C1500DriverFactory()	
	: m_p1500Driver(NULL)
{

}

C1500DriverFactory::~C1500DriverFactory()
{

}

C1500Driver* C1500DriverFactory::CreateDriver() 
{
	if (m_p1500Driver == NULL)
	{
		m_p1500Driver = new C1500Driver;
	}

	return m_p1500Driver;
}

void C1500DriverFactory::DestoryDriver()
{
	if (m_p1500Driver != NULL)
	{
		delete m_p1500Driver;
		m_p1500Driver = NULL;
	}
}
