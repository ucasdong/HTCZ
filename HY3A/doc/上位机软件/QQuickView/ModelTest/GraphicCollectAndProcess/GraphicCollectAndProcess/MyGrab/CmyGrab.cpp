#include "CmyGrab.h"
#include "FileOperation.h"
#include "DlgMtfCal.h"
CString com_name[]=
{
	_T("相机标识"),_T("m"),_T("r"),_T("增益"),_T("级数"),_T("积分时间"),_T("k"),_T("b"),_T("自定义（默认空）")
};
CmyGrab::CmyGrab()
{
	m_framecnt = 0xff;
	m_linecnt = 0xffffffff;
	m_Accelerate = 0;
	m_displayspectrlA = CAMERA_SPECTRL_ALL;
	m_displayspectrlB = CAMERA_SPECTRL_ALL;
	m_current_index = 0;
	m_Acq		= NULL;
//	m_orgBuffer = NULL;
	m_Buffer	= NULL;
    m_Xfer      = NULL;
   m_loadfile = NULL;	
//	memset(m_linecnt,0,sizeof(int)*10);
	for(int i = 0;i < CAMERA_SPECTRL_CNT; i++){
		m_savefileA[i] = NULL; 
		m_auxsavefileA[i] = NULL; 
		m_bufferfileA[i] =  NULL;
		m_auxfileA[i] =  NULL;
		m_save_static_fileA[i] = NULL; 
	    m_auxsave_static_fileA[i] = NULL;  
		m_static_line[i] = -1;
	}
	m_Accelerate = 1;
}


CmyGrab::~CmyGrab()
{
}

void CmyGrab::XferCallback(SapXferCallbackInfo *pInfo)
{
	CmyGrab *pDlg= (CmyGrab *) pInfo->GetContext();
	int index;
//	char    filename[100];
   // If grabbing in trash buffer, do not display the image, update the
   // appropriate number of frames on the status bar instead
   if (pInfo->IsTrash())
   {
      CString str;
      str.Format(_T("Frames acquired in trash buffer: %d"), pInfo->GetEventCount());
	   AfxMessageBox(str);
     // pDlg->m_statusWnd.SetWindowText(str);
   }

   // Refresh view
   else
   { 
	//   pDlg->run_thread = true;
	   CTime tm = CTime::GetCurrentTime();
	 //  CString strFile,str;
#ifdef  THREAD_AFFINITY
	HANDLE hthread = ::GetCurrentThread(); // 本进程的HANDLE
//	 DWORD dwSysMask, dwProcessMask; // 系统配置的所有处理器，本进程可用的处理器
     ::SetThreadAffinityMask(hthread, MAIN_GRAB_BASE_THREAD1);
#endif 
	 //  int cnt=0;
  //  TRACE("thread grab --------------------------------processing\n");
	   if(!pDlg->run_thread) return;
	  // SetThreadExecutionState(ES_DISPLAY_REQUIRED);
	
	   pDlg->SplitBuffer();
	//   TRACE("grabing----%d---%d\n",tm.GetSecond(),tm.GetTickCount());  		
	   ReleaseSemaphore(pDlg->m_hProSemaphore, 1, NULL);
	   
   }
}
unsigned  CmyGrab::ProThread_View0(void *params){
	 BYTE tmp = 1; 
	 CmyGrab *pDlg= (CmyGrab *) params;
	while(theapp_mainparams.m_application_run){

	   WaitForSingleObject(pDlg->m_hProSemaphore, INFINITE);
	   
	   if(theapp_mainparams.m_window_manager.auxdlg != NULL){
            SendMessage(theapp_mainparams.m_window_manager.auxdlg->GetSafeHwnd(),WM_AUX_WIN,NULL,NULL);
	    }
	   Sleep(500);
	   	//ReleaseSemaphore(theapp_mainparams.m_hProSemaphore, 1, NULL);
		 //  SendMessage(pDlg->m_ImageWnd_A[0]->GetViewportHwnd(),WM_NEW_Semaphore,NULL,NULL);
	//	   SendMessage(pDlg->m_ImageWnd_A[1]->GetViewportHwnd(),WM_NEW_Semaphore,NULL,NULL);
	//	   SendMessage(pDlg->m_ImageWnd_A[2]->GetViewportHwnd(),WM_NEW_Semaphore,NULL,NULL);
	//	   SendMessage(pDlg->m_ImageWnd_A[3]->GetViewportHwnd(),WM_NEW_Semaphore,NULL,NULL);

	}
	return 1;
}

void CmyGrab::SignalCallback(SapAcqCallbackInfo *pInfo)
{
	CmyGrab *pDlg = (CmyGrab *) pInfo->GetContext();
   pDlg->GetSignalStatus(pInfo->GetSignalStatus());
}


void CmyGrab::GetSignalStatus()
{
   SapAcquisition::SignalStatus signalStatus;

   if (m_Acq && m_Acq->IsSignalStatusAvailable())
   {
      if (m_Acq->GetSignalStatus(&signalStatus, SignalCallback, this))
         GetSignalStatus(signalStatus);
   }
}

void CmyGrab::GetSignalStatus(SapAcquisition::SignalStatus signalStatus)
{
  
}

static char servername[] = "Xtium-CL_MX4_1";

void CmyGrab::init_grab(int mode,CMyImageExWnd* myhwnd,int count,CMyImageExWndEventHandler* wnd)
{
	m_sapCnt = 0;
	UINT32 threadId;

	m_cameraname = _T("Z3相机");
	
	m_displaycnt = count;
	
//	m_parentHwnd =  wnd;

	bool ok = theApp.m_reg->SectionExists("param_set");
	if (ok){
		theApp.m_reg->Descend(("param_set"));
		//theApp.m_reg->Restore("store_fmt", m_storefmt);
		theApp.m_reg->Restore("store_filefolder", m_storefolder);
		
		m_suffix =   _storefmt[0];
		m_storefolder += "\\";

		theApp.m_reg->Ascend();
	}
	else{
		m_storefmt = 0;
		m_storefolder = "";
		m_suffix =   _storefmt[0];
	}

//	m_parentHwnd = wnd;
	char serverName[] = "Xtium-CL_MX4_1";
	

	SapLocation *m_loc;
	int port;

	m_loc = new SapLocation(CStringA(serverName),0);

	// Define on-line objects
	if(mode == 0)
		m_Acq = new SapAcquisition(*m_loc,"c_02E_2048_mode_Default_Default.ccf");
	else if(mode == 1)
		m_Acq = new SapAcquisition(*m_loc,"c_02E_4096_mode_Default_Default.ccf");
	else
		m_Acq = new SapAcquisition(*m_loc,"c_02E_test10248_mode_Default_Default.ccf");

	m_Buffer    = new SapBufferWithTrash(BUFFER_COUNT,m_Acq);
	for(int i= 0;i < SPECTRL_COUNT;i++){
	m_BufferA[i]    = new SapBufferWithTrash(BUFFER_COUNT);
	m_BufferA_joint[i] = new SapBufferWithTrash(BUFFER_COUNT);
	}



    m_Xfer      = new SapAcqToBuf(m_Acq, m_Buffer, XferCallback, this);
	
      // Create acquisition object
#ifndef NOCARD
//	m_Acq->ResetServer(serverName);
	if (m_Acq && !*m_Acq && !m_Acq->Create()){
         
		 CString message;
         message.Format(_T("系统没有找到板卡,请重新启动系统"));
		// DestroyObjects();
         if(AfxMessageBox(message,MB_YESNO) == IDYES){
			 ReBoot(0);
		 }
         return ;
    }
	


   //check to see if both acquision devices support scatter gather.
   BOOL acq0SupportSG = SapBuffer::IsBufferTypeSupported(m_Acq->GetLocation(),SapBuffer::TypeScatterGather);

   if (!acq0SupportSG)// || !acq1SupportSG)
   {
      // check if they support scatter gather physical
      BOOL acq0SupportSGP = SapBuffer::IsBufferTypeSupported(m_Acq->GetLocation(),SapBuffer::TypeScatterGatherPhysical);
      
      if (!(!acq0SupportSG && acq0SupportSGP))
      {
         CString message;
         message.Format(_T("The chosen acquisition devices\n\n-%s\n\ndo not support similar buffer types.\nThe demo will now close."),(LPCTSTR)m_Acq->GetLocation().GetServerName());
         AfxMessageBox(message);
         return ;
      }
   }
#endif
	// Define other objects
//   m_minWidth = 12096 - AUX_STARTX;
//   m_minHeight = 512;
 //  m_minHeight = 2593;
//   m_minWidth = 9224;
//    myhwnd[0].AttachSapBuffer(m_BufferA);
//	myhwnd[1].AttachSapBuffer(m_BufferB);
	
	// Create all objects
	if (!CreateObjects()) 
   { 
     return ; 
   }
	int width = m_Buffer->GetWidth() / 2;
	int height = m_Buffer->GetHeight(); 
	int buffersize = width * height;
	
	if((buffersize % 512) != 0)
		buffersize = (buffersize / 512) * 512 + 512;
	m_pData = (char*)malloc(buffersize);
	memset(m_pData,0,buffersize);
	m_auxData = (short*)malloc(sizeof(short)*width * height);
	for(int i = 0;i<m_displaycnt ;i++){
		myhwnd[i].SetWindowSize(width,IMAGE_HEIGHT);
		myhwnd[i].AttachEventHandler(wnd);
		myhwnd[i].AttachSapBuffer(m_BufferA[i],m_BufferA_joint[i]);
        myhwnd[i].CenterImage(true);
		myhwnd[i].SetActive(true);
        myhwnd[i].Reset(); 
		myhwnd[i].SetCameraSpectrl(i / SPECTRL_COUNT,i % SPECTRL_COUNT);
		m_ImageWnd_A[i] = &myhwnd[i];
	}
#ifdef MULTISPECTRL 
	for(int i = 0;i<SPECTRL_COUNT ;i++){
		myhwnd[i + m_displaycnt].SetWindowSize(IMAGE_WIDHT * 3,IMAGE_HEIGHT);
		myhwnd[i + m_displaycnt].AttachEventHandler(wnd);
		myhwnd[i + m_displaycnt].AttachSapBuffer(m_BufferA[i + m_displaycnt],m_BufferA_joint[i + m_displaycnt]);
        myhwnd[i + m_displaycnt].CenterImage(true);
		myhwnd[i + m_displaycnt].SetActive(true);
        myhwnd[i + m_displaycnt].Reset(); 
		myhwnd[i + m_displaycnt].SetCameraSpectrl(3,i);
		
		m_ImageWnd_A[i + m_displaycnt] = &myhwnd[i + m_displaycnt];
	}
#endif
	m_hProSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

	
	m_hProThread = (HANDLE) _beginthreadex(NULL, 0, ProThread_View0, this,CREATE_SUSPENDED , &threadId);
//	SetThreadAffinityMask(m_hProThread,MAIN_GRAB_BASE_THREAD1);
	g_threader.InsertThread(m_hProThread); 
	ResumeThread(m_hProThread);
}


