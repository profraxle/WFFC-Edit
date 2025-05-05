#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "afxwin.h"
#include "SceneObject.h"
#include <vector>

// TerrainDialogue dialog

class TerrainDialogue : public CDialogEx
{
	DECLARE_DYNAMIC(TerrainDialogue)

public:
	TerrainDialogue(CWnd* pParent, std::vector<SceneObject>* SceneGraph);   // modal // takes in out scenegraph in the constructor
	TerrainDialogue(CWnd* pParent = NULL);
	virtual ~TerrainDialogue();

	bool valueUpdated;
	int storeState;
	float storeRadius;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TERRAINDIALOGUE};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void End();		//kill the dialogue

	bool updateFlags;




	DECLARE_MESSAGE_MAP()
public:
	// Control variable for more efficient access of the listbox
	CListBox m_listBox;
	virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedOk();

	afx_msg void OnEnKillFocusRadius();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	void OnKillFocus(UINT ctrlID, float& store);
	afx_msg void OnBnClickedRaiseterrain();
	afx_msg void OnBnClickedLowterrain();
	afx_msg void OnBnClickedFlatterrain();
	afx_msg void OnBnClickedSmoothterrain();

	void ChangeRadius(float nRadius);
};


INT_PTR CALLBACK SelectProc(HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);