#include "ToolMain.h"
#include "resource.h"
#include <vector>
#include <sstream>
#include "Mouse.h"
#include "MFCMain.h"
#include "enums.h"
//
//ToolMain Class
ToolMain::ToolMain()
{

	m_currentChunk = 0;		//default value
	m_selectedObject = -1;	//initial selection ID
	m_sceneGraph.clear();	//clear the vector for the scenegraph
	m_databaseConnection = NULL;

	//zero input commands
	m_toolInputCommands.forward		= false;
	m_toolInputCommands.back		= false;
	m_toolInputCommands.left		= false;
	m_toolInputCommands.right		= false;

	m_toolInputCommands.mouse_LB_Down = false;
	m_toolInputCommands.mouse_X = 0;
	m_toolInputCommands.mouse_Y = 0;

	m_selectedGizmo = -1;

	m_gizmoState = GizmoState::TRANSLATE;
	m_toolState = ToolState::GIZMO;
	m_TerrainState = TerrainState::RAISE;

	terrainActionDone = false;
}


ToolMain::~ToolMain()
{
	sqlite3_close(m_databaseConnection);		//close the database connection
}


int ToolMain::getCurrentSelectionID()
{

	return m_selectedObject;
}

void ToolMain::onActionInitialise(HWND handle, int width, int height)
{
	//window size, handle etc for directX
	m_width		= width;
	m_height	= height;
	

	m_d3dRenderer.Initialize(handle, m_width, m_height);

	//database connection establish
	int rc;
	rc = sqlite3_open_v2("database/test.db",&m_databaseConnection, SQLITE_OPEN_READWRITE, NULL);

	if (rc) 
	{
		TRACE("Can't open database");
		//if the database cant open. Perhaps a more catastrophic error would be better here
	}
	else 
	{
		TRACE("Opened database successfully");
	}

	onActionLoad();
}

