
// StressClientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "StressClient.h"
#include "StressClientDlg.h"
#include "afxdialogex.h"

#include "Client.h"
#include "TimeObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CStressClientDlg 대화 상자



CStressClientDlg::CStressClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStressClientDlg::IDD, pParent)
	, stringServerIp_(_T(""))
	, stringServerPort_(_T(""))
	, stringClientCountMax_(_T(""))
	, stringSendCountPerSecond_(_T(""))
	, stringSendPacketSizeMax_(_T(""))
	, stringCloseProbPerFrame_(_T(""))
	, stringClientConnectedCount_(_T(""))
	, stringSendCountTotal_(_T(""))
	, stringRecvMyPacketCount_(_T(""))
	, stringRecvOtherPacketCount_(_T(""))
	, stringRecvMyPacketResponseTimeAvg_(_T(""))
	, stringRecvOtherPacketResponseTimeAvg_(_T(""))
	, stringRecvDiffPacketCount_(_T(""))
	, stringRunningTime_(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStressClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, editServerIp_);
	DDX_Text(pDX, IDC_EDIT1, stringServerIp_);
	DDX_Control(pDX, IDC_EDIT2, editServerPort_);
	DDX_Text(pDX, IDC_EDIT2, stringServerPort_);
	DDX_Control(pDX, IDC_EDIT3, editClientCountMax_);
	DDX_Text(pDX, IDC_EDIT3, stringClientCountMax_);
	DDX_Control(pDX, IDC_EDIT4, editSendCountPerSecond_);
	DDX_Text(pDX, IDC_EDIT4, stringSendCountPerSecond_);
	DDX_Control(pDX, IDC_EDIT5, editSendPacketSizeMax_);
	DDX_Text(pDX, IDC_EDIT5, stringSendPacketSizeMax_);
	DDX_Control(pDX, IDC_EDIT6, editCloseProbPerFrame_);
	DDX_Text(pDX, IDC_EDIT6, stringCloseProbPerFrame_);
	DDX_Control(pDX, IDC_RADIO1, buttonBroadcastEnable_);
	DDX_Control(pDX, IDC_RADIO2, buttonBroadcastDisable_);
	DDX_Text(pDX, IDC_STATIC1, stringClientConnectedCount_);
	DDX_Text(pDX, IDC_STATIC2, stringSendCountTotal_);
	DDX_Text(pDX, IDC_STATIC3, stringRecvMyPacketCount_);
	DDX_Text(pDX, IDC_STATIC4, stringRecvOtherPacketCount_);
	DDX_Text(pDX, IDC_STATIC5, stringRecvMyPacketResponseTimeAvg_);
	DDX_Text(pDX, IDC_STATIC6, stringRecvOtherPacketResponseTimeAvg_);
	DDX_Text(pDX, IDC_STATIC7, stringRecvDiffPacketCount_);
	DDX_Text(pDX, IDC_STATIC8, stringRunningTime_);
	DDX_Control(pDX, IDC_BUTTON1, buttonStart_);
	DDX_Control(pDX, IDC_BUTTON2, buttonStop_);
}

BEGIN_MESSAGE_MAP(CStressClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO1, &CStressClientDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CStressClientDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_BUTTON1, &CStressClientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CStressClientDlg::OnBnClickedButton2)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CStressClientDlg 메시지 처리기

BOOL CStressClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	//AllocConsole();

	UpdateData(TRUE);
	stringServerIp_ = theApp.GetConfig().serverIp_.c_str();
	stringServerPort_.Format(_T("%d"), theApp.GetConfig().serverPort_);
	stringClientCountMax_.Format(_T("%d"), theApp.GetConfig().clientCountMax_);
	stringSendCountPerSecond_.Format(_T("%d"), theApp.GetConfig().sendCountPerSecond_);
	stringSendPacketSizeMax_.Format(_T("%d"), theApp.GetConfig().sendPacketSizeMax_);
	stringCloseProbPerFrame_.Format(_T("%d"), theApp.GetConfig().closeProbPerFrame_);

	stringClientConnectedCount_ = "";
	stringSendCountTotal_ = "";
	stringRecvMyPacketCount_ = "";
	stringRecvOtherPacketCount_ = "";
	stringRecvMyPacketResponseTimeAvg_ = "";
	stringRecvOtherPacketResponseTimeAvg_ = "";
	stringRecvDiffPacketCount_ = "";

	buttonBroadcastEnable_.SetCheck(0);
	buttonBroadcastDisable_.SetCheck(1);
	buttonStart_.EnableWindow(TRUE);
	buttonStop_.EnableWindow(FALSE);
	UpdateData(FALSE);

	SetTimer(1, 100, 0);
	SetTimer(2, 1000, 0);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CStressClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CStressClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CStressClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CStressClientDlg::OnBnClickedRadio1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.GetConfig().broadcast_ = true;
}


