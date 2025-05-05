#include "MFCMain.h"
#include "resource.h"
#include "Enums.h"

BEGIN_MESSAGE_MAP(MFCMain, CWinApp)
	ON_COMMAND(ID_FILE_QUIT,	&MFCMain::MenuFileQuit)
	ON_COMMAND(ID_FILE_SAVETERRAIN, &MFCMain::MenuFileSaveTerrain)
	ON_COMMAND(ID_EDIT_SELECT, &MFCMain::MenuEditSelect)
	ON_COMMAND(ID_BUTTON40001,	&MFCMain::ToolBarButton1)
	ON_COMMAND(ID_UNDO_BUTTON, &MFCMain::ToolBarButton2)
	ON_COMMAND(ID_REDO_BUTTON, &MFCMain::ToolBarButton3)
	ON_COMMAND(ID_GIZMO_BUTTON, &MFCMain::ToolBarButton4)
	ON_COMMAND(ID_TERRAIN_BUTTON, &MFCMain::ToolBarButton5)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TOOL, &CMyFrame::OnUpdatePage)
END_MESSAGE_MAP()

BOOL MFCMain::InitInstance()
{
	//instanciate the mfc frame
	m_frame = new CMyFrame();
	m_pMainWnd = m_frame;

	m_frame->Create(	NULL,
					_T("World Of Flim-Flam Craft Editor"),
					WS_OVERLAPPEDWINDOW,
					CRect(100, 100, 1320, 795),
					NULL,
					NULL,
					0,
					NULL
				);

	//show and set the window to run and update. 
	m_frame->ShowWindow(SW_SHOW);
	m_frame->UpdateWindow();


	//get the rect from the MFC window so we can get its dimensions
	m_toolHandle = m_frame->m_DirXView.GetSafeHwnd();				//handle of directX child window
	m_frame->m_DirXView.GetClientRect(&WindowRECT);
	m_width = 800;
	m_height = 600;

	m_ToolSystem.onActionInitialise(m_toolHandle, m_width, m_height);
	m_ToolSystem.SetMFCMain(this);

	MenuEditGizmo();
	MenuEditTerrain();

	m_frame->m_toolBar.GetToolBarCtrl().EnableButton(ID_GIZMO_BUTTON, FALSE);
	return TRUE;
}

int MFCMain::Run()
{
	MSG msg;
	BOOL bGotMsg;

	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		if (true)
		{
			bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		}
		else
		{
			bGotMsg = (GetMessage(&msg, NULL, 0U, 0U) != 0);
		}

		if (bGotMsg)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			m_ToolSystem.UpdateInput(&msg);
		}
		else
		{	
			int ID = m_ToolSystem.getCurrentSelectionID();
			std::wstring statusString = L"Selected Object: " + std::to_wstring(ID);
			m_ToolSystem.Tick(&msg);

			//send current object ID to status bar in The main frame
			m_frame->m_wndStatusBar.SetPaneText(1, statusString.c_str(), 1);	
		}
	}

	return (int)msg.wParam;
}

void MFCMain::MenuFileQuit()
{
	//will post message to the message thread that will exit the application normally
	PostQuitMessage(0);
}

void MFCMain::MenuFileSaveTerrain()
{
	m_ToolSystem.onActionSaveTerrain();
}

void MFCMain::MenuEditSelect()
{

	CWnd* pPlaceholder = m_frame->GetDlgItem(IDC_SELECT_DIALOG_PLACEHOLDER);

		if (pPlaceholder) {
			//SelectDialogue m_ToolSelectDialogue(NULL, &m_ToolSystem.m_sceneGraph);		//create our dialoguebox //modal constructor
			//m_ToolSelectDialogue.DoModal();	// start it up modal

			//modeless dialogue must be declared in the class.   If we do local it will go out of scope instantly and destroy itself
			m_ToolSelectDialogue.Create(IDD_DIALOG1,pPlaceholder);	//Start up modeless
			m_ToolSelectDialogue.ShowWindow(SW_SHOW);	//show modeless

			CRect rect;
			pPlaceholder->GetClientRect(&rect);
			m_ToolSelectDialogue.MoveWindow(&rect);

		}
	m_ToolSelectDialogue.SetObjectData(&m_ToolSystem.m_sceneGraph, &m_ToolSystem.m_selectedObject);

}