void ToolMain::onActionLoad()
{
	//load current chunk and objects into lists
	if (!m_sceneGraph.empty())		//is the vector empty
	{
		m_sceneGraph.clear();		//if not, empty it
	}

	//SQL
	int rc;
	char *sqlCommand;
	char *ErrMSG = 0;
	sqlite3_stmt *pResults;								//results of the query
	sqlite3_stmt *pResultsChunk;

	//OBJECTS IN THE WORLD
	//prepare SQL Text
	sqlCommand = "SELECT * from Objects";				//sql command which will return all records from the objects table.
	//Send Command and fill result object
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0 );
	
	//loop for each row in results until there are no more rows.  ie for every row in the results. We create and object
	while (sqlite3_step(pResults) == SQLITE_ROW)
	{	
		SceneObject newSceneObject;
		newSceneObject.ID = sqlite3_column_int(pResults, 0);
		newSceneObject.chunk_ID = sqlite3_column_int(pResults, 1);
		newSceneObject.model_path		= reinterpret_cast<const char*>(sqlite3_column_text(pResults, 2));
		newSceneObject.tex_diffuse_path = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 3));
		newSceneObject.posX = sqlite3_column_double(pResults, 4);
		newSceneObject.posY = sqlite3_column_double(pResults, 5);
		newSceneObject.posZ = sqlite3_column_double(pResults, 6);
		newSceneObject.rotX = sqlite3_column_double(pResults, 7);
		newSceneObject.rotY = sqlite3_column_double(pResults, 8);
		newSceneObject.rotZ = sqlite3_column_double(pResults, 9);
		newSceneObject.scaX = sqlite3_column_double(pResults, 10);
		newSceneObject.scaY = sqlite3_column_double(pResults, 11);
		newSceneObject.scaZ = sqlite3_column_double(pResults, 12);
		newSceneObject.render = sqlite3_column_int(pResults, 13);
		newSceneObject.collision = sqlite3_column_int(pResults, 14);
		newSceneObject.collision_mesh = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 15));
		newSceneObject.collectable = sqlite3_column_int(pResults, 16);
		newSceneObject.destructable = sqlite3_column_int(pResults, 17);
		newSceneObject.health_amount = sqlite3_column_int(pResults, 18);
		newSceneObject.editor_render = sqlite3_column_int(pResults, 19);
		newSceneObject.editor_texture_vis = sqlite3_column_int(pResults, 20);
		newSceneObject.editor_normals_vis = sqlite3_column_int(pResults, 21);
		newSceneObject.editor_collision_vis = sqlite3_column_int(pResults, 22);
		newSceneObject.editor_pivot_vis = sqlite3_column_int(pResults, 23);
		newSceneObject.pivotX = sqlite3_column_double(pResults, 24);
		newSceneObject.pivotY = sqlite3_column_double(pResults, 25);
		newSceneObject.pivotZ = sqlite3_column_double(pResults, 26);
		newSceneObject.snapToGround = sqlite3_column_int(pResults, 27);
		newSceneObject.AINode = sqlite3_column_int(pResults, 28);
		newSceneObject.audio_path = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 29));
		newSceneObject.volume = sqlite3_column_double(pResults, 30);
		newSceneObject.pitch = sqlite3_column_double(pResults, 31);
		newSceneObject.pan = sqlite3_column_int(pResults, 32);
		newSceneObject.one_shot = sqlite3_column_int(pResults, 33);
		newSceneObject.play_on_init = sqlite3_column_int(pResults, 34);
		newSceneObject.play_in_editor = sqlite3_column_int(pResults, 35);
		newSceneObject.min_dist = sqlite3_column_double(pResults, 36);
		newSceneObject.max_dist = sqlite3_column_double(pResults, 37);
		newSceneObject.camera = sqlite3_column_int(pResults, 38);
		newSceneObject.path_node = sqlite3_column_int(pResults, 39);
		newSceneObject.path_node_start = sqlite3_column_int(pResults, 40);
		newSceneObject.path_node_end = sqlite3_column_int(pResults, 41);
		newSceneObject.parent_id = sqlite3_column_int(pResults, 42);
		newSceneObject.editor_wireframe = sqlite3_column_int(pResults, 43);
		newSceneObject.name = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 44));

		newSceneObject.light_type = sqlite3_column_int(pResults, 45);
		newSceneObject.light_diffuse_r = sqlite3_column_double(pResults, 46);
		newSceneObject.light_diffuse_g = sqlite3_column_double(pResults, 47);
		newSceneObject.light_diffuse_b = sqlite3_column_double(pResults, 48);
		newSceneObject.light_specular_r = sqlite3_column_double(pResults, 49);
		newSceneObject.light_specular_g = sqlite3_column_double(pResults, 50);
		newSceneObject.light_specular_b = sqlite3_column_double(pResults, 51);
		newSceneObject.light_spot_cutoff = sqlite3_column_double(pResults, 52);
		newSceneObject.light_constant = sqlite3_column_double(pResults, 53);
		newSceneObject.light_linear = sqlite3_column_double(pResults, 54);
		newSceneObject.light_quadratic = sqlite3_column_double(pResults, 55);
	
		registeredIDs.push_back(newSceneObject.ID);

		//send completed object to scenegraph
		m_sceneGraph.push_back(newSceneObject);
	}

	//THE WORLD CHUNK
	//prepare SQL Text
	sqlCommand = "SELECT * from Chunks";				//sql command which will return all records from  chunks table. There is only one tho.
														//Send Command and fill result object
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResultsChunk, 0);


	sqlite3_step(pResultsChunk);
	m_chunk.ID = sqlite3_column_int(pResultsChunk, 0);
	m_chunk.name = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 1));
	m_chunk.chunk_x_size_metres = sqlite3_column_int(pResultsChunk, 2);
	m_chunk.chunk_y_size_metres = sqlite3_column_int(pResultsChunk, 3);
	m_chunk.chunk_base_resolution = sqlite3_column_int(pResultsChunk, 4);
	m_chunk.heightmap_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 5));
	m_chunk.tex_diffuse_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 6));
	m_chunk.tex_splat_alpha_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 7));
	m_chunk.tex_splat_1_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 8));
	m_chunk.tex_splat_2_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 9));
	m_chunk.tex_splat_3_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 10));
	m_chunk.tex_splat_4_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 11));
	m_chunk.render_wireframe = sqlite3_column_int(pResultsChunk, 12);
	m_chunk.render_normals = sqlite3_column_int(pResultsChunk, 13);
	m_chunk.tex_diffuse_tiling = sqlite3_column_int(pResultsChunk, 14);
	m_chunk.tex_splat_1_tiling = sqlite3_column_int(pResultsChunk, 15);
	m_chunk.tex_splat_2_tiling = sqlite3_column_int(pResultsChunk, 16);
	m_chunk.tex_splat_3_tiling = sqlite3_column_int(pResultsChunk, 17);
	m_chunk.tex_splat_4_tiling = sqlite3_column_int(pResultsChunk, 18);


	//Process REsults into renderable
	m_d3dRenderer.BuildDisplayList(&m_sceneGraph);
	//build the renderable chunk 
	m_d3dRenderer.BuildDisplayChunk(&m_chunk);

}

