#pragma once
#include <SapClassBasic.h>
//#include <SapClassGui.h>
// #include "MFCApplication6.h"
// #include "MyImageExWnd.h"

//#define DEBUG_WANGKUN
//#define NOCARD
// CGrabImage ¶Ô»°¿ò
class MySapBufferRoi : public SapBufferRoi
{
public:
	MySapBufferRoi(SapBuffer *pParent, int xmin = 0, int ymin = 0, int width = -1, int height = -1)
		: SapBufferRoi(pParent, xmin, ymin, width, height) {}
	virtual CORBUFFER GetTrash() const { return m_hTrashChild; }
};


class CmyGrab
{
public:
	CmyGrab();
	~CmyGrab();


public:

	int           m_current_index;
	SapAcquisition*    m_Acq;

	SapBuffer*         m_Buffer;


	SapTransfer*       m_Xfer;

	SapViewScaleParams m_scaleparams;
#ifdef MULTISPECTRL 
	SapBuffer*         m_BufferA[CAMERA_SPECTRL_CNT + SPECTRL_COUNT];
	SapBuffer*         m_BufferA_joint[CAMERA_SPECTRL_CNT + SPECTRL_COUNT];

	CMyImageExWnd*  	   m_ImageWnd_A[CAMERA_SPECTRL_CNT + SPECTRL_COUNT];
	CMyImageExWnd*  	   m_ImageWnd_B[CAMERA_SPECTRL_CNT];
#else
	SapBuffer*         m_BufferA[CAMERA_SPECTRL_CNT];
	SapBuffer*         m_BufferA_joint[CAMERA_SPECTRL_CNT];

	CMyImageExWnd*  	   m_ImageWnd_A[CAMERA_SPECTRL_CNT];
	CMyImageExWnd*  	   m_ImageWnd_B[CAMERA_SPECTRL_CNT];
#endif

	HANDLE             m_savefileA[CAMERA_SPECTRL_CNT];
	HANDLE             m_auxsavefileA[CAMERA_SPECTRL_CNT];

	HANDLE             m_save_static_fileA[CAMERA_SPECTRL_CNT];
	HANDLE             m_auxsave_static_fileA[CAMERA_SPECTRL_CNT];

	int                m_static_line[CAMERA_SPECTRL_CNT];
	HANDLE           m_loadfile;
	ULONGLONG        m_filesize;
	ULONGLONG        m_fileoffside;
	BYTE*             m_bufferfileA[CAMERA_SPECTRL_CNT];
	//	short*             m_bufferfileB[CAMERA_SPECTRL_CNT]; 

	BYTE*             m_auxfileA[CAMERA_SPECTRL_CNT];
	//	BYTE*             m_auxfileB[CAMERA_SPECTRL_CNT]; 
	int               m_save_spectrl[CAMERA_SPECTRL_CNT + 1];
	CString            m_storefolder;
	CString            m_suffix;
	UINT32             m_minWidth;
	UINT32             m_minHeight;
	UINT32             m_bufferWidth;
	UINT32             m_bufferHeight;
	UINT32             m_bufferFormat;
	int                m_range;
	int                m_sapCnt;
	int                m_imgsize;
	CString            m_cameraname;
	int                m_camera;
	int                m_displaycnt;
	CWnd*              m_parentHwnd;
	int                m_displayspectrlA, m_displayspectrlB;
	int                m_storefmt;
	int                m_mergeSpectrl0_A, m_mergeSpectrl1_A, m_mergeSpectrl2_A;
	int                m_mergeSpectrl0_B, m_mergeSpectrl1_B, m_mergeSpectrl2_B;
	float*             m_rms;
	short*             m_average;
	float*             m_snr;
	char *             m_pData;
	short*             m_auxData;
	bool               run_thread;
	HANDLE             m_hProSemaphore;
	HANDLE             m_hProThread;
	bool               m_cameraA_status;
	bool               m_cameraB_status;

	bool              testrunning;
	UINT              m_linecnt;
	BYTE              m_framecnt;

public:
	void init_grab(int camera, CMyImageExWnd* myhwnd, int count = CAMERA_SPECTRL_CNT, CMyImageExWndEventHandler* wnd = NULL);
	void Grab(void);
	void Stop(void);

	static void XferCallback(SapXferCallbackInfo *pInfo);
	static void SignalCallback(SapAcqCallbackInfo *pInfo);
	void GetSignalStatus();
	void GetSignalStatus(SapAcquisition::SignalStatus signalStatus);

	BOOL CreateObjects();
	void OnDestroy();
	BOOL DestroyObjects();

	void MoveWindow(void);
	void FileOpen(const char* fileName);
	void FileOpen(const char* fileName, int offsize);
	bool FileNext();
	int  ReBoot(DWORD verInfo);
	void SplitBuffer();
	void SplitCopyBuffer();
	static unsigned __stdcall ProThread_View0(void *params);
	static unsigned __stdcall ProThread_View1(void *params);
	bool IsGrabbing();
	void testCrad(void);
	void StoptestCard();
	static unsigned __stdcall  XferCallback_test(void *params);
	bool SaveBMP(void);
	int m_Accelerate;
	void Accelerate();
	bool CheckFrameCnt(WORD* data);
	bool CheckLineCnt(WORD* data);
	CString GetFilename(bool longorstatic, int spectrl);
};