BOOL CmyGrab::CreateObjects()
{
	CWaitCursor wait;

#ifndef NOCARD	
   m_bufferWidth  = m_Acq->GetXferParams().GetWidth() / 2;
   m_bufferHeight = 64;//m_Acq->GetXferParams().GetHeight();
   m_bufferFormat = m_Acq->GetXferParams().GetFormat();


    m_average =  (short*)malloc(sizeof(short)*m_bufferWidth);
	m_rms     =  (float*)malloc(sizeof(short)*m_bufferWidth);
	m_snr     =  (float*)malloc(sizeof(short)*m_bufferWidth);


    m_scaleparams.SetHeight(m_bufferHeight);
	m_scaleparams.SetLeft(0);
	m_scaleparams.SetTop(0);
	m_scaleparams.SetWidth(m_bufferWidth);

    run_thread = false;
   m_imgsize = m_bufferWidth * m_bufferHeight;

   // Create buffer object
   if (SapBuffer::IsBufferTypeSupported(m_Acq->GetLocation(),SapBuffer::TypeScatterGather))
      m_Buffer->SetParameters(BUFFER_COUNT,  m_Acq->GetXferParams().GetWidth(), m_Acq->GetXferParams().GetHeight(), m_bufferFormat, SapBuffer::TypeScatterGather);
   else if (SapBuffer::IsBufferTypeSupported(m_Acq->GetLocation(),SapBuffer::TypeScatterGatherPhysical))
      m_Buffer->SetParameters(BUFFER_COUNT,  m_Acq->GetXferParams().GetWidth(), m_Acq->GetXferParams().GetHeight(), m_bufferFormat, SapBuffer::TypeScatterGatherPhysical);
   
   m_Buffer->SetPixelDepth( m_Acq->GetXferParams().GetPixelDepth());


#else
    m_bufferWidth  = m_Buffer->GetWidth();
   m_bufferHeight = IMAGE_HEIGHT;
   m_bufferFormat =SapFormatMono16;
 //  m_Buffer->SetParameters(1,  m_bufferWidth, m_bufferHeight, m_bufferFormat, SapBuffer::TypeScatterGatherPhysical);
    m_average =  (short*)malloc(sizeof(short)*m_bufferWidth);
	m_rms     =  (float*)malloc(sizeof(short)*m_bufferWidth);
	m_snr     =  (float*)malloc(sizeof(short)*m_bufferWidth);


#endif
   if (m_Buffer && !*m_Buffer && !m_Buffer->Create()) 
   {
      DestroyObjects();
      return FALSE;
   }
  
#ifdef MULTISPECTRL 
   for(int i = 0;i<CAMERA_SPECTRL_CNT;i++){
		m_BufferA[i]->SetParameters(1,  m_bufferWidth, m_bufferHeight , SapFormatMono16, SapBuffer::TypeScatterGatherPhysical);
		if (m_BufferA[i] && !*m_BufferA[i] && !m_BufferA[i]->Create()){
			DestroyObjects();
			return FALSE;
		}
	}
   for(int i = 0;i< SPECTRL_COUNT;i++){
		m_BufferA[i + CAMERA_SPECTRL_CNT]->SetParameters(1,  m_bufferWidth * 3, m_bufferHeight , SapFormatMono16, SapBuffer::TypeScatterGatherPhysical);
		if (m_BufferA[i + CAMERA_SPECTRL_CNT] && !*m_BufferA[i + CAMERA_SPECTRL_CNT] && !m_BufferA[i + CAMERA_SPECTRL_CNT]->Create()){
			DestroyObjects();
			return FALSE;
		}
	}
   theapp_mainparams.m_joint_pixelnumber = 200;
   for(int i = 0;i<CAMERA_SPECTRL_CNT + SPECTRL_COUNT;i++){
	   m_BufferA_joint[i]->SetParameters(1,  theapp_mainparams.m_joint_pixelnumber, m_bufferHeight , m_BufferA[i]->GetFormat(), SapBuffer::TypeScatterGatherPhysical);
		if (m_BufferA_joint[i] && !*m_BufferA_joint[i] && !m_BufferA_joint[i]->Create()){
			DestroyObjects();
			return FALSE;
		}
	}
#else
   for(int i = 0;i<CAMERA_SPECTRL_CNT;i++){
		m_BufferA[i]->SetParameters(1,  m_bufferWidth, m_bufferHeight , SapFormatMono16, SapBuffer::TypeScatterGatherPhysical);
		if (m_BufferA[i] && !*m_BufferA[i] && !m_BufferA[i]->Create()){
			DestroyObjects();
			return FALSE;
		}
	}
    theapp_mainparams.m_joint_pixelnumber = 200;
   for(int i = 0;i<CAMERA_SPECTRL_CNT;i++){
	   m_BufferA_joint[i]->SetParameters(1,  theapp_mainparams.m_joint_pixelnumber, m_bufferHeight , SapFormatMono16, SapBuffer::TypeScatterGatherPhysical);
		if (m_BufferA_joint[i] && !*m_BufferA_joint[i] && !m_BufferA_joint[i]->Create()){
			DestroyObjects();
			return FALSE;
		}
	}
#endif
/*	m_BufferB->SetParameters(1,  m_bufferWidth, m_bufferHeight, m_bufferFormat, SapBuffer::TypeScatterGatherPhysical);
	 if (m_BufferB && !*m_BufferB && !m_BufferB->Create()) 
   {
      DestroyObjects();
      return FALSE;
   }
  */
  
	 

   // Create view object
#ifndef NOCARD
   // Create transfer object
   if (m_Xfer && !*m_Xfer && !m_Xfer->Create()) 
   {
      DestroyObjects();
      return FALSE;
   }
#endif

   return TRUE;
}

BOOL CmyGrab::DestroyObjects()
{
	BOOL bResult= TRUE;

	if (m_Xfer && *m_Xfer)
      bResult&= m_Xfer->Destroy();

    if(m_bufferfileA[0] != NULL)
	   free(m_bufferfileA[0]);
    if(m_auxfileA[0] != NULL)
	   free(m_auxfileA[0]);

    if (m_Buffer && *m_Buffer )
       bResult&=  m_Buffer->Destroy();
#ifdef MULTISPECTRL
    for(int i = 0;i<CAMERA_SPECTRL_CNT + SPECTRL_COUNT;i++){
		  if (m_BufferA[i] && *m_BufferA[i])
				bResult&=  m_BufferA[i]->Destroy();
	  	  if (m_BufferA_joint[i] && *m_BufferA_joint[i])
				bResult&=  m_BufferA_joint[i]->Destroy();
	
	}
#else
	for(int i = 0;i<CAMERA_SPECTRL_CNT;i++){
		  if (m_BufferA[i] && *m_BufferA[i])
				bResult&=  m_BufferA[i]->Destroy();
		   if (m_BufferA_joint[i] && *m_BufferA_joint[i])
				bResult&=  m_BufferA_joint[i]->Destroy();
	   }
#endif
     
//   if (m_BufferB && *m_BufferB )
//      bResult&=  m_BufferB->Destroy();

   // Destroy acquisition object
   if (m_Acq && *m_Acq )
      bResult&= m_Acq->Destroy();

   
   return bResult;
}


