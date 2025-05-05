// GizmoDialogue.cpp : implementation file
//

#include "stdafx.h"
#include "GizmoDialogue.h"
#include "Enums.h"
// GizmoDialogue dialog

IMPLEMENT_DYNAMIC(GizmoDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(GizmoDialogue, CDialogEx)
	ON_COMMAND(IDOK, &GizmoDialogue::End)					//ok button
	ON_EN_KILLFOCUS(IDC_XPOS,&GizmoDialogue::OnEnKillfocusXPos)
	ON_EN_KILLFOCUS(IDC_YPOS, &GizmoDialogue::OnEnKillfocusYPos)
	ON_EN_KILLFOCUS(IDC_ZPOS, &GizmoDialogue::OnEnKillfocusZPos)
	ON_EN_KILLFOCUS(IDC_YAW, &GizmoDialogue::OnEnKillfocusYaw)
	ON_EN_KILLFOCUS(IDC_PITCH, &GizmoDialogue::OnEnKillfocusPitch)
	ON_EN_KILLFOCUS(IDC_ROLL, &GizmoDialogue::OnEnKillfocusRoll)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_TRANSLATE, &GizmoDialogue::OnBnClickedTranslate)
	ON_BN_CLICKED(IDC_ROTATE, &GizmoDialogue::OnBnClickedRotate)
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

	GetDlgItem(IDC_TRANSLATE)->EnableWindow(FALSE);

	bool updateFlags = true;

	CWnd* pEdit1 = GetDlgItem(IDC_XPOS);
	CWnd* pEdit2 = GetDlgItem(IDC_YPOS);
	CWnd* pEdit3 = GetDlgItem(IDC_ZPOS);
	CWnd* pEdit4 = GetDlgItem(IDC_PITCH);
	CWnd* pEdit5 = GetDlgItem(IDC_YAW);
	CWnd* pEdit6 = GetDlgItem(IDC_ROLL);

	if (pEdit1)
	{

		pEdit1->SetWindowTextW(L"0.00");
	}

	if (pEdit2)
	{


		pEdit2->SetWindowTextW(L"0.00");
	}

	if (pEdit3)
	{


		pEdit3->SetWindowTextW(L"0.00");
	}

	if (pEdit4)
	{

		pEdit4->SetWindowTextW(L"0.00");
	}

	if (pEdit5)
	{

		pEdit5->SetWindowTextW(L"0.00");
	}

	if (pEdit6)
	{

		pEdit6->SetWindowTextW(L"0.00");
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



void GizmoDialogue::ChangeSelectedObject(float x, float y, float z, float pitch, float yaw, float roll, int ID)
{
	updateFlags = false;
	CWnd* pEdit1 = GetDlgItem(IDC_XPOS);
	CWnd* pEdit2 = GetDlgItem(IDC_YPOS);
	CWnd* pEdit3 = GetDlgItem(IDC_ZPOS);
	CWnd* pEdit4 = GetDlgItem(IDC_PITCH);
	CWnd* pEdit5 = GetDlgItem(IDC_YAW);
	CWnd* pEdit6 = GetDlgItem(IDC_ROLL);
	CWnd* pEditFrame = GetDlgItem(IDC_PROPFRAME);

	nX = x;
	nY = y;
	nZ = z;

	nPitch = pitch;
	nYaw = yaw;
	nRoll = roll;

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

	if (pEditFrame)
	{
		CString str;
		str.Format(L"Object %i Properties",ID);  // Format float to string with 3 decimal places

		pEditFrame->SetWindowTextW(str);;
	}
	updateFlags = true;
}
void GizmoDialogue::OnEnKillfocusXPos()
{
	OnKillFocus(IDC_XPOS, nX);
}
void GizmoDialogue::OnEnKillfocusYPos()
{
	OnKillFocus(IDC_YPOS, nY);
}
void GizmoDialogue::OnEnKillfocusZPos()
{
	OnKillFocus(IDC_ZPOS, nZ);
}

void GizmoDialogue::OnEnKillfocusYaw()
{
	OnKillFocus(IDC_YAW, nYaw);
}
void GizmoDialogue::OnEnKillfocusPitch()
{
	OnKillFocus(IDC_PITCH, nPitch);
}
void GizmoDialogue::OnEnKillfocusRoll()
{
	OnKillFocus(IDC_ROLL, nRoll);
}



void GizmoDialogue::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Set focus to the dialog itself (removes focus from edit boxes)
	SetFocus();

	CDialogEx::OnLButtonDown(nFlags, point);
}

void GizmoDialogue::OnKillFocus(UINT ctrlID, float& store)
{

	if (!updateFlags) {
		return;
	}

	CString str;
	CWnd* pEdit = GetDlgItem(ctrlID);
	pEdit->GetWindowText(str);

	store = _wtoi(str);

	valueUpdated = true;
}


void GizmoDialogue::OnBnClickedTranslate()
{
	valueUpdated = true;
	gizmoState = GizmoState::TRANSLATE;

	GetDlgItem(IDC_TRANSLATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_ROTATE)->EnableWindow(TRUE);
}

void GizmoDialogue::OnBnClickedRotate()
{
	valueUpdated = true;
	gizmoState = GizmoState::ROTATE;

	GetDlgItem(IDC_TRANSLATE)->EnableWindow(TRUE);
	GetDlgItem(IDC_ROTATE)->EnableWindow(FALSE);
}
