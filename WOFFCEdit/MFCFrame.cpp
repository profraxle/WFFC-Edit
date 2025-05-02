#include "MFCFrame.h"
#include "resource.h"



BEGIN_MESSAGE_MAP(CMyFrame, CFrameWnd)
	
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TOOL, &CMyFrame::OnUpdatePage)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_TOOL
};
//FRAME CLASS


//frame initialiser
CMyFrame::CMyFrame()
{
	m_selectionID = 999; //an obviously wrong selection ID,  to verify its working
}

void CMyFrame::SetCurrentSelectionID(int ID)
{
	m_selectionID = ID;
}

void CMyFrame::OnUpdatePage(CCmdUI * pCmdUI)
{
	pCmdUI->Enable();
	CString strPage;
	strPage.Format(_T("%d"), m_selectionID);
	pCmdUI->SetText(strPage);
}

//oncretae, called after init but before window is shown. 
int CMyFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;

	// create a view to occupy the client area of the frame //This is where DirectX is rendered
	if (!m_DirXView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	m_DirXView.ShowWindow(SW_HIDE);

	m_menu1.LoadMenuW(IDR_MENU1);
	SetMenu(&m_menu1);

	if (!m_toolBar.CreateEx(this, TBSTYLE_TRANSPARENT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CRect rect;
	GetClientRect(&rect);
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_TOOL, SBPS_NORMAL, rect.Width() - 500);//set width of status bar panel

	if (!m_selectDialogPlaceholder.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_GRAYRECT,
		CRect(100, 0, 250, 1080), this, IDC_SELECT_DIALOG_PLACEHOLDER))
	{
		TRACE0("Failed to create placeholder\n");
		return -1;
	}

	// Optional: Set a border or color to visualize it
	m_selectDialogPlaceholder.SetWindowText(_T("Select Dialog Placeholder")); // (for debugging)

	// Continue creating toolbar/status bar etc as normal...



	return 0;
}

void CMyFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// Shrink DirXView to leave 250px gap on the right for tools
	if (m_DirXView.GetSafeHwnd())
	{
		m_DirXView.MoveWindow(0, 0, cx - 250, cy);
	}

	// Place placeholder into that right panel
	if (m_selectDialogPlaceholder.GetSafeHwnd())
	{
		m_selectDialogPlaceholder.MoveWindow(cx - 240, 10, 230, cy - 20);
	}
}