void ToolMain::onActionSave()
{
	//SQL
	int rc;
	char *sqlCommand;
	char *ErrMSG = 0;
	sqlite3_stmt *pResults;								//results of the query
	

	//OBJECTS IN THE WORLD Delete them all
	//prepare SQL Text
	sqlCommand = "DELETE FROM Objects";	 //will delete the whole object table.   Slightly risky but hey.
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
	sqlite3_step(pResults);

	//Populate with our new objects
	std::wstring sqlCommand2;
	int numObjects = m_sceneGraph.size();	//Loop thru the scengraph.

	for (int i = 0; i < numObjects; i++)
	{
		std::stringstream command;
		command << "INSERT INTO Objects " 
			<<"VALUES(" << m_sceneGraph.at(i).ID << ","
			<< m_sceneGraph.at(i).chunk_ID  << ","
			<< "'" << m_sceneGraph.at(i).model_path <<"'" << ","
			<< "'" << m_sceneGraph.at(i).tex_diffuse_path << "'" << ","
			<< m_sceneGraph.at(i).posX << ","
			<< m_sceneGraph.at(i).posY << ","
			<< m_sceneGraph.at(i).posZ << ","
			<< m_sceneGraph.at(i).rotX << ","
			<< m_sceneGraph.at(i).rotY << ","
			<< m_sceneGraph.at(i).rotZ << ","
			<< m_sceneGraph.at(i).scaX << ","
			<< m_sceneGraph.at(i).scaY << ","
			<< m_sceneGraph.at(i).scaZ << ","
			<< m_sceneGraph.at(i).render << ","
			<< m_sceneGraph.at(i).collision << ","
			<< "'" << m_sceneGraph.at(i).collision_mesh << "'" << ","
			<< m_sceneGraph.at(i).collectable << ","
			<< m_sceneGraph.at(i).destructable << ","
			<< m_sceneGraph.at(i).health_amount << ","
			<< m_sceneGraph.at(i).editor_render << ","
			<< m_sceneGraph.at(i).editor_texture_vis << ","
			<< m_sceneGraph.at(i).editor_normals_vis << ","
			<< m_sceneGraph.at(i).editor_collision_vis << ","
			<< m_sceneGraph.at(i).editor_pivot_vis << ","
			<< m_sceneGraph.at(i).pivotX << ","
			<< m_sceneGraph.at(i).pivotY << ","
			<< m_sceneGraph.at(i).pivotZ << ","
			<< m_sceneGraph.at(i).snapToGround << ","
			<< m_sceneGraph.at(i).AINode << ","
			<< "'" << m_sceneGraph.at(i).audio_path << "'" << ","
			<< m_sceneGraph.at(i).volume << ","
			<< m_sceneGraph.at(i).pitch << ","
			<< m_sceneGraph.at(i).pan << ","
			<< m_sceneGraph.at(i).one_shot << ","
			<< m_sceneGraph.at(i).play_on_init << ","
			<< m_sceneGraph.at(i).play_in_editor << ","
			<< m_sceneGraph.at(i).min_dist << ","
			<< m_sceneGraph.at(i).max_dist << ","
			<< m_sceneGraph.at(i).camera << ","
			<< m_sceneGraph.at(i).path_node << ","
			<< m_sceneGraph.at(i).path_node_start << ","
			<< m_sceneGraph.at(i).path_node_end << ","
			<< m_sceneGraph.at(i).parent_id << ","
			<< m_sceneGraph.at(i).editor_wireframe << ","
			<< "'" << m_sceneGraph.at(i).name << "'" << ","

			<< m_sceneGraph.at(i).light_type << ","
			<< m_sceneGraph.at(i).light_diffuse_r << ","
			<< m_sceneGraph.at(i).light_diffuse_g << ","
			<< m_sceneGraph.at(i).light_diffuse_b << ","
			<< m_sceneGraph.at(i).light_specular_r << ","
			<< m_sceneGraph.at(i).light_specular_g << ","
			<< m_sceneGraph.at(i).light_specular_b << ","
			<< m_sceneGraph.at(i).light_spot_cutoff << ","
			<< m_sceneGraph.at(i).light_constant << ","
			<< m_sceneGraph.at(i).light_linear << ","
			<< m_sceneGraph.at(i).light_quadratic

			<< ")";
		std::string sqlCommand2 = command.str();
		rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand2.c_str(), -1, &pResults, 0);
		sqlite3_step(pResults);	
	}
}

