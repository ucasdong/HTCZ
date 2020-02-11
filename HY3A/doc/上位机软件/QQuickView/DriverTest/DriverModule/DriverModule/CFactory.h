#pragma once

#include "CDriver.h"

class CDriverFactory
{
public:
	CDriverFactory(void){}
	virtual ~CDriverFactory(void){}

public:
	virtual CDriver* CreateDriver() = 0;
	virtual void DestoryDriver() = 0;
};




