#include "UndoRedoDialogue.h"
// UndoRedoDialogue.cpp : implementation file
//

#include "stdafx.h"
#include "UndoRedoDialogue.h"

// UndoRedoDialogue dialog

IMPLEMENT_DYNAMIC(UndoRedoDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(UndoRedoDialogue, CDialogEx)
	ON_COMMAND(IDOK, &UndoRedoDialogue::End)					//ok button
	ON_BN_CLICKED(IDOK, &UndoRedoDialogue::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_LIST1, &UndoRedoDialogue::Select)	//listbox
END_MESSAGE_MAP()


UndoRedoDialogue::UndoRedoDialogue(CWnd* pParent, std::vector<SceneObject>* SceneGraph)		//constructor used in modal
	: CDialogEx(IDD_UNDOREDO, pParent)
{
	m_sceneGraph = SceneGraph;
}

UndoRedoDialogue::UndoRedoDialogue(CWnd* pParent)			//constructor used in modeless
	: CDialogEx(IDD_UNDOREDO, pParent)
{
}

UndoRedoDialogue::~UndoRedoDialogue()
{
}

///pass through pointers to the data in the tool we want to manipulate
void UndoRedoDialogue::SetObjectData(std::vector<SceneObject>* SceneGraph, int* selection)
{
	m_sceneGraph = SceneGraph;
	m_currentSelection = selection;

	//roll through all the objects in the scene graph and put an entry for each in the listbox
	//int numSceneObjects = m_sceneGraph->size();
	for (int i = 0; i < 400; i++)
	{
		//easily possible to make the data string presented more complex. showing other columns.
		std::wstring listBoxEntry = std::to_wstring(i);
		m_listBox.AddString(listBoxEntry.c_str());
	}
}


void UndoRedoDialogue::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACTIONLIST, m_listBox);
}

void UndoRedoDialogue::End()
{
	DestroyWindow();	//destory the window properly.  INcluding the links and pointers created.  THis is so the dialogue can start again. 
}

void UndoRedoDialogue::Select()
{
	int index = m_listBox.GetCurSel();
	CString currentSelectionValue;

	m_listBox.GetText(index, currentSelectionValue);

	*m_currentSelection = _ttoi(currentSelectionValue);

}

BOOL UndoRedoDialogue::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

void UndoRedoDialogue::PostNcDestroy()
{
}




// UndoRedoDialogue message handlers callback   - We only need this if the dailogue is being setup-with createDialogue().  We are doing
//it manually so its better to use the messagemap
/*INT_PTR CALLBACK SelectProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		//	EndDialog(hwndDlg, wParam);
			DestroyWindow(hwndDlg);
			return TRUE;


		case IDCANCEL:
			EndDialog(hwndDlg, wParam);
			return TRUE;
			break;
		}
	}

	return INT_PTR();
}*/


void UndoRedoDialogue::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}