void ToolMain::onActionSaveTerrain()
{
	m_d3dRenderer.SaveDisplayChunk(&m_chunk);
}

void ToolMain::Tick(MSG *msg)
{

	//set the tool mode in the renderer
	m_d3dRenderer.SetToolState(m_toolState);

	//Renderer Update Call
	m_d3dRenderer.Tick(&m_toolInputCommands);

	//chamge based on which tool is being used
	switch (m_toolState) {
	case ToolState::GIZMO:
		m_d3dRenderer.SetGizmoState(m_gizmoState);
		break;
	}

	//if the gizmo dialogue has been edited and sent a signal
	if (m_MFCMain->m_GizmoDialogue.valueUpdated) 
	{
		//do this if theres a selected object
		if (m_selectedObject != -1) {


			UpdateHistory();

			//load the selected objects position and rotation from the dialogue
			m_sceneGraph[m_selectedObject].posX = m_MFCMain->m_GizmoDialogue.nX;
			m_sceneGraph[m_selectedObject].posY = m_MFCMain->m_GizmoDialogue.nY;
			m_sceneGraph[m_selectedObject].posZ = m_MFCMain->m_GizmoDialogue.nZ;

			m_sceneGraph[m_selectedObject].rotX = m_MFCMain->m_GizmoDialogue.nPitch;
			m_sceneGraph[m_selectedObject].rotY = m_MFCMain->m_GizmoDialogue.nYaw;
			m_sceneGraph[m_selectedObject].rotZ = m_MFCMain->m_GizmoDialogue.nRoll;

			//update the display list
			m_d3dRenderer.UpdateDisplayList(&m_sceneGraph);

			//feedback into the dialogue with new options
			m_MFCMain->m_GizmoDialogue.ChangeSelectedObject(m_sceneGraph[m_selectedObject].posX, m_sceneGraph[m_selectedObject].posY, m_sceneGraph[m_selectedObject].posZ,
				m_sceneGraph[m_selectedObject].rotX, m_sceneGraph[m_selectedObject].rotY, m_sceneGraph[m_selectedObject].rotZ, m_selectedObject);
		}

		//set the gizmoState to the selected one
		m_gizmoState = m_MFCMain->m_GizmoDialogue.gizmoState;

		//unset signal
		m_MFCMain->m_GizmoDialogue.valueUpdated = false;
	}

	//if the radius of terrain tool has been changed from renderer
	if (m_d3dRenderer.m_changeRadius)
	{

		//set the radius value in the dialogue
		m_d3dRenderer.m_changeRadius = false;
		m_MFCMain->m_TerrainDialogue.ChangeRadius(m_d3dRenderer.m_terrainRadius);
	}

	//if the dialogue has been updated
	if (m_MFCMain->m_TerrainDialogue.valueUpdated) 
	{
		//set the new terrain state
		m_TerrainState = m_MFCMain->m_TerrainDialogue.storeState;
		m_MFCMain->m_TerrainDialogue.valueUpdated = false;

		//set the new terrain radius and feedback to the dialogue
		m_d3dRenderer.m_terrainRadius =  m_MFCMain->m_TerrainDialogue.storeRadius;
		m_MFCMain->m_TerrainDialogue.ChangeRadius(m_d3dRenderer.m_terrainRadius);
	}

	
	//get the mouse position on whole window
	POINT pt;
	GetCursorPos(&pt);

	// Get window under mouse
	HWND hWndUnderMouse = WindowFromPoint(pt);

	//check if the mouse is focused in renderer
	isInGame = (hWndUnderMouse == m_MFCMain->m_frame->m_DirXView.GetSafeHwnd());
	
	//set focus on the renderer if so
	if (isInGame) {
		m_MFCMain->m_frame->m_DirXView.SetFocus();		
	}

	//handle mouse being down for different tool states
	if (m_toolInputCommands.mouse_LB_Down && isInGame) 
	{
		switch (m_toolState)
		{
		case ToolState::GIZMO:
				HandleGizmos();
				break;
		case ToolState::TERRAIN:
				HandleTerrain();
				break;
		}
	}

	//handle mouse going down to select object
	if (canSelect && isInGame) {


		//store the selected object temporarily
		int tempSelect = m_d3dRenderer.MousePicking();

		int gizmoInteract = -1;
		gizmoInteract = m_d3dRenderer.MouseInteractGizmo();

		//if a different object is selected, update ID or deselect if same
		if (tempSelect == m_selectedObject ) {

			if (gizmoInteract != -1) {
				UpdateHistory();
				futureHistory = std::stack<Command>();
				m_selectedGizmo = gizmoInteract;
				
			}
			else {
				m_selectedObject = -1;
				m_selectedGizmo = -1;
			}
		}
		else {

			if (gizmoInteract != -1) {
				UpdateHistory();
				futureHistory = std::stack<Command>();
				m_selectedGizmo = gizmoInteract;

			}
			else {

				m_selectedObject = tempSelect;

				if (tempSelect != -1) {
					m_MFCMain->m_GizmoDialogue.ChangeSelectedObject(m_sceneGraph[m_selectedObject].posX, m_sceneGraph[m_selectedObject].posY, m_sceneGraph[m_selectedObject].posZ,
						m_sceneGraph[m_selectedObject].rotX, m_sceneGraph[m_selectedObject].rotY, m_sceneGraph[m_selectedObject].rotZ,m_selectedObject);
				}
			}
		}

		//set the selected object and gizmo within the renderer to the one selected here
		m_d3dRenderer.SetSelectedIndex(m_selectedObject);
		m_d3dRenderer.SetSelectedGizmo(m_selectedGizmo);
		canSelect = false;
	}
	

	//copy object
	if (m_toolInputCommands.copy) {

		if (m_selectedObject != -1) {
			copyObject = m_sceneGraph[m_selectedObject];
		}
	}

	//paste object
	if (m_toolInputCommands.paste && pasteTrigger) 
	{
			SceneObject newObj = copyObject;
			newObj.posY += 1;

			int IDtry = 0;
			bool IDfound = false;

			//register ID not found before
			while (!IDfound) {
				if (std::find(registeredIDs.begin(), registeredIDs.end(), IDtry) != registeredIDs.end()){
					++IDtry;
				}
				else {
					IDfound = true;
				}

			}
			newObj.ID = IDtry;

			UpdateHistory();
			futureHistory = std::stack<Command>();

			m_sceneGraph.push_back(newObj);
			m_toolInputCommands.paste = false;

			m_d3dRenderer.BuildDisplayList(&m_sceneGraph);
			pasteTrigger = false;
	}

	//undo and redo triggers

	if (m_toolInputCommands.undo && undoTrigger) {
		UndoFunction();
	}
	if (m_toolInputCommands.redo && redoTrigger) {
		RedoFunction();
	}

	//delete selected object
	if (m_toolInputCommands.deleteObject) {
		if (m_selectedObject != -1) {
			UpdateHistory();
			futureHistory = std::stack<Command>();
			m_sceneGraph.erase(m_sceneGraph.begin() + m_selectedObject);
			m_d3dRenderer.BuildDisplayList(&m_sceneGraph);
			m_selectedObject = -1;
			m_d3dRenderer.SetSelectedIndex(-1);
			m_d3dRenderer.SetSelectedGizmo(-1);
		}
	}

	//save the terrain and objects
	if (m_toolInputCommands.save && saveTrigger) 
	{
		onActionSaveTerrain();
		onActionSave();

		saveTrigger = false;
	}
}