void CmyGrab::OnDestroy() 
{
	// Delete all objects
   if (m_ImageWnd_A)     delete m_ImageWnd_A;
   if (m_ImageWnd_B)     delete m_ImageWnd_B;

   if (m_Xfer)         delete m_Xfer; 
   if (m_Buffer)	   delete m_Buffer; 
  // if (m_BufferB)	   delete m_BufferB; 
   for(int i = 0;i<CAMERA_SPECTRL_CNT;i++)
   if (m_BufferA[i])	   delete m_BufferA[i]; 

   if (m_Acq)		   delete m_Acq; 
   
}


void CmyGrab::Grab(void)
{
	bool ok = theApp.m_reg->SectionExists("param_set");
	if (ok){
		theApp.m_reg->Descend(("param_set"));
		//theApp.m_reg->Restore("store_fmt", m_storefmt);
		theApp.m_reg->Restore("store_filefolder", m_storefolder);
		
		m_suffix =   _storefmt[0];
		m_storefolder += "\\";

		theApp.m_reg->Ascend();
	}
#ifndef NOCARD
	if( !m_Xfer->Grab())// && m_Xfer[1]->Grab())
	{
		 CString message;
         message.Format(_T("板卡启动出错,请重新启动系统"));
		// DestroyObjects();
         if(AfxMessageBox(message,MB_YESNO) == IDYES){
			 ReBoot(0);
		 }
         return ;
	}
/*		CTime tm = CTime::GetCurrentTime();
 CString strFile,str;
	if(theapp_mainparams.m_autosave){
		strFile.Format(_T("%s相机-时刻-%2d-%2d-%2d.raw"),m_storefolder,tm.GetHour(), tm.GetMinute(), tm.GetSecond());
		m_savefileA[0] = CreateFile(strFile,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
		if(m_savefileA[0] == INVALID_HANDLE_VALUE)
		       return ;
	    if(theapp_mainparams.m_split_save){
		 	if(theapp_mainparams.m_aux_save){
			    strFile.Format(_T("%s相机辅助数据-时刻-%2d-%2d-%2d.raw"),m_storefolder,tm.GetHour(), tm.GetMinute(), tm.GetSecond());
			    m_auxsavefileA[0] = CreateFile(strFile,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
		        if(m_auxsavefileA[0] == INVALID_HANDLE_VALUE)
		            return ;
		    }
        }
	}
	*/
	run_thread = true;

#endif
}


void CmyGrab::Stop(void)
{
#ifndef NOCARD
//	wchar_t ch[10];
//	while(run_thread);
//	while(run_thread);

   
	run_thread = false;
	if( m_Xfer->Freeze())// && m_Xfer[1]->Freeze())
	{
		if (CAbortDlg(theapp_mainparams.m_window_manager.grabdlg, m_Xfer).DoModal() != IDOK) 
				m_Xfer->Abort();
     }
	 CloseHandle(m_savefileA[0]);
	 CloseHandle(m_auxsavefileA[0]);
//	while(run_thread);
	
#endif
//	_itow_s(m_sapCnt,ch,10);
//	 AfxMessageBox(ch);
}


void CmyGrab::MoveWindow(void)
{

}

void CmyGrab::FileOpen(const char* fileName,int offsize){
	char* pData;
	DWORD size = m_Buffer->GetWidth() * m_Buffer->GetHeight();
	DWORD cnt= 0;
	CFileStatus fileStatus;
	char tmp[248];
	m_Buffer->GetAddress((void**)&pData);
	theapp_mainparams.m_filename = fileName;

	if(m_loadfile != INVALID_HANDLE_VALUE)
		CloseHandle(m_loadfile);;
	
	m_loadfile = CreateFile(fileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	

	//HANDLE hFile=CreateFile("1.txt",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	CFile::GetStatus(fileName,fileStatus);
	m_filesize = fileStatus.m_size;
	DWORD dwError = GetLastError() ; 
	m_fileoffside = 0;
	//_llseek((HFILE)m_loadfile,offsize,FILE_BEGIN);
	OVERLAPPED vlp;
	memset(&vlp,0,sizeof(OVERLAPPED));
	vlp.Offset = offsize;
	ReadFile(m_loadfile,pData,size,&cnt,&vlp);

	m_fileoffside = size;
	SplitBuffer();	
}
void CmyGrab::FileOpen(const char* fileName)
{
	//m_Buffer->SetPixelDepth(16);
	//CString m_Options;
//	m_Options.Format(_T("-format raw -width %d -height %d -offset %d"), m_bufferWidth, m_bufferHeight, 0);
//	m_Buffer->SetPixelDepth(CORDATA_FORMAT_MONO16);
	char* pData;
	DWORD size = m_Buffer->GetWidth() * m_Buffer->GetHeight();
	DWORD cnt= 0;
	CFileStatus fileStatus;
	char tmp[248];
	m_Buffer->GetAddress((void**)&pData);
	theapp_mainparams.m_filename = fileName;

	if(m_loadfile != INVALID_HANDLE_VALUE)
		CloseHandle(m_loadfile);;
	
	m_loadfile = CreateFile(fileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	

	//HANDLE hFile=CreateFile("1.txt",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	CFile::GetStatus(fileName,fileStatus);
	m_filesize = fileStatus.m_size;
	DWORD dwError = GetLastError() ; 
	m_fileoffside = 0;
	ReadFile(m_loadfile,pData,size,&cnt,NULL);

	m_fileoffside = size;
	SplitBuffer();	
}
void CmyGrab::Accelerate(){
	m_Accelerate++;
}
bool CmyGrab::FileNext(){
	bool status = true;
	char* pData;
	DWORD size = m_Buffer->GetWidth() * m_Buffer->GetHeight();
	DWORD cnt= 0;
	char tmp[248]; 
	m_Buffer->GetAddress((void**)&pData);
	theapp_mainparams.m_fileoffside = m_fileoffside;
//	if(m_filesize < m_fileoffside)
//		return false;
	while(cnt <m_Accelerate){
		ReadFile(m_loadfile,pData,size,&cnt,NULL);
		cnt++;
	}
	SplitBuffer();
//	ReadFile(m_loadfile,tmp,248,&cnt,NULL);
	m_fileoffside += size;
	
	return status;
}
int CmyGrab::ReBoot(DWORD verInfo)
{
	OSVERSIONINFO osver={sizeof(OSVERSIONINFO)};
    GetVersionEx(&osver);
     
    if(osver.dwMajorVersion>=5)
    {
        HANDLE ToHandle;
        TOKEN_PRIVILEGES tkp;

        if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&ToHandle))
       {
             LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
             tkp.PrivilegeCount=1;
             tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;

              AdjustTokenPrivileges(ToHandle,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);
             //获得权限后关闭计算机
             ExitWindowsEx(EWX_REBOOT,0);
       }
    }
    else
    {
        ExitWindowsEx(EWX_REBOOT,0);
     }
     return 0;
}


void CmyGrab::SplitCopyBuffer(){
	short* pData;
	
	int width = m_Buffer->GetWidth() - 1 - ACTUAL_START;
	int height= m_Buffer->GetHeight();
	int start = 0;
	int actual_width = width;
	int writecnt;
	int childheight = m_Buffer->GetHeight();
    run_thread = true;

	run_thread = false;
	return;
}
CString CmyGrab::GetFilename(bool longorstatic,int spectrl){
	CString str1,str = "";
	CFileOperation op;
		if(spectrl == 0)
			str1.Format("左1半视场");
		else if(spectrl == 1)
			str1.Format("左2半视场");
		else if(spectrl == 2)
			str1.Format("右1半视场");
		else
			str1.Format("右2半视场");
		str += str1;
    if((theapp_mainparams.m_long_save.m_aux_analysis) && longorstatic){
		str = theapp_mainparams.m_long_save.m_save_dir;
		//if(spectrl < 100)
	
		str1.Format(_T("增益%2x-曝光时间%x.raw"),g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 12]/256,g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 20]/256);
		str += str1;
		str += ".raw";
	}
	else if((theapp_mainparams.m_single_save.m_aux_analysis) && (longorstatic == false)){
		str = theapp_mainparams.m_single_save.m_static_save_dir;
	
	
		
		str1.Format(_T("增益%2x-曝光时间%x.raw"),g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 12]/256,g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 20]/256);
		str += str1;
		str += ".raw";
	}
	else{
		CTime tm = CTime::GetCurrentTime();
		if(theapp_mainparams.m_long_save.m_autosave){
			str = theapp_mainparams.m_long_save.m_save_dir;
	//		str += "\\";
			str += theapp_mainparams.m_filename_ruler.m_first_str;
		}
		else{
			str = theapp_mainparams.m_single_save.m_static_save_dir;
//			str += "\\";
			str += theapp_mainparams.m_filename_ruler.m_first_str;

		}
		str += str1;
		str += ".raw";
		
	}
	return str;
}