void MFCMain::MenuEditUndoRedo()
{
}

void MFCMain::ToolBarButton1()
{
	m_ToolSystem.onActionSaveTerrain();
	m_ToolSystem.onActionSave();
}

void MFCMain::ToolBarButton2()
{
	m_ToolSystem.UndoFunction();
}

void MFCMain::ToolBarButton3()
{
	m_ToolSystem.RedoFunction();
}

void MFCMain::ToolBarButton4()
{
	m_ToolSystem.SetToolState(ToolState::GIZMO);
	m_frame->m_toolBar.GetToolBarCtrl().EnableButton(ID_GIZMO_BUTTON, FALSE);
	m_frame->m_toolBar.GetToolBarCtrl().EnableButton(ID_TERRAIN_BUTTON, TRUE);
	m_GizmoDialogue.ShowWindow(SW_SHOW);
	m_TerrainDialogue.ShowWindow(SW_HIDE);
}

void MFCMain::ToolBarButton5()
{
	m_ToolSystem.SetToolState(ToolState::TERRAIN);
	m_frame->m_toolBar.GetToolBarCtrl().EnableButton(ID_GIZMO_BUTTON, TRUE);
	m_frame->m_toolBar.GetToolBarCtrl().EnableButton(ID_TERRAIN_BUTTON, FALSE);
	m_GizmoDialogue.ShowWindow(SW_HIDE);
	m_TerrainDialogue.ShowWindow(SW_SHOW);
}

MFCMain::MFCMain()
{
}


MFCMain::~MFCMain()
{
}

void MFCMain::MenuEditGizmo()
{   
	CWnd* pPlaceholder = m_frame->GetDlgItem(IDC_SELECT_DIALOG_PLACEHOLDER);

	if (pPlaceholder) {
		// If the dialog hasn't already been created, create it
		if (!::IsWindow(m_GizmoDialogue.GetSafeHwnd())) {
			m_GizmoDialogue.Create(IDD_GIZMODIALOGUE, pPlaceholder);
		}

		// Modify the extended style of the dialog to remove borders and title bar
		m_GizmoDialogue.ModifyStyle(WS_CAPTION | WS_SYSMENU | WS_BORDER, 0, SWP_FRAMECHANGED);
		m_GizmoDialogue.ModifyStyleEx(WS_EX_DLGMODALFRAME, 0, SWP_FRAMECHANGED);


		// Get the client rectangle of the placeholder
		CRect rect;
		pPlaceholder->GetClientRect(&rect);

		m_GizmoDialogue.SetParent(pPlaceholder);

		// Move and resize the dialog to fit inside the placeholder
		m_GizmoDialogue.MoveWindow(&rect);



		// Show the dialog
		m_GizmoDialogue.ShowWindow(SW_SHOW);
	}
}

void MFCMain::MenuEditTerrain()
{
	CWnd* pPlaceholder = m_frame->GetDlgItem(IDC_SELECT_DIALOG_PLACEHOLDER);

	if (pPlaceholder) {
		// If the dialog hasn't already been created, create it
		if (!::IsWindow(m_TerrainDialogue.GetSafeHwnd())) {
			m_TerrainDialogue.Create(IDD_TERRAINDIALOGUE, pPlaceholder);
		}

		// Modify the extended style of the dialog to remove borders and title bar
		m_TerrainDialogue.ModifyStyle(WS_CAPTION | WS_SYSMENU | WS_BORDER, 0, SWP_FRAMECHANGED);
		m_TerrainDialogue.ModifyStyleEx(WS_EX_DLGMODALFRAME, 0, SWP_FRAMECHANGED);


		// Get the client rectangle of the placeholder
		CRect rect;
		pPlaceholder->GetClientRect(&rect);

		m_TerrainDialogue.SetParent(pPlaceholder);

		// Move and resize the dialog to fit inside the placeholder
		m_TerrainDialogue.MoveWindow(&rect);



		// Show the dialog
		m_TerrainDialogue.ShowWindow(SW_HIDE);
	}
}