void ToolMain::UpdateHistory() {


	BYTE heightMap[TERRAINRESOLUTION * TERRAINRESOLUTION];
	m_d3dRenderer.GetHeightmap(heightMap);


	Command newCommand(m_sceneGraph, m_selectedObject, heightMap);

	history.push(newCommand);
}

void ToolMain::UpdateFutureHistory() {
	BYTE heightMap[TERRAINRESOLUTION * TERRAINRESOLUTION];
	m_d3dRenderer.GetHeightmap(heightMap);


	Command newCommand(m_sceneGraph, m_selectedObject, heightMap);

	futureHistory.push(newCommand);
}

void ToolMain::SetMFCMain(MFCMain* n_MFCMain)
{
	m_MFCMain = n_MFCMain;
}

void ToolMain::UpdateInput(MSG* msg)
{

	switch (msg->message)
	{
		//Global inputs,  mouse position and keys etc
	case WM_KEYDOWN:
		m_keyArray[msg->wParam] = true;
		break;

	case WM_KEYUP:
		m_keyArray[msg->wParam] = false;
		break;

	case WM_MOUSEMOVE:
		DirectX::Mouse::ProcessMessage(msg->message, msg->wParam, msg->lParam);
		m_toolInputCommands.mouse_X = GET_X_LPARAM(msg->lParam);
		m_toolInputCommands.mouse_Y = GET_Y_LPARAM(msg->lParam);
		break;

	case WM_LBUTTONDOWN:	//mouse button down,  you will probably need to check when its up too
		//set some flag for the mouse button in inputcommands
		DirectX::Mouse::ProcessMessage(msg->message, msg->wParam, msg->lParam);
		m_toolInputCommands.mouse_LB_Down = true;
		canSelect = true;
		point1 = DirectX::XMVectorSet(0, 0, 0, 1);
		point2 = DirectX::XMVectorSet(0, 0, 0, 1);
		break;

	case WM_LBUTTONUP:
		DirectX::Mouse::ProcessMessage(msg->message, msg->wParam, msg->lParam);
		m_toolInputCommands.mouse_LB_Down = false;
		terrainActionDone = false;
		break;
	case WM_RBUTTONDOWN:	//mouse button down,  you will probably need to check when its up too
		//set some flag for the mouse button in inputcommands
		DirectX::Mouse::ProcessMessage(msg->message, msg->wParam, msg->lParam);
		break;

	case WM_RBUTTONUP:	//mouse button down,  you will probably need to check when its up too
		//set some flag for the mouse button in inputcommands
		DirectX::Mouse::ProcessMessage(msg->message, msg->wParam, msg->lParam);
		break;
	case WM_MOUSEWHEEL:
		DirectX::Mouse::ProcessMessage(msg->message, msg->wParam, msg->lParam);
		break;

	}
	//here we update all the actual app functionality that we want.  This information will either be used int toolmain, or sent down to the renderer (Camera movement etc
	//WASD movement

	if (m_keyArray['W'] && isInGame)
	{
		m_toolInputCommands.forward = true;
	}
	else m_toolInputCommands.forward = false;

	if (m_keyArray['S'] && isInGame && !m_toolInputCommands.control)
	{
		m_toolInputCommands.back = true;
	}
	else m_toolInputCommands.back = false;
	if (m_keyArray['A'] && isInGame)
	{
		m_toolInputCommands.left = true;
	}
	else m_toolInputCommands.left = false;

	if (m_keyArray['D'] && isInGame)
	{
		m_toolInputCommands.right = true;
	}
	else m_toolInputCommands.right = false;

	if (m_keyArray[17]) {
		m_toolInputCommands.control= true;
	}else  m_toolInputCommands.control = false;

	if (m_keyArray['C'] && m_toolInputCommands.control) {
		m_toolInputCommands.copy = true;
	}
	else m_toolInputCommands.copy = false;

	if (m_keyArray['V'] && m_toolInputCommands.control) {
		m_toolInputCommands.paste = true;
	}
	else {
		m_toolInputCommands.paste = false;
		pasteTrigger = true;
	}

	if (m_keyArray['Z'] && m_toolInputCommands.control) {
		m_toolInputCommands.undo = true;
	}
	else {
		m_toolInputCommands.undo = false;
		undoTrigger = true;
	}

	if (m_keyArray['Y'] && m_toolInputCommands.control) {
		m_toolInputCommands.redo = true;
	}
	else {
		m_toolInputCommands.redo = false;
		redoTrigger = true;
	}

	if (m_keyArray[46]) {
		m_toolInputCommands.deleteObject = true;
	}
	else {
		m_toolInputCommands.deleteObject = false;
	}

	if (m_keyArray['T'] && m_toolInputCommands.control) {
		m_MFCMain->ToolBarButton5();
		m_MFCMain->m_TerrainDialogue.ChangeRadius(m_d3dRenderer.m_terrainRadius);
	}
	else if (m_keyArray['T'] && m_toolState == ToolState::GIZMO) {
		m_MFCMain->m_GizmoDialogue.OnBnClickedTranslate();
	}

	if (m_keyArray['R'] && m_toolState == ToolState::GIZMO) {
		m_MFCMain->m_GizmoDialogue.OnBnClickedRotate();
	}

	if (m_keyArray['G'] && m_toolInputCommands.control) {
		m_MFCMain->ToolBarButton4();
	}

	if (m_keyArray['F'] && m_toolState == ToolState::TERRAIN) {
		m_MFCMain->m_TerrainDialogue.OnBnClickedFlatterrain();

	}

	if (m_keyArray['R'] && m_toolState == ToolState::TERRAIN) {
		m_MFCMain->m_TerrainDialogue.OnBnClickedRaiseterrain();

	}

	if (m_keyArray['I'] && m_toolState == ToolState::TERRAIN) {
		m_MFCMain->m_TerrainDialogue.OnBnClickedSmoothterrain();

	}

	if (m_keyArray['L'] && m_toolState == ToolState::TERRAIN) {
		m_MFCMain->m_TerrainDialogue.OnBnClickedLowterrain();

	}


	if (m_keyArray['S'] && m_toolInputCommands.control) {
		m_toolInputCommands.save = true;
	}
	else {
		m_toolInputCommands.save = false;
		saveTrigger = true;
	}


}


