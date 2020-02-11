
// DriverModuleDlg.h : 头文件
//

#pragma once

#include"../../../Driver/interface/CDriverOperator.h"
#include "afxwin.h"

// CDriverModuleDlg 对话框
class CDriverModuleDlg : public CDialogEx
{
// 构造
public:
	CDriverModuleDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DRIVERMODULE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
