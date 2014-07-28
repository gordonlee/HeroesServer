
// StressClientDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// CStressClientDlg 대화 상자
class CStressClientDlg : public CDialogEx
{
// 생성입니다.
public:
	CStressClientDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_STRESSCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