void CmyGrab::SplitBuffer()
{
	WORD *pData,*pData1;
	
	int width = m_Buffer->GetWidth() ;
	int height= m_Buffer->GetHeight();
	DWORD size;
    DWORD value;
	CTime tm = CTime::GetCurrentTime();
    CString strFile,str;
	BYTE* tmp;
	int index ;
	int skip;
	int buffersize = width * height * m_Buffer->GetBytesPerPixel();
	
	if((buffersize % 512) != 0)
		buffersize = (buffersize / 512) * 512 +  512;

	pData = (WORD* )m_pData;
	index = 0;	
	//m_Buffer->ReadRect(0,0,10,6,pData);
	m_Buffer->GetAddress((void**)&pData);

	if(pData[0] == 0x3040){
		index = 0;
	}
	else if(pData[0] == 0x3080){
		index = 1;
	}
	else if(pData[0] == 0x30c0){
		index = 2;
	}
	else if(pData[0] == 0x3100){
		index = 3;
	}
	else 
		index = 0;

	m_BufferA[index]->GetAddress((void**)&pData1);
	
	memcpy(&pData1[1],&pData[1],m_Buffer->GetWidth() * m_BufferA[index]->GetHeight() * m_Buffer->GetBytesPerPixel());
	for(int i = 0; i < m_BufferA[index]->GetHeight();i++){
		pData1[m_Buffer->GetWidth() * i / 2] = 0x3240;
	}
	SetEvent(m_ImageWnd_A[index]->m_viewportWnd.m_hProSemaphore);
	int i= 0;
	if(theapp_mainparams.m_long_save.m_autosave){
		i = index;
		if(theapp_mainparams.m_long_save.m_save_type & (1 << index)){
			    strFile = GetFilename(true,i);
				if(m_savefileA[i] == NULL){
					m_savefileA[i] = CreateFile(strFile,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
				}
				else{
					buffersize = m_BufferA[0]->GetWidth() * m_BufferA[0]->GetHeight() * m_BufferA[0]->GetBytesPerPixel();
					m_BufferA[i]->GetAddress((void**)&pData1);
					WriteFile(m_savefileA[i],pData1,buffersize,&size,NULL);
					FlushFileBuffers(m_savefileA[i]); 
				}
		}
		
		if(theapp_mainparams.m_long_save.m_save_aux){
			if(m_auxsavefileA[0] == NULL){
					if(theapp_mainparams.m_long_save.m_aux_analysis)
						strFile.Format(_T("%s\\相机辅助数据-时刻-%2d-%2d-%2d-增益%2x-曝光时间%x.raw"),theapp_mainparams.m_long_save.m_save_dir,tm.GetHour(), tm.GetMinute(), tm.GetSecond(),g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 12 + index]/256,g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 20 + index]/256);
					else
						strFile.Format(_T("%s\\相机辅助数据-时刻-%2d-%2d-%2d.raw"),theapp_mainparams.m_long_save.m_save_dir,tm.GetHour(), tm.GetMinute(), tm.GetSecond());
					m_auxsavefileA[0] = CreateFile(strFile,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
				}
				else{
					buffersize =  AUXVALUE_LENGTH * m_Buffer->GetHeight() * m_Buffer->GetBytesPerPixel();
					m_Buffer->ReadRect(0,0,AUXVALUE_LENGTH,m_Buffer->GetHeight(),pData);
					WriteFile(m_auxsavefileA[0],pData,buffersize,&size,NULL);
					FlushFileBuffers(m_auxsavefileA[0]); 
				}
		}
	}
	else{
		for(int indexj= 0;indexj < CAMERA_SPECTRL_CNT;indexj++){
			if(m_savefileA[indexj] != NULL){
				CloseHandle(m_savefileA[indexj]);
				m_savefileA[indexj] = NULL;
			}
			if(m_auxsavefileA[indexj] != NULL){
				CloseHandle(m_auxsavefileA[indexj]);
				m_auxsavefileA[indexj] = NULL;
			}
		}
	}

	if(theapp_mainparams.m_single_save.m_static_save){
		long t = GetTickCount();
		CTime time = CTime::GetCurrentTime();
		if(theapp_mainparams.m_single_save.m_save_appointment < time){
			    i = index;
				if(m_static_line[index] == -1){
					if(theapp_mainparams.m_single_save.m_save_type & ( 1 << index)){
							m_bufferfileA[i] = (BYTE*)malloc(theapp_mainparams.m_single_save.m_static_save_line * m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel());
							m_static_line[i] = 0;
							if(theapp_mainparams.m_single_save.m_static_save_aux){
								m_auxfileA[index] = (BYTE*)malloc(theapp_mainparams.m_single_save.m_static_save_line * AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel());
							}
					}
				}
				else{
					if(m_static_line[index]  < theapp_mainparams.m_single_save.m_static_save_line){
							buffersize = m_BufferA[i]->GetWidth() * m_BufferA[i]->GetHeight() * m_BufferA[i]->GetBytesPerPixel();
							tmp = m_bufferfileA[i] + m_static_line[i] * m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel();
							if(theapp_mainparams.m_single_save.m_static_save_line <= (m_static_line[i] + m_Buffer->GetHeight())){
								memcpy(tmp,pData,m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel() * (theapp_mainparams.m_single_save.m_static_save_line - m_static_line[index]));
							}
							else{
								memcpy(tmp,pData,buffersize);
							}
							m_static_line[i] += m_Buffer->GetHeight() - 1;
						
						if(theapp_mainparams.m_single_save.m_static_save_aux){
							m_Buffer->ReadRect(0,0,AUXVALUE_LENGTH,m_Buffer->GetHeight() - 1,pData);
							tmp = m_auxfileA[index] + m_static_line[index] * AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel();
							memcpy(tmp,pData,AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel() * (theapp_mainparams.m_single_save.m_static_save_line - m_static_line[index]));
						}	
					}
					else{
						if(m_static_line[index] < theapp_mainparams.m_single_save.m_static_save_line)
							return;
						
						CFile file;
						i = index;
					
						if(theapp_mainparams.m_single_save.m_save_type & ( 1 << index)){
								strFile = GetFilename(false,i);
								if(theapp_mainparams.m_single_save.m_static_fmt == 0){
									if(!file.Open(strFile,CFile::modeCreate|CFile::modeWrite))
										return;
									file.Write(m_bufferfileA[i],m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel() * theapp_mainparams.m_single_save.m_static_save_line);
									file.Close();
									TRACE("%s\n",strFile);
								}
								
								m_static_line[i] = -1;
								free(m_bufferfileA[i]);
								m_bufferfileA[i] = NULL;
								theapp_mainparams.m_single_save.m_save_type = theapp_mainparams.m_single_save.m_save_type - (1 << index);
						}
						
						if(theapp_mainparams.m_single_save.m_static_save_aux){
							strFile.Format(_T("%s辅助数据.raw"),theapp_mainparams.m_single_save.m_static_save_dir);
							CFile file;
							if(!file.Open(strFile,CFile::modeCreate|CFile::modeWrite))
								return;
							file.Write(m_auxfileA[index],AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel() * theapp_mainparams.m_single_save.m_static_save_line);
							file.Close();
							free(m_auxfileA[index]);
							m_auxfileA[index] = NULL;
						}
						//}
				    if(theapp_mainparams.m_single_save.m_save_type == 0){
						theapp_mainparams.m_single_save.m_static_save = false;
						if(theapp_mainparams.m_single_save.m_static_msg)
							 AfxMessageBox("图像保存结束！");
					}
				}
					}
				}	
	}
#ifdef THREE_CCD
	value = pData[1] * 256;
	value += pData[0];
	value = value >> 6;
	
	index = value & 0xf - 1;
	if(index > 2)
		return;
	m_BufferA[index]->GetAddress((void**)&pData1);
	memcpy(pData1,pData,m_BufferA[index]->GetWidth()*m_BufferA[index]->GetHeight() * m_BufferA[index]->GetBytesPerPixel());
	
	
	if((index)  == theapp_mainparams.m_spectrl){
		EnterCriticalSection(&g_Aux_Value.cs);
		for(int i = 0; i < 15 ;i++)
			memcpy(&g_Aux_Value.Aux_Value[i * AUXVALUE_LENGTH],&pData[(m_Buffer->GetWidth() * i)],2 * AUXVALUE_LENGTH);
		LeaveCriticalSection(&g_Aux_Value.cs);
	}
	//ReleaseSemaphore(m_hProSemaphore, 1, NULL);
	//PostMessage(m_ImageWnd_A[index]->GetViewportHwnd(),WM_NEW_Semaphore,NULL,NULL);
	SetEvent(m_ImageWnd_A[index]->m_viewportWnd.m_hProSemaphore);
//	int i= 0;
	if(theapp_mainparams.m_long_save.m_autosave){
		i = index;
		if(theapp_mainparams.m_long_save.m_save_type == SAVE_SPECTRL_TYPE){
			    strFile = GetFilename(true,i);
				if(m_savefileA[i] == NULL){
					m_savefileA[i] = CreateFile(strFile,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
				}
				else{
					buffersize = m_BufferA[0]->GetWidth() * m_BufferA[0]->GetHeight() * m_BufferA[0]->GetBytesPerPixel();
					m_BufferA[i]->GetAddress((void**)&pData1);
					WriteFile(m_savefileA[i],pData1,buffersize,&size,NULL);
					FlushFileBuffers(m_savefileA[i]); 
				}
		}
		else{
				if(m_savefileA[0] == NULL){
					strFile = GetFilename(true,255);
					m_savefileA[0] = CreateFile(strFile,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
				}
				else{
					buffersize = m_Buffer->GetWidth() * m_Buffer->GetHeight() * m_Buffer->GetBytesPerPixel();
					WriteFile(m_savefileA[0],pData,buffersize,&size,NULL);
					FlushFileBuffers(m_savefileA[0]); 
				}
			}
		if(theapp_mainparams.m_long_save.m_save_aux){
			if(m_auxsavefileA[0] == NULL){
					if(theapp_mainparams.m_long_save.m_aux_analysis)
						strFile.Format(_T("%s\\相机辅助数据-时刻-%2d-%2d-%2d-增益%2x-曝光时间%x.raw"),theapp_mainparams.m_long_save.m_save_dir,tm.GetHour(), tm.GetMinute(), tm.GetSecond(),g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 12 + index]/256,g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 20 + index]/256);
					else
						strFile.Format(_T("%s\\相机辅助数据-时刻-%2d-%2d-%2d.raw"),theapp_mainparams.m_long_save.m_save_dir,tm.GetHour(), tm.GetMinute(), tm.GetSecond());
					m_auxsavefileA[0] = CreateFile(strFile,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
				}
				else{
					buffersize =  AUXVALUE_LENGTH * m_Buffer->GetHeight() * m_Buffer->GetBytesPerPixel();
					m_Buffer->ReadRect(0,0,AUXVALUE_LENGTH,m_Buffer->GetHeight(),pData);
					WriteFile(m_auxsavefileA[0],pData,buffersize,&size,NULL);
					FlushFileBuffers(m_auxsavefileA[0]); 
				}
		}
	}
	else{
		for(int indexj= 0;indexj < CAMERA_SPECTRL_CNT;indexj++){
			if(m_savefileA[indexj] != NULL){
				CloseHandle(m_savefileA[indexj]);
				m_savefileA[indexj] = NULL;
			}
			if(m_auxsavefileA[indexj] != NULL){
				CloseHandle(m_auxsavefileA[indexj]);
				m_auxsavefileA[indexj] = NULL;
			}
		}
	}

	if(theapp_mainparams.m_single_save.m_static_save){
		long t = GetTickCount();
		CTime time = CTime::GetCurrentTime();
		if(theapp_mainparams.m_single_save.m_save_appointment < time){
			    i = index;
				if(m_static_line[index] == -1){
					if(theapp_mainparams.m_single_save.m_save_type == SAVE_SPECTRL_TYPE){
							m_bufferfileA[i] = (BYTE*)malloc(theapp_mainparams.m_single_save.m_static_save_line * m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel());
							m_static_line[i] = 0;
					}
					else{
						m_bufferfileA[index] = (BYTE*)malloc(theapp_mainparams.m_single_save.m_static_save_line * m_Buffer->GetWidth() * m_Buffer->GetBytesPerPixel());
						if(theapp_mainparams.m_single_save.m_static_save_aux){
							m_auxfileA[index] = (BYTE*)malloc(theapp_mainparams.m_single_save.m_static_save_line * AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel());
						}
						m_static_line[index] = 0;
					}
					if(theapp_mainparams.m_single_save.m_static_save_aux){
						m_auxfileA[0] = (BYTE*)malloc(theapp_mainparams.m_single_save.m_static_save_line * AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel());
					}
				}
				else{
					if(m_static_line[index]  < theapp_mainparams.m_single_save.m_static_save_line){
						if(theapp_mainparams.m_single_save.m_save_type == SAVE_SPECTRL_TYPE){
							buffersize = m_BufferA[i]->GetWidth() * m_BufferA[i]->GetHeight();// * m_BufferA[i]->GetBytesPerPixel();
							tmp = m_bufferfileA[i] + m_static_line[i] * m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel();
							if(theapp_mainparams.m_single_save.m_static_save_line <= (m_static_line[i] + m_Buffer->GetHeight())){
								memcpy(tmp,pData,m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel() * (theapp_mainparams.m_single_save.m_static_save_line - m_static_line[index]));
							}
							else{
								memcpy(tmp,pData,buffersize);
							}
							m_static_line[i] += m_Buffer->GetHeight() - 1;
						}
						else{
							buffersize = m_Buffer->GetWidth() * (m_Buffer->GetHeight() - 1) * m_Buffer->GetBytesPerPixel();
							tmp = m_bufferfileA[0] + m_static_line[0] * m_Buffer->GetWidth() * m_Buffer->GetBytesPerPixel();
							if(theapp_mainparams.m_single_save.m_static_save_line <= (m_static_line[0] + m_Buffer->GetHeight())){
								memcpy(tmp,pData,m_Buffer->GetWidth() * m_Buffer->GetBytesPerPixel() * (theapp_mainparams.m_single_save.m_static_save_line - m_static_line[0]));
							}
							else{
								memcpy(tmp,pData,buffersize);
							}
							m_static_line[0] += m_Buffer->GetHeight() - 1;
						}
						if(theapp_mainparams.m_single_save.m_static_save_aux){
							m_Buffer->ReadRect(0,0,AUXVALUE_LENGTH,m_Buffer->GetHeight() - 1,pData);
							tmp = m_auxfileA[index] + m_static_line[index] * AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel();
							memcpy(tmp,pData,AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel() * (theapp_mainparams.m_single_save.m_static_save_line - m_static_line[index]));
						}	
					}
					else{
						
						if(m_static_line[index] < theapp_mainparams.m_single_save.m_static_save_line)
							return;
						
						CFile file;
						i = index;
						//for(int i = 0;i < CAMERA_SPECTRL_CNT;i++){
						CString savefile = theapp_mainparams.m_single_save.m_static_save_dir;
						if(theapp_mainparams.m_single_save.m_aux_analysis){
							savefile.Format("%s增益%2x-曝光时间%2x",savefile,g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 12 + index]/256,g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 20 + index]/256);
						}
						if(theapp_mainparams.m_single_save.m_save_type == SAVE_SPECTRL_TYPE){
								strFile = GetFilename(true,i);
								if(theapp_mainparams.m_single_save.m_static_fmt == 0){
									if(!file.Open(strFile,CFile::modeCreate|CFile::modeWrite))
										return;
									file.Write(m_bufferfileA[i],m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel() * theapp_mainparams.m_single_save.m_static_save_line);
									file.Close();
									TRACE("%s\n",strFile);
								}
								else if(theapp_mainparams.m_single_save.m_static_fmt == 2){
									CDibImage m_image;
									WORD      ss;
									m_image.Create(CSize(m_BufferA[i]->GetWidth(),theapp_mainparams.m_single_save.m_static_save_line),8);
									tmp = (BYTE*)m_image.GetDIBBits();
									for(int k = 0;k < m_BufferA[i]->GetWidth();k++){
										for(int j = 0;j < theapp_mainparams.m_single_save.m_static_save_line;j++){
											ss = *(m_bufferfileA[i] + (j * m_BufferA[i]->GetWidth() + k)  * m_BufferA[i]->GetBytesPerPixel());
											ss = ss * 256 + *(m_bufferfileA[i] + (j * m_BufferA[i]->GetWidth() + k)  * m_BufferA[i]->GetBytesPerPixel() + 1);
											if(theapp_mainparams.m_display_para.m_displaybit >= 7)
												tmp[j * m_BufferA[i]->GetWidth() + k] = ss >> (theapp_mainparams.m_display_para.m_displaybit - 7);
											else
												tmp[j * m_BufferA[i]->GetWidth() + k] = ss << (7 - theapp_mainparams.m_display_para.m_displaybit);
										}
									}
									m_image.Save(strFile);
								}
								else{
	
								}
								m_static_line[i] = -1;
								free(m_bufferfileA[i]);
								m_bufferfileA[i] = NULL;
							
						}
						else{
								strFile = GetFilename(true,255);
								if(theapp_mainparams.m_single_save.m_static_fmt == 0){
									if(!file.Open(strFile,CFile::modeCreate|CFile::modeWrite))
										return;
									file.Write(m_bufferfileA[0],m_Buffer->GetWidth() * m_Buffer->GetBytesPerPixel() * theapp_mainparams.m_single_save.m_static_save_line);
									file.Close();
									TRACE("%s\n",strFile);
								}
								else if(theapp_mainparams.m_single_save.m_static_fmt == 2){
									CDibImage m_image;
									WORD      ss;
									m_image.Create(CSize(m_Buffer->GetWidth(),theapp_mainparams.m_single_save.m_static_save_line),8);
									tmp = (BYTE*)m_image.GetDIBBits();
									for(int k = 0;k < m_Buffer->GetWidth();k++){
										for(int j = 0;j < theapp_mainparams.m_single_save.m_static_save_line;j++){
											ss = *(m_bufferfileA[0] + (j * m_Buffer->GetWidth() + k)  * m_Buffer->GetBytesPerPixel());
											ss = ss * 256 + *(m_bufferfileA[0] + (j * m_Buffer->GetWidth() + k)  * m_Buffer->GetBytesPerPixel() + 1);
											if(theapp_mainparams.m_display_para.m_displaybit >= 7)
												tmp[j * m_Buffer->GetWidth() + k] = ss >> (theapp_mainparams.m_display_para.m_displaybit - 7);
											else
												tmp[j * m_Buffer->GetWidth() + k] = ss << (7 - theapp_mainparams.m_display_para.m_displaybit);
										}
									}
									m_image.Save(strFile);
								}
								else{
	
								}
								m_static_line[0] = -1;
								free(m_bufferfileA[index]);
								m_bufferfileA[0] = NULL;
						
						}
						if(theapp_mainparams.m_single_save.m_static_save_aux){
							strFile.Format(_T("%s辅助数据.raw"),theapp_mainparams.m_single_save.m_static_save_dir);
							CFile file;
							if(!file.Open(strFile,CFile::modeCreate|CFile::modeWrite))
								return;
							file.Write(m_auxfileA[index],AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel() * theapp_mainparams.m_single_save.m_static_save_line);
							file.Close();
							free(m_auxfileA[index]);
							m_auxfileA[index] = NULL;
						}
						//}
					
					theapp_mainparams.m_single_save.m_static_save = false;
					if(theapp_mainparams.m_single_save.m_static_msg)
					     AfxMessageBox("图像保存结束！");
				}
					}
				}	
	}
