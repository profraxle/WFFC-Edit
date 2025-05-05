// TerrainDialogue.cpp : implementation file
//

#include "stdafx.h"
#include "TerrainDialogue.h"
#include "Enums.h"

// TerrainDialogue dialog

IMPLEMENT_DYNAMIC(TerrainDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(TerrainDialogue, CDialogEx)
	ON_COMMAND(IDOK, &TerrainDialogue::End)					//ok button
	ON_EN_KILLFOCUS(IDC_RADIUS, &TerrainDialogue::OnEnKillFocusRadius)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_RAISETERRAIN, &TerrainDialogue::OnBnClickedRaiseterrain)
	ON_BN_CLICKED(IDC_LOWTERRAIN, &TerrainDialogue::OnBnClickedLowterrain)
	ON_BN_CLICKED(IDC_FLATTERRAIN, &TerrainDialogue::OnBnClickedFlatterrain)
	ON_BN_CLICKED(IDC_SMOOTHTERRAIN, &TerrainDialogue::OnBnClickedSmoothterrain)
END_MESSAGE_MAP()


TerrainDialogue::TerrainDialogue(CWnd* pParent, std::vector<SceneObject>* SceneGraph)		//constructor used in modal
	: CDialogEx(IDD_TERRAINDIALOGUE, pParent)
{
}

TerrainDialogue::TerrainDialogue(CWnd* pParent)			//constructor used in modeless
	: CDialogEx(IDD_TERRAINDIALOGUE, pParent)
{
}

TerrainDialogue::~TerrainDialogue()
{
}

void TerrainDialogue::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void TerrainDialogue::End()
{
	DestroyWindow();	//destory the window properly.  INcluding the links and pointers created.  THis is so the dialogue can start again. 
}



BOOL TerrainDialogue::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	updateFlags = true;
	GetDlgItem(IDC_RAISETERRAIN)->EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void TerrainDialogue::PostNcDestroy()
{
}


void TerrainDialogue::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}

void TerrainDialogue::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Set focus to the dialog itself (removes focus from edit boxes)
	SetFocus();

	CDialogEx::OnLButtonDown(nFlags, point);
}

void TerrainDialogue::OnEnKillFocusRadius()
{

	if (!updateFlags) {
		return;
	}

	CString str;
	CWnd* pEdit = GetDlgItem(IDC_RADIUS);
	pEdit->GetWindowText(str);

	storeRadius = _wtoi(str);

	valueUpdated = true;
}


void TerrainDialogue::OnBnClickedRaiseterrain()
{
	valueUpdated = true;
	storeState = TerrainState::RAISE;

	GetDlgItem(IDC_RAISETERRAIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_LOWTERRAIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_FLATTERRAIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_SMOOTHTERRAIN)->EnableWindow(TRUE);

}

void TerrainDialogue::OnBnClickedLowterrain()
{
	valueUpdated = true;
	storeState = TerrainState::LOWER;

	GetDlgItem(IDC_RAISETERRAIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_LOWTERRAIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_FLATTERRAIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_SMOOTHTERRAIN)->EnableWindow(TRUE);
}

void TerrainDialogue::OnBnClickedFlatterrain()
{
	valueUpdated = true;
	storeState = TerrainState::FLATTEN;

	GetDlgItem(IDC_RAISETERRAIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_LOWTERRAIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_FLATTERRAIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_SMOOTHTERRAIN)->EnableWindow(TRUE);
}

void TerrainDialogue::OnBnClickedSmoothterrain()
{
	valueUpdated = true;
	storeState = TerrainState::SMOOTH;

	GetDlgItem(IDC_RAISETERRAIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_LOWTERRAIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_FLATTERRAIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_SMOOTHTERRAIN)->EnableWindow(FALSE);
}

void TerrainDialogue::ChangeRadius(float nRadius)
{
	CString str;
	str.Format(L"%.2f", nRadius);  // Format float to string with 3 decimal places
	storeRadius = nRadius;

	GetDlgItem(IDC_RADIUS)->SetWindowTextW(str);
}
