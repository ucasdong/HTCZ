
// DriverModuleDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DriverModule.h"
#include "DriverModuleDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define STANDARD_FILTER		"RAW data Files (*.raw)|*.raw|BMP Files (*.bmp)|*.bmp||"


BYTE *m_pData = NULL;


void CDriverModuleDlg::HexDump(char *buf,int len,int addr) 
{
	int i,j,k;
	char binstr[80];

	for (i=0;i<len;i++) {
		if (0==(i%16)) {
			sprintf_s(binstr,"%08x -",i+addr);
			sprintf_s(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
		} else if (15==(i%16)) {
			sprintf_s(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
			sprintf_s(binstr,"%s  ",binstr);
			for (j=i-15;j<=i;j++) {
				sprintf_s(binstr,"%s%c",binstr,('!'<buf[j]&&buf[j]<='~')?buf[j]:'.');
			}
			CString strText;
			((CEdit*)GetDlgItem(IDC_EDIT_OUTPUT))->GetWindowText(strText);
			strText += "\r\n";
			strText += binstr;
			((CEdit*)GetDlgItem(IDC_EDIT_OUTPUT))->SetWindowText(strText);
			//printf("%s\n",binstr);
		} else {
			sprintf_s(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
		}
	}
	if (0!=(i%16)) {
		k=16-(i%16);
		for (j=0;j<k;j++) {
			sprintf_s(binstr,"%s   ",binstr);
		}
		sprintf_s(binstr,"%s  ",binstr);
		k=16-k;
		for (j=i-k;j<i;j++) {
			sprintf_s(binstr,"%s%c",binstr,('!'<buf[j]&&buf[j]<='~')?buf[j]:'.');
		}
		CString strText;
		((CEdit*)GetDlgItem(IDC_EDIT_OUTPUT))->GetWindowText(strText);	
		strText += "\n";
		strText += binstr;
		((CEdit*)GetDlgItem(IDC_EDIT_OUTPUT))->SetWindowText(strText);
		//printf("%s\n",binstr);
	}
}



void BufferCallback(CDriverBuffer *pBuffer, void *pThis)
{
	CDriverModuleDlg *pDlg = (CDriverModuleDlg*)pThis;

	int width = pBuffer->GetWidth();
	int height = pBuffer->GetHeight();
	int bytes = pBuffer->GetBytesPerPixel();
	int nIndex = pBuffer->GetIndex();

	BYTE *m_ptmp = NULL;
	pBuffer->GetAddress((void**)&m_ptmp);

	////pDlg->HexDump((char*)m_ptmp, width*height*bytes, (int)m_ptmp);
	///pDlg->PrintData();
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDriverModuleDlg �Ի���



CDriverModuleDlg::CDriverModuleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDriverModuleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDriverModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, COMBO_SAVE_TYPE, m_cobSaveType);
}

BEGIN_MESSAGE_MAP(CDriverModuleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(BTN_BROWSE, &CDriverModuleDlg::OnBnClickedBrowse)
	ON_BN_CLICKED(BTN_START, &CDriverModuleDlg::OnBnClickedStart)
	ON_BN_CLICKED(BTN_STOP, &CDriverModuleDlg::OnBnClickedStop)
	ON_BN_CLICKED(BTN_START_SAVE, &CDriverModuleDlg::OnBnClickedStartSave)
	ON_BN_CLICKED(BTN_STOP_SAVE, &CDriverModuleDlg::OnBnClickedStopSave)
	ON_BN_CLICKED(BTN_BROWSE2, &CDriverModuleDlg::OnBnClickedBrowse2)
	ON_BN_CLICKED(BTN_SWITCH_MODE, &CDriverModuleDlg::OnBnClickedSwitchMode)
	ON_BN_CLICKED(BTN_GETSTATUS, &CDriverModuleDlg::OnBnClickedGetstatus)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDriverModuleDlg ��Ϣ�������

BOOL CDriverModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	((CEdit*)GetDlgItem(IDC_EDIT_WIDTH))->SetWindowText("860");
	((CEdit*)GetDlgItem(IDC_EDIT_HEIGHT))->SetWindowText("3534");

	m_cobSaveType.InsertString(0, "��������");
	m_cobSaveType.InsertString(1, "ԭʼ����");
	m_cobSaveType.SetCurSel(0);

	m_driverOperator = new CDriverOperator;

	if (1 != m_driverOperator->InitOperator("json"))
	{
		AfxMessageBox("��ʼ������!");
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDriverModuleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDriverModuleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDriverModuleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDriverModuleDlg::OnBnClickedBrowse()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog dlg(TRUE, _T(""), NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, _T(STANDARD_FILTER), this);

	if(dlg.DoModal() == IDOK)
	{
		CString strPath = dlg.GetPathName();
		SetDlgItemText(EDIT_PATH, strPath);
	}
}


void CDriverModuleDlg::OnBnClickedStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strPath(_T(""));
	GetDlgItemText(EDIT_PATH, strPath);
	if (strPath == "")
	{
		AfxMessageBox("��������Դ��ַ!");
		return;
	}

	m_driverOperator->RegisterRecvBufferCallback(BufferCallback, this);

	m_driverOperator->StartPlaybackGrab(strPath.GetBuffer());
}


void CDriverModuleDlg::OnBnClickedStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_driverOperator->StopPlaybackGrab();

}


void CDriverModuleDlg::OnBnClickedStartSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������	
	CString strPath(_T(""));
	((CEdit*)GetDlgItem(EDIT_PATH2))->GetWindowText(strPath);
	int nType = m_cobSaveType.GetCurSel();
	strPath = strPath + "\\Test.raw";

	if (0 == nType)
	{
		m_driverOperator->StartSave(SAVE_DATA_TRANSFER, strPath);
	}
	else
	{
		m_driverOperator->StartSave(SAVE_ORIGINAL, strPath);
	}
}