#endif
#ifdef MULTISPECTRL 
	EnterCriticalSection(&g_Aux_Value.cs);
	skip = m_Buffer->GetWidth();
	for(int i = 0; i < 15 ;i++)
		memcpy(&g_Aux_Value.Aux_Value[i * AUXVALUE_LENGTH],&pData[(skip + m_Buffer->GetWidth() * i)* 2],2 * AUXVALUE_LENGTH);
	LeaveCriticalSection(&g_Aux_Value.cs);
	//}
/*	if(!CheckFrameCnt((WORD*)pData)){
		AfxMessageBox("帧计数错误！");
		return;
	}
	if(!CheckLineCnt((WORD*)pData)){
		AfxMessageBox("行计数错误！");
		return;
	}
	*/
	if(theapp_mainparams.m_joint_display > 0){
		for(int i = 0;i < SPECTRL_COUNT;i++){
			m_BufferA[CAMERA_SPECTRL_CNT + i]->GetAddress((void**)&pData1);
			for(int j = 0;j < m_BufferA[CAMERA_SPECTRL_CNT + i]->GetHeight();j++){
				memcpy(&pData1[j * IMAGE_WIDHT * 6],&pData[(m_Buffer->GetWidth() * j + i * IMAGE_WIDHT  + AUXVALUE_LENGTH) * 2 + skip],IMAGE_WIDHT * 2); 
				memcpy(&pData1[j * IMAGE_WIDHT * 6 + IMAGE_WIDHT * 2],&pData[(m_Buffer->GetWidth() * j + i * IMAGE_WIDHT  + SPECTRL_COUNT * IMAGE_WIDHT + AUXVALUE_LENGTH) * 2 + skip],IMAGE_WIDHT * 2); 
				memcpy(&pData1[j * IMAGE_WIDHT * 6 + IMAGE_WIDHT * 4],&pData[(m_Buffer->GetWidth() * j + i * IMAGE_WIDHT  + 2 * SPECTRL_COUNT * IMAGE_WIDHT + AUXVALUE_LENGTH) * 2 + skip],IMAGE_WIDHT * 2); 
			}
			SetEvent(m_ImageWnd_A[CAMERA_SPECTRL_CNT + i]->m_viewportWnd.m_hProSemaphore);
		}
	}
	else{
		for(int i = 0;i < CAMERA_SPECTRL_CNT;i++){
			m_BufferA[i]->GetAddress((void**)&pData1);
			for(int j = 0;j < m_BufferA[i]->GetHeight();j++){
				memcpy(&pData1[j * IMAGE_WIDHT * 2],&pData[(m_Buffer->GetWidth() * j + i * IMAGE_WIDHT  + AUXVALUE_LENGTH) * 2 + skip],IMAGE_WIDHT * 2); 
			}
			if(theapp_mainparams.m_joint_display == 1){
				m_BufferA_joint[i]->GetAddress((void**)&pData1);
				for(int j = 0;j < m_BufferA_joint[i]->GetHeight();j++){
					memcpy(&pData1[j * theapp_mainparams.m_joint_pixelnumber * 2],&pData[(m_Buffer->GetWidth() * j + i * IMAGE_WIDHT  + AUXVALUE_LENGTH) * 2 + skip],theapp_mainparams.m_joint_pixelnumber * 2); 
				}
			}
			SetEvent(m_ImageWnd_A[i]->m_viewportWnd.m_hProSemaphore);
		}
		if(theapp_mainparams.m_displayccdnumber != 255){
			theapp_mainparams.m_window_manager.mirrodlg->SendMessage(WM_CLONE_WIN,NULL,NULL);
		}
	}
	
	if(theapp_mainparams.m_long_save.m_autosave){
		if(theapp_mainparams.m_long_save.m_save_type == SAVE_SPECTRL_TYPE){
			for(int i = 0;i< CAMERA_SPECTRL_CNT;i++){
				if(m_savefileA[i] == NULL){
					if(theapp_mainparams.m_long_save.m_aux_analysis)
						strFile.Format(_T("%s\\CCD%d-谱段%d-时刻-%2d-%2d-%2d-增益%2x-曝光时间%x.raw"),theapp_mainparams.m_long_save.m_save_dir,i / 4,i % 4,tm.GetHour(), tm.GetMinute(), tm.GetSecond(),g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 12 + index]/256,g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 20 + index]/256);
					else
						strFile.Format(_T("%s\\CCD%d-谱段%d-时刻-%2d-%2d-%2d.raw"),theapp_mainparams.m_long_save.m_save_dir,i / 4,i % 4,tm.GetHour(), tm.GetMinute(), tm.GetSecond());
					m_savefileA[i] = CreateFile(strFile,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
				}
				else{
					buffersize = m_BufferA[0]->GetWidth() * m_BufferA[0]->GetHeight() * m_BufferA[0]->GetBytesPerPixel();
					m_BufferA[i]->GetAddress((void**)&pData1);
					WriteFile(m_savefileA[i],pData1,buffersize,&size,NULL);
					FlushFileBuffers(m_savefileA[i]); 
				}
			}
		}
		else if(theapp_mainparams.m_long_save.m_save_type == SAVE_CCD_TYPE){
			for(int i = 0;i< CAMERA_CCD_COUNT;i++){
				if(m_savefileA[i] == NULL){
					if(theapp_mainparams.m_long_save.m_aux_analysis)
						strFile.Format(_T("%s\\CCD%d-时刻-%2d-%2d-%2d-增益%2x-曝光时间%2x.raw"),theapp_mainparams.m_long_save.m_save_dir,i,tm.GetHour(), tm.GetMinute(), tm.GetSecond(),g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 12 + index]/256,g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 20 + index]/256);
					else
						strFile.Format(_T("%s\\CCD%d-时刻-%2d-%2d-%2d.raw"),theapp_mainparams.m_long_save.m_save_dir,i,tm.GetHour(), tm.GetMinute(), tm.GetSecond());
					m_savefileA[i] = CreateFile(strFile,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
				}
				else{
					buffersize = m_BufferA[0]->GetWidth() * m_BufferA[0]->GetHeight() * m_BufferA[0]->GetBytesPerPixel() * SPECTRL_COUNT;
					for(int j = 0 ; j < SPECTRL_COUNT;j++){
						m_BufferA[i * SPECTRL_COUNT + j]->GetAddress((void**)&pData1);
						WriteFile(m_savefileA[i],pData1,buffersize,&size,NULL);
					}
					FlushFileBuffers(m_savefileA[i]); 
				}
			}
		}
		else{
				if(m_savefileA[0] == NULL){
					if(theapp_mainparams.m_long_save.m_aux_analysis)
						strFile.Format(_T("%s\\相机-时刻-%2d-%2d-%2d-增益%2x-曝光时间%x.raw"),theapp_mainparams.m_long_save.m_save_dir,tm.GetHour(), tm.GetMinute(), tm.GetSecond(),g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 12 + index]/256,g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 20 + index]/256);
					else
						strFile.Format(_T("%s\\相机-时刻-%2d-%2d-%2d.raw"),theapp_mainparams.m_long_save.m_save_dir,tm.GetHour(), tm.GetMinute(), tm.GetSecond());
					m_savefileA[0] = CreateFile(strFile,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
				}
				else{
					buffersize = m_Buffer->GetWidth() * m_Buffer->GetHeight() * m_Buffer->GetBytesPerPixel();
					WriteFile(m_savefileA[0],pData,buffersize,&size,NULL);
					FlushFileBuffers(m_savefileA[0]); 
				}
			}
		if(theapp_mainparams.m_long_save.m_save_aux){
			if(m_auxsavefileA[0] == NULL){
					if(theapp_mainparams.m_long_save.m_aux_analysis)
						strFile.Format(_T("%s\\相机辅助数据-时刻-%2d-%2d-%2d-增益%2x-曝光时间%x.raw"),theapp_mainparams.m_long_save.m_save_dir,tm.GetHour(), tm.GetMinute(), tm.GetSecond(),g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 12 + index]/256,g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 20 + index]/256);
					else
						strFile.Format(_T("%s\\相机辅助数据-时刻-%2d-%2d-%2d.raw"),theapp_mainparams.m_long_save.m_save_dir,tm.GetHour(), tm.GetMinute(), tm.GetSecond());
					m_auxsavefileA[0] = CreateFile(strFile,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
				}
				else{
					buffersize =  AUXVALUE_LENGTH * m_Buffer->GetHeight() * m_Buffer->GetBytesPerPixel();
					m_Buffer->ReadRect(0,0,AUXVALUE_LENGTH,m_Buffer->GetHeight(),pData1);
					WriteFile(m_auxsavefileA[0],pData1,buffersize,&size,NULL);
					FlushFileBuffers(m_auxsavefileA[0]); 
				}
		}
	}
	else{
		for(int index = 0;index < CAMERA_SPECTRL_CNT;index++){
			if(m_savefileA[index] != NULL){
				CloseHandle(m_savefileA[index]);
				m_savefileA[index] = NULL;
			}
			if(m_auxsavefileA[index] != NULL){
				CloseHandle(m_auxsavefileA[index]);
				m_auxsavefileA[index] = NULL;
			}
		}
	}

	if(theapp_mainparams.m_single_save.m_static_save){
		long t = GetTickCount();
		CTime time = CTime::GetCurrentTime();
		if(theapp_mainparams.m_single_save.m_save_appointment > time){
			    index = 0;
				if(m_static_line[index] == -1){
					if(theapp_mainparams.m_single_save.m_save_type == SAVE_SPECTRL_TYPE){
						for(int i = 0;i < CAMERA_SPECTRL_CNT;i++){
							m_bufferfileA[i] = (BYTE*)malloc(theapp_mainparams.m_single_save.m_static_save_line * m_BufferA[i]->GetWidth() * m_Buffer->GetBytesPerPixel());
							m_static_line[i] = 0;
						}
					}
					else if(theapp_mainparams.m_single_save.m_save_type == SAVE_CCD_TYPE){
						for(int i = 0;i < CAMERA_CCD_COUNT;i++){
							m_bufferfileA[i] = (BYTE*)malloc(theapp_mainparams.m_single_save.m_static_save_line * m_BufferA[i]->GetWidth() * m_Buffer->GetBytesPerPixel() * SPECTRL_COUNT);
							m_static_line[i] = 0;
						}
					}
					else{
						m_bufferfileA[index] = (BYTE*)malloc(theapp_mainparams.m_single_save.m_static_save_line * m_Buffer->GetWidth() * m_Buffer->GetBytesPerPixel());
						if(theapp_mainparams.m_single_save.m_static_save_aux){
							m_auxfileA[index] = (BYTE*)malloc(theapp_mainparams.m_single_save.m_static_save_line * AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel());
						}
						m_static_line[index] = 0;
					}
					if(theapp_mainparams.m_single_save.m_static_save_aux){
						m_auxfileA[0] = (BYTE*)malloc(theapp_mainparams.m_single_save.m_static_save_line * AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel());
					}
				}
				else{
					if(m_static_line[index]  < theapp_mainparams.m_single_save.m_static_save_line){
						if(theapp_mainparams.m_single_save.m_save_type == SAVE_SPECTRL_TYPE){
							buffersize = m_BufferA[0]->GetWidth() * m_BufferA[0]->GetHeight() * m_BufferA[0]->GetBytesPerPixel();
							for(int i = 0;i < CAMERA_SPECTRL_CNT;i++){
								tmp = m_bufferfileA[i] + m_static_line[i] * m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel();
								if(theapp_mainparams.m_single_save.m_static_save_line <= (m_static_line[i] + m_Buffer->GetHeight())){
									memcpy(tmp,pData,m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel() * (theapp_mainparams.m_single_save.m_static_save_line - m_static_line[index]));
								}
								else{
									memcpy(tmp,pData,buffersize);
								}
								m_static_line[i] += m_Buffer->GetHeight();
							}
							
						}
						else if(theapp_mainparams.m_single_save.m_save_type == SAVE_CCD_TYPE){
							buffersize = m_BufferA[0]->GetWidth() * m_BufferA[0]->GetHeight() * m_BufferA[0]->GetBytesPerPixel() * SPECTRL_COUNT;
							for(int i = 0;i < CAMERA_CCD_COUNT;i++){
								tmp = m_bufferfileA[i] + m_static_line[i] * m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel();
								if(theapp_mainparams.m_single_save.m_static_save_line <= (m_static_line[i] + m_Buffer->GetHeight())){
									memcpy(tmp,pData,m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel() * (theapp_mainparams.m_single_save.m_static_save_line - m_static_line[index]));
								}
								else{
									memcpy(tmp,pData,buffersize);
								}
								m_static_line[i] += m_Buffer->GetHeight();
							}
						}
						else{
							buffersize = m_Buffer->GetWidth() * m_Buffer->GetHeight() * m_Buffer->GetBytesPerPixel() * SPECTRL_COUNT;
							tmp = m_bufferfileA[0] + m_static_line[0] * m_Buffer->GetWidth() * m_Buffer->GetBytesPerPixel();
							if(theapp_mainparams.m_single_save.m_static_save_line <= (m_static_line[index] + m_Buffer->GetHeight())){
								memcpy(tmp,pData,m_Buffer->GetWidth() * m_Buffer->GetBytesPerPixel() * (theapp_mainparams.m_single_save.m_static_save_line - m_static_line[index]));
							}
							else{
								memcpy(tmp,pData,buffersize);
							}
							m_static_line[index] += m_Buffer->GetHeight();
						}
						if(theapp_mainparams.m_single_save.m_static_save_aux){
							m_Buffer->ReadRect(0,0,AUXVALUE_LENGTH,m_Buffer->GetHeight(),pData1);
							tmp = m_auxfileA[index] + m_static_line[index] * AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel();
							memcpy(tmp,pData1,AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel() * (theapp_mainparams.m_single_save.m_static_save_line - m_static_line[index]));
						}	
					}
					else{
						
						if(m_static_line[index] < theapp_mainparams.m_single_save.m_static_save_line)
							return;
						
						CFile file;
						int i = index;
						//for(int i = 0;i < CAMERA_SPECTRL_CNT;i++){
						CString savefile = theapp_mainparams.m_single_save.m_static_save_dir;
						if(theapp_mainparams.m_single_save.m_aux_analysis){
							savefile.Format("%s增益%2x-曝光时间%2x",savefile,g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 12 + index]/256,g_Aux_Value.Aux_Value[AUXVALUE_LENGTH * 5 + 20 + index]/256);
						}
						if(theapp_mainparams.m_long_save.m_save_type == SAVE_SPECTRL_TYPE){
							for(int i = 0;i < CAMERA_SPECTRL_CNT;i++){
								if(theapp_mainparams.m_single_save.m_loop_time != 0xfff)
									strFile.Format(_T("%sCCD%d-谱段%dloop%d%s"),savefile,i / 4,i % 4,theapp_mainparams.m_single_save.m_loop_time,_storefmt[theapp_mainparams.m_single_save.m_static_fmt]);
								else
									strFile.Format(_T("%sCCD%d-谱段%d-%s"),savefile,i / 4,i % 4,_storefmt[theapp_mainparams.m_single_save.m_static_fmt]);
								if(theapp_mainparams.m_single_save.m_static_fmt == 0){
									if(!file.Open(strFile,CFile::modeCreate|CFile::modeWrite))
										return;
									file.Write(m_bufferfileA[i],m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel() * theapp_mainparams.m_single_save.m_static_save_line);
									file.Close();
									TRACE("%s\n",strFile);
								}
								else if(theapp_mainparams.m_single_save.m_static_fmt == 2){
									CDibImage m_image;
									WORD      ss;
									m_image.Create(CSize(m_BufferA[i]->GetWidth(),theapp_mainparams.m_single_save.m_static_save_line),8);
									tmp = (BYTE*)m_image.GetDIBBits();
									for(int k = 0;k < m_BufferA[i]->GetWidth();k++){
										for(int j = 0;j < theapp_mainparams.m_single_save.m_static_save_line;j++){
											ss = *(m_bufferfileA[i] + (j * m_BufferA[i]->GetWidth() + k)  * m_BufferA[i]->GetBytesPerPixel());
											ss = ss * 256 + *(m_bufferfileA[i] + (j * m_BufferA[i]->GetWidth() + k)  * m_BufferA[i]->GetBytesPerPixel() + 1);
											if(theapp_mainparams.m_display_para.m_displaybit >= 7)
												tmp[j * m_BufferA[i]->GetWidth() + k] = ss >> (theapp_mainparams.m_display_para.m_displaybit - 7);
											else
												tmp[j * m_BufferA[i]->GetWidth() + k] = ss << (7 - theapp_mainparams.m_display_para.m_displaybit);
										}
									}
									m_image.Save(strFile);
								}
								else{
	
								}
								m_static_line[i] = -1;
								free(m_bufferfileA[i]);
								m_bufferfileA[i] = NULL;
							}
						}
						else if(theapp_mainparams.m_long_save.m_save_type == SAVE_CCD_TYPE){
							for(int i = 0;i < CAMERA_CCD_COUNT;i++){
								if(theapp_mainparams.m_single_save.m_loop_time != 0xfff)
									strFile.Format(_T("%sCCD%dloop%d%s"),savefile,i / 4,theapp_mainparams.m_single_save.m_loop_time,_storefmt[theapp_mainparams.m_single_save.m_static_fmt]);
								else
									strFile.Format(_T("%sCCD%d-%s"),savefile,i / 4,_storefmt[theapp_mainparams.m_single_save.m_static_fmt]);
								if(theapp_mainparams.m_single_save.m_static_fmt == 0){
									if(!file.Open(strFile,CFile::modeCreate|CFile::modeWrite))
										return;
									file.Write(m_bufferfileA[i],m_BufferA[i]->GetWidth() * m_BufferA[i]->GetBytesPerPixel() * theapp_mainparams.m_single_save.m_static_save_line * SPECTRL_COUNT);
									file.Close();
									TRACE("%s\n",strFile);
								}
								else if(theapp_mainparams.m_single_save.m_static_fmt == 2){
									CDibImage m_image;
									WORD      ss;
									m_image.Create(CSize(m_BufferA[i]->GetWidth(),theapp_mainparams.m_single_save.m_static_save_line),8);
									tmp = (BYTE*)m_image.GetDIBBits();
									for(int k = 0;k < m_BufferA[i]->GetWidth();k++){
										for(int j = 0;j < theapp_mainparams.m_single_save.m_static_save_line;j++){
											ss = *(m_bufferfileA[i] + (j * m_BufferA[i]->GetWidth() + k)  * m_BufferA[i]->GetBytesPerPixel());
											ss = ss * 256 + *(m_bufferfileA[i] + (j * m_BufferA[i]->GetWidth() + k)  * m_BufferA[i]->GetBytesPerPixel() + 1);
											if(theapp_mainparams.m_display_para.m_displaybit >= 7)
												tmp[j * m_BufferA[i]->GetWidth() + k] = ss >> (theapp_mainparams.m_display_para.m_displaybit - 7);
											else
												tmp[j * m_BufferA[i]->GetWidth() + k] = ss << (7 - theapp_mainparams.m_display_para.m_displaybit);
										}
									}
									m_image.Save(strFile);
								}
								else{
	
								}
								m_static_line[i] = -1;
								free(m_bufferfileA[i]);
								m_bufferfileA[i] = NULL;
							}
						}
						else{
								if(theapp_mainparams.m_single_save.m_loop_time != 0xfff)
									strFile.Format(_T("%相机loop%d%s"),savefile,theapp_mainparams.m_single_save.m_loop_time,_storefmt[theapp_mainparams.m_single_save.m_static_fmt]);
								else
									strFile.Format(_T("%s相机-%s"),savefile,_storefmt[theapp_mainparams.m_single_save.m_static_fmt]);
								if(theapp_mainparams.m_single_save.m_static_fmt == 0){
									if(!file.Open(strFile,CFile::modeCreate|CFile::modeWrite))
										return;
									file.Write(m_bufferfileA[index],m_Buffer->GetWidth() * m_Buffer->GetBytesPerPixel() * theapp_mainparams.m_single_save.m_static_save_line);
									file.Close();
									TRACE("%s\n",strFile);
								}
								else if(theapp_mainparams.m_single_save.m_static_fmt == 2){
									CDibImage m_image;
									WORD      ss;
									m_image.Create(CSize(m_Buffer->GetWidth(),theapp_mainparams.m_single_save.m_static_save_line),8);
									tmp = (BYTE*)m_image.GetDIBBits();
									for(int k = 0;k < m_Buffer->GetWidth();k++){
										for(int j = 0;j < theapp_mainparams.m_single_save.m_static_save_line;j++){
											ss = *(m_bufferfileA[index] + (j * m_Buffer->GetWidth() + k)  * m_Buffer->GetBytesPerPixel());
											ss = ss * 256 + *(m_bufferfileA[index] + (j * m_Buffer->GetWidth() + k)  * m_Buffer->GetBytesPerPixel() + 1);
											if(theapp_mainparams.m_display_para.m_displaybit >= 7)
												tmp[j * m_Buffer->GetWidth() + k] = ss >> (theapp_mainparams.m_display_para.m_displaybit - 7);
											else
												tmp[j * m_Buffer->GetWidth() + k] = ss << (7 - theapp_mainparams.m_display_para.m_displaybit);
										}
									}
									m_image.Save(strFile);
								}
								else{
	
								}
								m_static_line[index] = -1;
								free(m_bufferfileA[index]);
								m_bufferfileA[index] = NULL;
							}
						}
						if(theapp_mainparams.m_single_save.m_static_save_aux){
							strFile.Format(_T("%s辅助数据.raw"),theapp_mainparams.m_single_save.m_static_save_dir);
							CFile file;
							if(!file.Open(strFile,CFile::modeCreate|CFile::modeWrite))
								return;
							file.Write(m_auxfileA[index],AUXVALUE_LENGTH * m_Buffer->GetBytesPerPixel() * theapp_mainparams.m_single_save.m_static_save_line);
							file.Close();
							free(m_auxfileA[index]);
							m_auxfileA[index] = NULL;
						}
						//}
					}
					theapp_mainparams.m_single_save.m_static_save = false;
					if(theapp_mainparams.m_single_save.m_static_msg)
					     AfxMessageBox("图像保存结束！");
				}
	}
