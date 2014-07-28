
// StressClientDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"


// CStressClientDlg ��ȭ ����
class CStressClientDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CStressClientDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_STRESSCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit editServerIp_;
	CString stringServerIp_;
	CEdit editServerPort_;
	CString stringServerPort_;
	CEdit editClientCountMax_;
	CString stringClientCountMax_;
	CEdit editSendCountPerSecond_;
	CString stringSendCountPerSecond_;
	CEdit editSendPacketSizeMax_;
	CString stringSendPacketSizeMax_;
	CEdit editCloseProbPerFrame_;
	CString stringCloseProbPerFrame_;
	CButton buttonBroadcastEnable_;
	CButton buttonBroadcastDisable_;
	CString stringClientConnectedCount_;
	CString stringSendCountTotal_;
	CString stringRecvMyPacketCount_;
	CString stringRecvOtherPacketCount_;
	CString stringRecvMyPacketResponseTimeAvg_;
	CString stringRecvOtherPacketResponseTimeAvg_;
	CString stringRecvDiffPacketCount_;
	CString stringRunningTime_;
	CButton buttonStart_;
	CButton buttonStop_;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

private:
	bool IsValidIp(string& ip);
	int runningTime_;
};
