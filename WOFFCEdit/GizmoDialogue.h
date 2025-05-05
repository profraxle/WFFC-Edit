#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "afxwin.h"
#include "SceneObject.h"
#include <vector>

// GizmoDialogue dialog

class GizmoDialogue : public CDialogEx
{
	DECLARE_DYNAMIC(GizmoDialogue)

public:
	GizmoDialogue(CWnd* pParent, std::vector<SceneObject>* SceneGraph);   // modal // takes in out scenegraph in the constructor
	GizmoDialogue(CWnd* pParent = NULL);
	virtual ~GizmoDialogue();
	void SetObjectData(std::vector<SceneObject>* SceneGraph, int* Selection);	//passing in pointers to the data the class will operate on.

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GIZMODIALOGUE };
#endif

	int gizmoState;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void End();		//kill the dialogue
	afx_msg void Select();	//Item has been selected

	std::vector<SceneObject>* m_sceneGraph;
	int* m_currentSelection;
	
	int selectedID;

	bool updateFlags;

	DECLARE_MESSAGE_MAP()
public:
	// Control variable for more efficient access of the listbox
	CListBox m_listBox;
	virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedOk();

	float nX;
	float nY;
	float nZ;

	float nYaw;
	float nPitch;
	float nRoll;


	bool valueUpdated;

	void ChangeSelectedObject(float x, float y, float z, float pitch, float yaw, float roll, int ID);


	afx_msg void OnEnKillfocusXPos();
	afx_msg void OnEnKillfocusYPos();
	afx_msg void OnEnKillfocusZPos();

	afx_msg void OnEnKillfocusPitch();
	afx_msg void OnEnKillfocusYaw();
	afx_msg void OnEnKillfocusRoll();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	void OnKillFocus(UINT ctrlID, float& store);
	afx_msg void OnBnClickedTranslate();
	afx_msg void OnBnClickedRotate();
};


INT_PTR CALLBACK SelectProc(HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);