/*	if(theapp_mainparams.mtfdlg != NULL){
		CDlgMtfCal *tmp = (CDlgMtfCal *)theapp_mainparams.mtfdlg;
		if(tmp->m_already)
			ReleaseSemaphore(tmp->m_hProSemaphore, 1, NULL);
	}
*/
#endif	
	return;
}
	


bool CmyGrab::IsGrabbing(){
	if(m_Xfer->IsGrabbing())
	     return true;
	else
		return false;
}

void CmyGrab::testCrad(void)
{
	UINT32 threadId;
	HANDLE m_hProThread1 = (HANDLE) _beginthreadex(NULL, 0, XferCallback_test, this,CREATE_SUSPENDED , &threadId);
	SetThreadAffinityMask(m_hProThread1,MAIN_GRAB_BASE_THREAD1<<20);
	ResumeThread(m_hProThread1);
	testrunning = true;
}
void CmyGrab::StoptestCard(){
	testrunning = false;
}
unsigned  CmyGrab::XferCallback_test(void *params){
	CmyGrab *pDlg= (CmyGrab *) params;
	CString m_playstart = "F:\\test_img\\test_raw_z3mult.raw";//f:\\MFCApplication6 - z3more\\data\\-时刻- 0-18-44-增益15-曝光时间40.raw";
	CString m_Options;
	CFileOperation m_findfile;
	BYTE* pData = (BYTE*)malloc(pDlg->m_Buffer->GetWidth()*pDlg->m_Buffer->GetHeight());
//    m_findfile.PlayStartFile(m_playstart);
	CFile file;
	 if(file.Open(m_playstart,CFile::modeRead) == NULL){
		 DWORD error = GetLastError();
		 return 0;
	 }
	while(1){
		Sleep(500);
//		m_Options.Format(_T("-format raw -width %d -height %d -offset %d"), pDlg->m_bufferWidth, pDlg->m_bufferHeight, 0);
//	m_Buffer->SetPixelDepth(CORDATA_FORMAT_MONO16);
//	    pDlg->m_Buffer->Load(CStringA(m_findfile.GetFileCurrentName()),-1,0,0,m_Options);
		uint size = file.Read(pData,pDlg->m_Buffer->GetWidth()*pDlg->m_Buffer->GetHeight());
		if(size < pDlg->m_Buffer->GetWidth()*pDlg->m_Buffer->GetHeight()){
			file.SeekToBegin();
			continue;
		}
		pDlg->m_Buffer->WriteRect(0,0,pDlg->m_Buffer->GetWidth(),pDlg->m_Buffer->GetHeight(),pData);
		pDlg->SplitBuffer();
	//	file.Read(pData,
        ReleaseSemaphore(pDlg->m_hProSemaphore, 1, NULL);
	   //}
	}
	return 0;
}

bool CmyGrab::SaveBMP()
{
	return false;
}

bool CmyGrab::CheckFrameCnt(WORD* data)
{
	WORD tmp;
	BYTE framecnt = 0;
	if(m_framecnt == 0xff){
		tmp = data[4];
		m_framecnt = tmp;
	}
	else{
		tmp = data[4];
		framecnt = tmp;
		if(framecnt  != m_framecnt)
			return false;
	}
	return true;
}

bool CmyGrab::CheckLineCnt(WORD* data)
{
	WORD tmp;
	UINT linecnt = 0;
	if(m_linecnt == 0xffffffff){
		tmp = data[1] >> 6;
		m_linecnt = tmp;
		tmp = data[2] >> 6;
		m_linecnt = m_linecnt << 8;
		m_linecnt += tmp;
		tmp = data[3] >> 6;
		m_linecnt = m_linecnt << 8;
		m_linecnt += tmp;
	}
	else{
		tmp = data[1] >> 6;
		linecnt = tmp;
		tmp = data[2] >> 6;
		linecnt = linecnt << 8;
		linecnt += tmp;
		tmp = data[3] >> 6;
		linecnt = linecnt << 8;
		linecnt += tmp;
		if((linecnt - 64) != m_linecnt)
			return false;
	}
}
