// GizmoDialogue.cpp : implementation file
//

#include "stdafx.h"
#include "GizmoDialogue.h"

// GizmoDialogue dialog

IMPLEMENT_DYNAMIC(GizmoDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(GizmoDialogue, CDialogEx)
	ON_COMMAND(IDOK, &GizmoDialogue::End)					//ok button
	ON_EN_CHANGE(IDC_YPOS, &GizmoDialogue::OnEnChangeYpos)
END_MESSAGE_MAP()


GizmoDialogue::GizmoDialogue(CWnd* pParent, std::vector<SceneObject>* SceneGraph)		//constructor used in modal
	: CDialogEx(IDD_GIZMODIALOGUE, pParent)
{
	m_sceneGraph = SceneGraph;
}

GizmoDialogue::GizmoDialogue(CWnd* pParent)			//constructor used in modeless
	: CDialogEx(IDD_GIZMODIALOGUE, pParent)
{
}

GizmoDialogue::~GizmoDialogue()
{
}

///pass through pointers to the data in the tool we want to manipulate
void GizmoDialogue::SetObjectData(std::vector<SceneObject>* SceneGraph, int* selection)
{
	m_sceneGraph = SceneGraph;
	m_currentSelection = selection;

	//roll through all the objects in the scene graph and put an entry for each in the listbox
	int numSceneObjects = m_sceneGraph->size();
	for (int i = 0; i < numSceneObjects; i++)
	{
		//easily possible to make the data string presented more complex. showing other columns.
		std::wstring listBoxEntry = std::to_wstring(m_sceneGraph->at(i).ID);
		m_listBox.AddString(listBoxEntry.c_str());
	}
}



void GizmoDialogue::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void GizmoDialogue::End()
{
	DestroyWindow();	//destory the window properly.  INcluding the links and pointers created.  THis is so the dialogue can start again. 
}

void GizmoDialogue::Select()
{
	int index = m_listBox.GetCurSel();
	CString currentSelectionValue;

	m_listBox.GetText(index, currentSelectionValue);

	*m_currentSelection = _ttoi(currentSelectionValue);

}

BOOL GizmoDialogue::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	CWnd* pEdit = GetDlgItem(IDC_XPOS);
	if (pEdit)
	{
		pEdit->SetWindowTextW(L"My default text here");
	}



	//uncomment for modal only
/*	//roll through all the objects in the scene graph and put an entry for each in the listbox
	int numSceneObjects = m_sceneGraph->size();
	for (size_t i = 0; i < numSceneObjects; i++)
	{
		//easily possible to make the data string presented more complex. showing other columns.
		std::wstring listBoxEntry = std::to_wstring(m_sceneGraph->at(i).ID);
		m_listBox.AddString(listBoxEntry.c_str());
	}*/

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void GizmoDialogue::PostNcDestroy()
{
}


void GizmoDialogue::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}



void GizmoDialogue::OnEnChangeYpos()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void GizmoDialogue::ChangeSelectedObject(float x, float y, float z, float pitch, float yaw, float roll)
{
	
	CWnd* pEdit1 = GetDlgItem(IDC_XPOS);
	CWnd* pEdit2 = GetDlgItem(IDC_YPOS);
	CWnd* pEdit3 = GetDlgItem(IDC_ZPOS);
	CWnd* pEdit4 = GetDlgItem(IDC_PITCH);
	CWnd* pEdit5 = GetDlgItem(IDC_YAW);
	CWnd* pEdit6 = GetDlgItem(IDC_ROLL);

	if (pEdit1)
	{
		CString str;
		str.Format(L"%.2f", x);  // Format float to string with 3 decimal places

		pEdit1->SetWindowTextW(str);;
	}

	if (pEdit2)
	{
		CString str;
		str.Format(L"%.2f", y);  // Format float to string with 3 decimal places

		pEdit2->SetWindowTextW(str);;
	}

	if (pEdit3)
	{
		CString str;
		str.Format(L"%.2f", z);  // Format float to string with 3 decimal places

		pEdit3->SetWindowTextW(str);;
	}

	if (pEdit4)
	{
		CString str;
		str.Format(L"%.2f",pitch);  // Format float to string with 3 decimal places

		pEdit4->SetWindowTextW(str);;
	}

	if (pEdit5)
	{
		CString str;
		str.Format(L"%.2f", yaw);  // Format float to string with 3 decimal places

		pEdit5->SetWindowTextW(str);;
	}

	if (pEdit6)
	{
		CString str;
		str.Format(L"%.2f", roll);  // Format float to string with 3 decimal places

		pEdit6->SetWindowTextW(str);;
	}

}