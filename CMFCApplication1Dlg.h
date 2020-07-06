
// MFCApplication1Dlg.h: 헤더 파일
//

#pragma once

class CMFCApplication1DlgAutoProxy;


// CMFCApplication1Dlg 대화 상자
class CMFCApplication1Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMFCApplication1Dlg);
	friend class CMFCApplication1DlgAutoProxy;

// 생성입니다.
public:
	CMFCApplication1Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	virtual ~CMFCApplication1Dlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	CMFCApplication1DlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR event);
	afx_msg void OnMouseHover(UINT flag, CPoint cp) ;
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
public:
	CString hostIpName;
	CButton m_Btn1;
	CButton m_Btn2;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CIPAddressCtrl m_IpAddr;
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR* pNMHDR, LRESULT* pResult);

	CWinThread* m_pThread;
	bool m_isWorkingThread;
	static UINT ThreadForCounting(LPVOID param) ;

};
