#pragma once

#include "CFactory.h"
#include "../driver/C1500Driver.h"

class C1500DriverFactory
{
public:
	C1500DriverFactory();
	virtual ~C1500DriverFactory();

public:
	C1500Driver* CreateDriver();
	void DestoryDriver();

private:
	C1500Driver* m_p1500Driver;
};