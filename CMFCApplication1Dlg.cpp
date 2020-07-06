
// MFCApplication1Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "CMFCApplication1.h"
#include "CMFCApplication1Dlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include "MFCFFmpegjpeg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "gdiplus.h"

// CMFCApplication1Dlg 대화 상자
IMPLEMENT_DYNAMIC(CMFCApplication1Dlg, CDialogEx);

CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
	, hostIpName(_T(""))
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = nullptr;
}

CMFCApplication1Dlg::~CMFCApplication1Dlg()
{
	// 이 대화 상자에 대한 자동화 프록시가 있을 경우 이 대화 상자에 대한
	//  후방 포인터를 null로 설정하여
	//  대화 상자가 삭제되었음을 알 수 있게 합니다.
	if (m_pAutoProxy != nullptr)
		m_pAutoProxy->m_pDialog = nullptr;
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, hostIpName);
	DDX_Control(pDX, IDC_BUTTON1, m_Btn1);
	DDX_Control(pDX, IDC_BUTTON2, m_Btn2);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IpAddr);

}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_MOUSEHOVER()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication1Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication1Dlg::OnBnClickedButton2)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, &CMFCApplication1Dlg::OnIpnFieldchangedIpaddress1)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 메시지 처리기

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	SetTimer(1000,1000*20,0) ;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		;
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCApplication1Dlg::OnPaint()
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

		TRACE(traceAppMsg, 0, "OnPaint1 \n");

	
	}
	else
	{
		CDialogEx::OnPaint();
		TRACE(traceAppMsg, 0, "OnPaint2 \n");
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 컨트롤러에서 해당 개체 중 하나를 계속 사용하고 있을 경우
//  사용자가 UI를 닫을 때 자동화 서버를 종료하면 안 됩니다.  이들
//  메시지 처리기는 프록시가 아직 사용 중인 경우 UI는 숨기지만,
//  UI가 표시되지 않아도 대화 상자는
//  남겨 둡니다.


void CMFCApplication1Dlg::OnMouseHover(UINT flag, CPoint cp)
{
	CDialogEx::OnMouseHover(flag, cp) ;
	TRACE(traceAppMsg, 0, "OnMOuseHover\n");

}
void CMFCApplication1Dlg::OnDestroy()
{
	CDialogEx::OnDestroy() ;
	KillTimer(1000) ;
	TRACE(traceAppMsg, 0, "OnDestroy \n");

}

void CMFCApplication1Dlg::OnTimer(UINT_PTR event)
{
	CDialogEx::OnTimer(event) ;
	TRACE(traceAppMsg, 0, "OnTimer \n");

	if (CanExit())
		CDialogEx::OnOK();

}

void CMFCApplication1Dlg::OnClose() // 상단우측종료 
{
	if (CanExit())
		CDialogEx::OnClose();

	TRACE(traceAppMsg, 0, "OnClose \n");

}

void CMFCApplication1Dlg::OnOK()
{
	if (CanExit())
		CDialogEx::OnOK();
}

void CMFCApplication1Dlg::OnCancel()
{
	if (CanExit())
		CDialogEx::OnCancel();
}

BOOL CMFCApplication1Dlg::CanExit()
{
	// 프록시 개체가 계속 남아 있으면 자동화 컨트롤러에서는
	//  이 애플리케이션을 계속 사용합니다.  대화 상자는 남겨 두지만
	//  해당 UI는 숨깁니다.
	if (m_pAutoProxy != nullptr)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

void CMFCApplication1Dlg::OnBnClickedButton1() // start
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_isWorkingThread = true;
	m_pThread = AfxBeginThread(ThreadForCounting, this);
	//SetTimer(1000,1000*20,0) ;
}


void CMFCApplication1Dlg::OnBnClickedButton2() // stop
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	
	m_isWorkingThread = false;
	if (m_pThread)
		WaitForSingleObject(m_pThread->m_hThread, 5000);
	//SetTimer(1000,1000*20,0) ;

	
}


void CMFCApplication1Dlg::OnIpnFieldchangedIpaddress1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


UINT CMFCApplication1Dlg::ThreadForCounting(LPVOID param)
{
	CMFCApplication1Dlg* pMain = (CMFCApplication1Dlg*)param;
 
	while (pMain->m_isWorkingThread)
	{
		static int count = 0 ;

		Sleep(33) ;
		count++ ;
		
		// 화면 전체를 캡쳐하기 위해서 Window DC 를 사용합니다. ::GetDC(NULL) 이라고 해도 결과는 동일합니다.
		HDC h_dc = ::GetWindowDC(NULL);
		// 캡쳐에 사용할 CImage 객체를 선언한다.
		CImage tips_image;
		// 수평 해상도를 얻는다.
		int cx = ::GetSystemMetrics(SM_CXSCREEN);
		// 수직 해상도를 얻는다.
		int cy = ::GetSystemMetrics(SM_CYSCREEN);
		// 화면의 색상 수를 얻는다.
		int color_depth = ::GetDeviceCaps(h_dc, BITSPIXEL);
		// 캡쳐에 사용할 이미지를 생성한다.
		tips_image.Create(cx, cy, color_depth, 0);

		// 화면 전체 이미지를 m_tips_image 객체에 복사한다. 
		::BitBlt(tips_image.GetDC(), 0, 0, cx, cy, h_dc, 0, 0, SRCCOPY);
		// 캡쳐한 이미지를 "test.png" 라는 이름으로 저장한다.
		CString fn1,fn2 ;
		fn1.Format(_T("test.jpg")) ;
		fn2.Format(_T("test.h264")) ;
		tips_image.Save(fn1, Gdiplus::ImageFormatJPEG);

		// 화면 캡쳐에 사용했던 DC를 해제한다.
		::ReleaseDC(NULL, h_dc); 
		tips_image.ReleaseDC();


		#if 1
		MFCFFmpegjpeg h264 ;
		int ret ;
		CString msgbox ;
		
		if (count==120)
			ret = h264.Jpeg2H264((const char*)"test.jpg", (const char*)"test.h264",true) ;
		else if (count < 120){
			ret = h264.Jpeg2H264((const char*)"test.jpg", (const char*)"test.h264",false) ;
			msgbox.Format(_T("count=%d, ret=%d \n"),count,ret) ;
			AfxMessageBox(msgbox);
		}
		#endif
		
	}
 
	return 0;
}




