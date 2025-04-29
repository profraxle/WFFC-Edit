#pragma once

#include <afxext.h>
#include "pch.h"
#include "Game.h"
#include "sqlite3.h"
#include "SceneObject.h"
#include "InputCommands.h"
#include <vector>
#include <stack>

class MFCMain;

class ToolMain
{
public: //methods
	ToolMain();
	~ToolMain();

	//onAction - These are the interface to MFC
	int		getCurrentSelectionID();										//returns the selection number of currently selected object so that It can be displayed.
	void	onActionInitialise(HWND handle, int width, int height);			//Passes through handle and hieght and width and initialises DirectX renderer and SQL LITE
	void	onActionFocusCamera();
	void	onActionLoad();													//load the current chunk
	afx_msg	void	onActionSave();											//save the current chunk
	afx_msg void	onActionSaveTerrain();									//save chunk geometry

	void	Tick(MSG *msg);
	void	UpdateInput(MSG *msg);


public:	//variables
	std::vector<SceneObject>    m_sceneGraph;	//our scenegraph storing all the objects in the current chunk
	ChunkObject					m_chunk;		//our landscape chunk
	int m_selectedObject;						//ID of current Selection

	int m_selectedGizmo;

	bool canSelect;

	DirectX::XMVECTOR point1;
	DirectX::XMVECTOR point2;

	std::vector<int> registeredIDs;

	int transformState;

	SceneObject copyObject;

	bool pasteTrigger, undoTrigger,redoTrigger;

	std::stack < std::vector<SceneObject>> history;
	void UpdateHistory();

	std::stack<std::vector<SceneObject>> futureHistory;
	void UpdateFutureHistory();

	void HandleGizmos();
	void HandleTerrain();


private:	//methods
	void	onContentAdded();


		
private:	//variables
	HWND	m_toolHandle;		//Handle to the  window
	Game	m_d3dRenderer;		//Instance of D3D rendering system for our tool
	InputCommands m_toolInputCommands;		//input commands that we want to use and possibly pass over to the renderer
	CRect	WindowRECT;		//Window area rectangle. 
	char	m_keyArray[256];
	sqlite3 *m_databaseConnection;	//sqldatabase handle

	int m_width;		//dimensions passed to directX
	int m_height;
	int m_currentChunk;			//the current chunk of thedatabase that we are operating on.  Dictates loading and saving. 
	

	MFCMain * m_MFCMain;
	ToolState toolState;


public:
	void SetMFCMain(MFCMain* n_MFCMain);
	
};

enum GizmoType
{
	TRANSLATE,
	ROTATE,
	SCALE,
	NONE
};

enum ToolState
{
	TERRAIN,
	GIZMO
};