void CStressClientDlg::OnBnClickedRadio2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.GetConfig().broadcast_ = false;
}


void CStressClientDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);

	wstring_convert<codecvt_utf8<wchar_t>> conv;
	string serverIp = conv.to_bytes(stringServerIp_.GetBuffer());
	int serverPort = _ttoi(stringServerPort_);
	int clientCountMax = _ttoi(stringClientCountMax_);
	int sendCountPerSecond = _ttoi(stringSendCountPerSecond_);
	int sendPacketSizeMax = _ttoi(stringSendPacketSizeMax_);
	int closeProbPerFrame = _ttoi(stringCloseProbPerFrame_);

	if (!IsValidIp(serverIp))
	{
		MessageBox(_T("서버 IP 오류"));
		return;
	}

	stringServerIp_ = serverIp.c_str();

	if (serverPort > 65536)
	{
		MessageBox(_T("서버 Port 오류"));
		return;
	}

	if (clientCountMax > 10000)
	{
		clientCountMax = 10000;
		stringClientCountMax_ = "10000";
	}

	if (sendCountPerSecond > 20)
	{
		sendCountPerSecond = 20;
		stringSendCountPerSecond_ = "20";
	}

	if (sendPacketSizeMax > 4096)
	{
		sendPacketSizeMax = 4096;
		stringSendPacketSizeMax_ = "4096";
	}

	if (closeProbPerFrame > 10000)
	{
		closeProbPerFrame = 10000;
		stringCloseProbPerFrame_ = "10000";
	}

	stringClientConnectedCount_ = "0";
	stringSendCountTotal_ = "0";
	stringRecvMyPacketCount_ = "0";
	stringRecvOtherPacketCount_ = "0";
	stringRecvMyPacketResponseTimeAvg_ = "0ms";
	stringRecvOtherPacketResponseTimeAvg_ = "0ms";
	stringRecvDiffPacketCount_ = "0";
	stringRunningTime_ = "진행 시간: ";

	editServerIp_.EnableWindow(FALSE);
	editServerPort_.EnableWindow(FALSE);
	editClientCountMax_.EnableWindow(FALSE);
	editSendCountPerSecond_.EnableWindow(FALSE);
	editSendPacketSizeMax_.EnableWindow(FALSE);
	editCloseProbPerFrame_.EnableWindow(FALSE);

	buttonStart_.EnableWindow(FALSE);
	buttonStop_.EnableWindow(TRUE);

	UpdateData(FALSE);

	runningTime_ = 58;

	theApp.GetConfig().serverIp_ = serverIp;
	theApp.GetConfig().serverPort_ = serverPort;
	theApp.GetConfig().clientCountMax_ = clientCountMax;
	theApp.GetConfig().sendCountPerSecond_ = sendCountPerSecond;
	theApp.GetConfig().sendPacketSizeMax_ = sendPacketSizeMax;
	theApp.GetConfig().closeProbPerFrame_ = closeProbPerFrame;

	for (int i = 0; i < clientCountMax; i++)
	{
		Client* client = new Client(theApp.GetClients().size());
		if (client->Run())
		{
			theApp.GetClients().push_back(client);
		}
	}

	theApp.GetStatus().Init();
	theApp.SetRun(true);
}


void CStressClientDlg::OnBnClickedButton2()
{
	theApp.SetRun(false);

	for (auto& client : theApp.GetClients())
	{
		if (client)
		{
			client->Stop();
		}
	}

	for (auto& client : theApp.GetClients())
	{
		if (client)
		{
			client->Join();
			delete client;
		}
	}

	theApp.GetClients().clear();
	theApp.GetStatus().Init();

	editServerIp_.EnableWindow(TRUE);
	editServerPort_.EnableWindow(TRUE);
	editClientCountMax_.EnableWindow(TRUE);
	editSendCountPerSecond_.EnableWindow(TRUE);
	editSendPacketSizeMax_.EnableWindow(TRUE);
	editCloseProbPerFrame_.EnableWindow(TRUE);

	buttonStart_.EnableWindow(TRUE);
	buttonStop_.EnableWindow(FALSE);
}

void CStressClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (theApp.IsRun())
	{
		if (nIDEvent == 1)
		{
			theApp.GetStatus().Init();

			for (auto& client : theApp.GetClients())
			{
				if (client && client->IsConnected())
				{
					theApp.GetStatus().clientConnectedCount_++;
					theApp.GetStatus().sendCountTotal_ += client->GetTotalSentCount();
					theApp.GetStatus().recvMyPacketCount_ += client->GetMyPacketRecvCount();
					theApp.GetStatus().recvMyPacketResponseTimeAvg_ += client->GetMyPacketResponseTimeAvg();
					theApp.GetStatus().recvOtherPacketCount_ += client->GetOtherPacketRecvCount();
					theApp.GetStatus().recvOtherPacketResponseTimeAvg_ += client->GetOtherPacketResponseTimeAvg();
					theApp.GetStatus().recvDiffPacketCount_ += client->GetDiffPacketCount();
				}
			}

			if (theApp.GetClients().size() > 0)
			{
				theApp.GetStatus().recvMyPacketResponseTimeAvg_ = (UInt64)(theApp.GetStatus().recvMyPacketResponseTimeAvg_ / (Int64)theApp.GetClients().size());
				theApp.GetStatus().recvOtherPacketResponseTimeAvg_ = (UInt64)(theApp.GetStatus().recvOtherPacketResponseTimeAvg_ / (Int64)theApp.GetClients().size());
			}

			UpdateData(TRUE);
			stringClientConnectedCount_.Format(_T("%d"), theApp.GetStatus().clientConnectedCount_);
			stringSendCountTotal_.Format(_T("%d"), theApp.GetStatus().sendCountTotal_);
			stringRecvMyPacketCount_.Format(_T("%d"), theApp.GetStatus().recvMyPacketCount_);
			stringRecvOtherPacketCount_.Format(_T("%d"), theApp.GetStatus().recvOtherPacketCount_);
			stringRecvMyPacketResponseTimeAvg_.Format(_T("%dms"), theApp.GetStatus().recvMyPacketResponseTimeAvg_);
			stringRecvOtherPacketResponseTimeAvg_.Format(_T("%dms"), theApp.GetStatus().recvOtherPacketResponseTimeAvg_);
			stringRecvDiffPacketCount_.Format(_T("%d"), theApp.GetStatus().recvDiffPacketCount_);
			UpdateData(FALSE);
		}
		else if (nIDEvent == 2)
		{
			int currentTime = ++runningTime_;
			int hour = currentTime / 3600;
			currentTime = currentTime % 3600;
			int minute = currentTime / 60;
			currentTime = currentTime % 60;
			int second = currentTime;

			UpdateData(TRUE);

			if (hour > 0)
			{
				stringRunningTime_.Format(_T("진행 시간: %d시간 %d분 %d초"), hour, minute, second);
			}
			else if (minute > 0)
			{
				stringRunningTime_.Format(_T("진행 시간: %d분 %d초"), minute, second);
			}
			else
			{
				stringRunningTime_.Format(_T("진행 시간: %d초"), second);
			}

			UpdateData(FALSE);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

bool CStressClientDlg::IsValidIp(string& ip)
{
	string addr[4];
	string fixedIP;

	string input = ip;
	int dotCount = 0;

	while (dotCount < 3)
	{
		size_t index = input.find_first_of(".");
		if (index == input.npos)
		{
			return false;
		}

		addr[dotCount] = input.substr(0, index);
		input = input.substr(index + 1, input.size() - index + 1);
		dotCount++;
	}

	addr[dotCount] = input;

	for (int i = 0; i < 4; i++)
	{
		if (addr[i].size() <= 0 || addr[i].size() > 3)
		{
			return false;
		}

		if (!all_of(addr[i].begin(), addr[i].end(), ::isdigit))
		{
			return false;
		}

		int num = atoi(addr[i].c_str());
		if (num < 0 && num > 255)
		{
			return false;
		}

		if (i > 0)
		{
			fixedIP += ".";
		}

		fixedIP += to_string(num);
	}

	ip = fixedIP;
	
	return true;
}


void CStressClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//FreeConsole();
}
