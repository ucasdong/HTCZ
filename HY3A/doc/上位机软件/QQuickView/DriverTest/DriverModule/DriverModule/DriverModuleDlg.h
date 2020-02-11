
// DriverModuleDlg.h : ͷ�ļ�
//

#pragma once

#include"../../../Driver/interface/CDriverOperator.h"
#include "afxwin.h"

// CDriverModuleDlg �Ի���
class CDriverModuleDlg : public CDialogEx
{
// ����
public:
	CDriverModuleDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DRIVERMODULE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedStartSave();
	afx_msg void OnBnClickedStopSave();
	afx_msg void OnBnClickedBrowse2();
	afx_msg void OnBnClickedSwitchMode();
	afx_msg void OnBnClickedGetstatus();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

public:
	void PrintData();
	void HexDump(char *buf,int len,int addr);

private:
	CDriverOperator *m_driverOperator;
	CComboBox m_cobSaveType;
};