void ToolMain::HandleGizmos() {
	
	if (m_selectedGizmo != -1 && m_selectedObject != -1) {

		//push the point back
		point1 = point2;

		//define axes
		DirectX::XMVECTOR axis;
		DirectX::XMVECTOR axis2;

		if (m_selectedGizmo == 0) {
			axis = DirectX::XMVectorSet(0, 1, 0, 0);

		}
		if (m_selectedGizmo == 1) {
			axis = DirectX::XMVectorSet(0, 0, 1, 0);
		}
		if (m_selectedGizmo == 2) {
			axis = DirectX::XMVectorSet(1, 0, 0, 0);
		}

		//rotate axis for local rotation
		DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(m_sceneGraph[m_selectedObject].rotX, m_sceneGraph[m_selectedObject].rotY, m_sceneGraph[m_selectedObject].rotZ);
		axis2 = DirectX::XMVector3Rotate(axis, quat);

		//switch state of gizmo and call the drag functions
		switch (m_gizmoState) {
		case GizmoState::TRANSLATE:
			point2 = m_d3dRenderer.DragGizmo(axis);
			break;
		case GizmoState::ROTATE:
			point2 = m_d3dRenderer.DragRotGizmo(axis);
			break;
		}

		//quick and dirty check to see if the point exists
		if (DirectX::XMVectorGetX(point1) != 0 && DirectX::XMVectorGetY(point1) != 0, DirectX::XMVectorGetZ(point1) != 0) {
			if (DirectX::XMVectorGetX(point2) != 0 && DirectX::XMVectorGetY(point2) != 0, DirectX::XMVectorGetZ(point2) != 0) {
				DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(point2, point1);

				//move the object based on whether its translation or rotation
				switch (m_gizmoState) {

				case GizmoState::TRANSLATE:
					if (m_selectedGizmo == 0) {
						m_sceneGraph[m_selectedObject].posY += DirectX::XMVectorGetY(diff);
					}
					if (m_selectedGizmo == 1) {
						m_sceneGraph[m_selectedObject].posZ += DirectX::XMVectorGetZ(diff);

					}
					if (m_selectedGizmo == 2) {
						m_sceneGraph[m_selectedObject].posX += DirectX::XMVectorGetX(diff);
					}

					//callback into side panel
					m_MFCMain->m_GizmoDialogue.ChangeSelectedObject(m_sceneGraph[m_selectedObject].posX, m_sceneGraph[m_selectedObject].posY, m_sceneGraph[m_selectedObject].posZ,
						m_sceneGraph[m_selectedObject].rotX, m_sceneGraph[m_selectedObject].rotY, m_sceneGraph[m_selectedObject].rotZ, m_selectedObject);
					break;
				case GizmoState::ROTATE:

					float angle = m_d3dRenderer.GetAngleDiff(point2, point1, axis2);

					if (std::isfinite(angle)) {

						if (m_selectedGizmo == 0) {
							m_sceneGraph[m_selectedObject].rotY += angle;
						}
						if (m_selectedGizmo == 1) {
							m_sceneGraph[m_selectedObject].rotZ += angle;

						}
						if (m_selectedGizmo == 2) {
							m_sceneGraph[m_selectedObject].rotX += angle;
						}

						//callback into side panel
						m_MFCMain->m_GizmoDialogue.ChangeSelectedObject(m_sceneGraph[m_selectedObject].posX, m_sceneGraph[m_selectedObject].posY, m_sceneGraph[m_selectedObject].posZ,
							m_sceneGraph[m_selectedObject].rotX, m_sceneGraph[m_selectedObject].rotY, m_sceneGraph[m_selectedObject].rotZ, m_selectedObject);
					}

					break;
				}

				//update the rendered list
				m_d3dRenderer.UpdateDisplayList(&m_sceneGraph);
			}
		}
	}

}

