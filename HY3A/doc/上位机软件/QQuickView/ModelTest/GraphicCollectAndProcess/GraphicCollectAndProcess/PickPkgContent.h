#pragma once

#include <windows.h>
#include <vector>
using namespace std;
#define PKG_COUNT   1024 * 1024

#define NO_ERROR    0
#define NO_ROWID    1

struct _ID{
	BYTE* id;
	int len;
	int skip;
	int position;
};
typedef void (*PickPkgCallbackFunc)(void *params,int index);//PickPkgContent* Info);
class PickPkgContent
{
public:
	PickPkgContent(void);
	~PickPkgContent(void);
public:
	void ResetPkg();
	void RegisterType(int type);
	void SetBufferSize(int x,int y);
	void SetBufferCount(int count);
	int  GetBufferIndex();
	BOOL CreateBuffer();
	BOOL CreateBuffer(int width,int height,int count,int byteperpixel);
	BOOL DestroyBuffer();
	BOOL ReadBuffer(BYTE* pdst);
	BOOL ReadBuffer(BYTE* pdst,int index);
	void ShowData();
	void SetPkgSize(int size);
	void SetRowID(BYTE* id,int len,int position);
	void SetPkgID(BYTE* id,int len,int position);
	void SetPkgFrameID(BYTE* id,int len,int position);
	void SetDataInfo(int size,int start);
	void SetBitperpixel(int byte);

	void PushPkg(BYTE* src,int count);
	
	//void CallbackFunc(void);
	void RegisterCallBack(PickPkgCallbackFunc func,void* param);
	static unsigned __stdcall ProThread_Pick(void *params);
		int           m_spectrl;
protected:
	void BufferNext();
	void GeneraterBuffer();
	int  FindDataID();
	void SetSkipPkg(int count);
	void*         m_param;
	int           m_debug_test;
	int           m_searchpoint_InData;
	int           m_skip_pkg;
	int           m_currentIndex;
	int           m_currentposition;
	int           m_currentPkgIndex;
	int           m_PickpkgIndex;
	int           m_check_bytes;
	int           m_count;
	int           m_type;
	int           m_pkg_size;
	_ID           m_RowID;
    _ID           m_PkgID;
	_ID           m_FrameID;
	int           m_DataSize;
	int           m_DataStartPosition;
	int           m_byteperpixel;
	BYTE*         m_pkg_vector;
	vector<BYTE*> m_buffer_vector;
	int           m_buffer_width;
	int           m_buffer_height;
	int           test;
	PickPkgCallbackFunc  m_pFunc;
	UINT          GetLastError();
private:
	UINT          m_lasterror;
public:
	void PickData(void);
};

