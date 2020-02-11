#pragma once

#include "CFactory.h"
#include "../driver/CDALSADriver.h"

class CDALSADriverFactory
{
public:
	CDALSADriverFactory();
	virtual ~CDALSADriverFactory();

public:
	CDALSADriver* CreateDriver();
	void DestoryDriver();

private:
	CDALSADriver* m_pDalsaDriver;
};