void ToolMain::HandleTerrain() 
{
	//update the history
	if (!terrainActionDone) {
		UpdateHistory();
		terrainActionDone= true;
	}

	//switch which tool is being used
	switch (m_TerrainState) {
	case TerrainState::RAISE:
		m_d3dRenderer.TerrainRaiseLower(true);
		break;
	case TerrainState::LOWER:
		m_d3dRenderer.TerrainRaiseLower(false);
		break;
	case TerrainState::FLATTEN:
		m_d3dRenderer.TerrainFlatten();
		break;
	case TerrainState::SMOOTH:
		m_d3dRenderer.TerrainSmooth();
		break;
	}
}

void ToolMain::UndoFunction() {

	if (!history.empty()) {

		//push to future history
		UpdateFutureHistory();

		//restore data from the command
		m_sceneGraph = history.top().sceneGraph;
		m_selectedObject = history.top().selectedObject;
		m_d3dRenderer.SetHeightmap(history.top().m_heightMap);
		m_d3dRenderer.SetSelectedIndex(m_selectedObject);

		//pop the history stack
		history.pop();
		

		//rebuild the display list
		m_d3dRenderer.BuildDisplayList(&m_sceneGraph);
		undoTrigger = false;
	}
}
void ToolMain::RedoFunction() {
	if (!futureHistory.empty()) {
		//push to history
		UpdateHistory();
		//restore data from the command
		m_sceneGraph = futureHistory.top().sceneGraph;
		m_selectedObject = futureHistory.top().selectedObject;
		m_d3dRenderer.SetSelectedIndex(m_selectedObject);

		//pop the history stack
		futureHistory.pop();
		
		//rebuild the display list
		m_d3dRenderer.BuildDisplayList(&m_sceneGraph);
		redoTrigger = false;
	}
}

void ToolMain::SetToolState(int state)
{
	m_toolState = state;
}