void CDriverModuleDlg::OnBnClickedStopSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nType = m_cobSaveType.GetCurSel();
	if (0 == nType)
	{
		m_driverOperator->StopSave(SAVE_DATA_TRANSFER);
	}
	else
	{
		m_driverOperator->StopSave(SAVE_ORIGINAL);
	}
}


void CDriverModuleDlg::OnBnClickedBrowse2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFolderPickerDialog dlg;
	CString strFolder(_T(""));
	if(dlg.DoModal() == IDOK)
	{
		strFolder = dlg.GetPathName();
	}

	if (strFolder != "")
	{
		((CEdit*)GetDlgItem(EDIT_PATH2))->SetWindowText(strFolder.GetBuffer());
	}
}


void CDriverModuleDlg::PrintData()
{
	static int aaa = 0;
	TRACE("=========================%d\n", aaa++);
}
void CDriverModuleDlg::OnBnClickedSwitchMode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strWidth, strHeight;
	((CEdit*)GetDlgItem(IDC_EDIT_WIDTH))->GetWindowText(strWidth);
	((CEdit*)GetDlgItem(IDC_EDIT_HEIGHT))->GetWindowText(strHeight);

	int nWidth = atoi(strWidth.GetBuffer());
	int nHeight = atoi(strHeight.GetBuffer());

	m_driverOperator->SetMode(nWidth, nHeight);
}


void CDriverModuleDlg::OnBnClickedGetstatus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nStatus = 0;
	m_driverOperator->GetCardStatus(nStatus);

	CString strMsg[8] = {"", "��Ԫ������Ч", "�������Դ����", "������е�Դ����", "����ʱ��3�ź�", "����ʱ��2�ź�", "����ʱ��1�ź�", "���ź�"};


	int nValid[8]; //�����0λ���λ, �����7Ϊ���λ
	nValid[7] = (nStatus & 0x01) == 0x01 ? 1 : 0;	//���λ ����λ
	nValid[6] = (nStatus & 0x02) == 0x02 ? 1 : 0;
	nValid[5] = (nStatus & 0x04) == 0x04 ? 1 : 0;
	nValid[4] = (nStatus & 0x08) == 0x08 ? 1 : 0;
	nValid[3] = (nStatus & 0x10) == 0x10 ? 1 : 0;
	nValid[2] = (nStatus & 0x20) == 0x20 ? 1 : 0;
	nValid[1] = (nStatus & 0x40) == 0x40 ? 1 : 0;
	nValid[0] = (nStatus & 0x80) == 0x80 ? 1 : 0;	//���λ ����λ

	CString strOutput;
	for (int i = 7; i >= 0; --i)
	{
		if (nValid[i] == 1)
		{
			strOutput += strMsg[i];
			strOutput += "\n";
		}
	}
	AfxMessageBox(strOutput);
}


void CDriverModuleDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_driverOperator != NULL)
	{
		delete m_driverOperator;

	} 
	CDialogEx::OnClose();
}
