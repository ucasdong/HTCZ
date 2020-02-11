#pragma once
#include "Common.h"
#include <QObject>
#include <QDebug>

#define PKG_COUNT   1024 * 1024

struct _ID {
	BYTE* id;
	int len;
	int skip;
	int position;
};

typedef void(*PickPkgCallbackFunc)(void *params, int index);//PickPkgContent* Info);

class PickPkg : public QObject
{
public:
	PickPkg();
	~PickPkg();

	_ID           m_RowID;
	_ID           m_PkgID;
	_ID           m_FrameID;

	int           m_buffer_width;
	int           m_buffer_height;
	int           m_byteperpixel;
	//int           m_allCount;
	int           m_count;	
	std::vector<BYTE*> m_buffer_vector;
	int           m_pkg_size;
	BYTE*         m_pkg_vector;
	int           m_DataSize;
	int           m_DataStartPosition;
	int           m_spectrl;
	PickPkgCallbackFunc  m_pFunc;
	void*         m_param;

	bool          m_bFinishOnePkg;
	int           m_headerIndex;
	int           m_PickCopyCount;
	int           m_iCurrentModel;

	void SetRowID(BYTE* id, int len, int position);
	void SetPkgID(BYTE* id, int len, int position);
	void SetPkgFrameID(BYTE* id, int len, int position);
	void SetBufferSize(int x, int y);
	void SetBufferCount(int count);
	void SetBitperpixel(int byte);
	bool CreateBuffer();
	bool CreateBuffer(int width, int height, int count, int byteperpixel);
	void SetDataInfo(int iAllLen, int start, int iValidSize);
	void RegisterCallBack(PickPkgCallbackFunc func, void* param);
	void PushPkg(BYTE* src, int count);
	bool ReadBuffer(BYTE* pdst);
	void changeModel(int iModel);
};

