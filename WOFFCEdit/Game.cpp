//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "DisplayObject.h"
#include <string>
#include <Windows.h>
#include <iostream>


using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game()

{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
	m_displayList.clear();
	
	//initial Settings
	//modes
	m_grid = false;

	//functional
	m_movespeed = 0.30;
	m_camRotRate = 3.0;

	//camera
	m_camPosition.x = 0.0f;
	m_camPosition.y = 3.7f;
	m_camPosition.z = -3.5f;

	m_camOrientation.x = 0;
	m_camOrientation.y = 0;
	m_camOrientation.z = 0;

	m_camLookAt.x = 0.0f;
	m_camLookAt.y = 0.0f;
	m_camLookAt.z = 0.0f;

	m_camLookDirection.x = 0.0f;
	m_camLookDirection.y = 0.0f;
	m_camLookDirection.z = 0.0f;

	m_camRight.x = 0.0f;
	m_camRight.y = 0.0f;
	m_camRight.z = 0.0f;

	m_camOrientation.x = 0.0f;
	m_camOrientation.y = 0.0f;
	m_camOrientation.z = 0.0f;

	m_SelectedIndex = 0;

}

Game::~Game()
{

#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_gamePad = std::make_unique<GamePad>();

    m_keyboard = std::make_unique<Keyboard>();

    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	GetClientRect(window, &m_ScreenDimensions);

#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

void Game::SetGridState(bool state)
{
	m_grid = state;
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick(InputCommands *Input)
{
	//copy over the input commands so we have a local version to use elsewhere.
	m_InputCommands = *Input;
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	//TODO  any more complex than this, and the camera should be abstracted out to somewhere else
	//camera motion is on a plane, so kill the 7 component of the look direction
	Vector3 planarMotionVector = m_camLookDirection;
	planarMotionVector.y = 0.0;

	 mouseState = m_mouse->GetState();



	if (mouseState.rightButton) 
	{
		Vector2 diff = mousePos - Vector2(mouseState.x, mouseState.y);
		
		m_camOrientation.y -= diff.x;
		m_camOrientation.x += diff.y;
	}
	mousePos = Vector2(mouseState.x, mouseState.y);

	if (m_camOrientation.x >= 90)
	{
		m_camOrientation.x = 90;
	}
	else if (m_camOrientation.x <= -90) {
		m_camOrientation.x = -90;
	}


	//create look direction from Euler angles in m_camOrientation
	m_camLookDirection.x = cos((m_camOrientation.y)*3.1415 / 180) * cos((m_camOrientation.x) * 3.1415 / 180);
	m_camLookDirection.y = sin((m_camOrientation.x)*3.1415 / 180);
	m_camLookDirection.z = sin((m_camOrientation.y) * 3.1415 / 180) * cos((m_camOrientation.x) * 3.1415 / 180);
	m_camLookDirection.Normalize();

	//create right vector from look Direction
	m_camLookDirection.Cross(Vector3::UnitY, m_camRight);

	//process input and update stuff
	if (m_InputCommands.forward)
	{	
		m_camPosition += m_camLookDirection*m_movespeed;
	}
	if (m_InputCommands.back)
	{
		m_camPosition -= m_camLookDirection*m_movespeed;
	}
	if (m_InputCommands.right)
	{
		m_camPosition += m_camRight*m_movespeed;
	}
	if (m_InputCommands.left)
	{
		m_camPosition -= m_camRight*m_movespeed;
	}




	//update lookat point
	m_camLookAt = m_camPosition + m_camLookDirection;

	//apply camera vectors
    m_view = Matrix::CreateLookAt(m_camPosition, m_camLookAt, Vector3::UnitY);

    m_batchEffect->SetView(m_view);
    m_batchEffect->SetWorld(Matrix::Identity);
	m_displayChunk.m_terrainEffect->SetView(m_view);
	m_displayChunk.m_terrainEffect->SetWorld(Matrix::Identity);

#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

   
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{

    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
	
	if (m_grid)
	{
		// Draw procedurally generated dynamic grid
		const XMVECTORF32 xaxis = { 512.f, 0.f, 0.f };
		const XMVECTORF32 yaxis = { 0.f, 0.f, 512.f };
		DrawGrid(xaxis, yaxis, g_XMZero, 512, 512, Colors::Gray);
	}
	//CAMERA POSITION ON HUD
	m_sprites->Begin();
	WCHAR   Buffer[256];
	std::wstring var = L"Cam X: " + std::to_wstring(mouseState.x) + L"Cam Z: " + std::to_wstring(mouseState.y);
	m_font->DrawString(m_sprites.get(), var.c_str() , XMFLOAT2(100, 10), Colors::Yellow);
	m_sprites->End();

	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	//RENDER OBJECTS FROM SCENEGRAPH
	int numRenderObjects = m_displayList.size();
	for (int i = 0; i < numRenderObjects; i++)
	{
		m_deviceResources->PIXBeginEvent(L"Draw model");
		const XMVECTORF32 scale = { m_displayList[i].m_scale.x, m_displayList[i].m_scale.y, m_displayList[i].m_scale.z };
		const XMVECTORF32 translate = { m_displayList[i].m_position.x, m_displayList[i].m_position.y, m_displayList[i].m_position.z };

		//convert degrees into radians for rotation matrix
		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_displayList[i].m_orientation.y *3.1415 / 180,
															m_displayList[i].m_orientation.x *3.1415 / 180,
															m_displayList[i].m_orientation.z *3.1415 / 180);

		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);

		m_displayList[i].m_model->Draw(context, *m_states, local, m_view, m_projection, false);	//last variable in draw,  make TRUE for wireframe

		m_deviceResources->PIXEndEvent();
	}

	


    m_deviceResources->PIXEndEvent();

	//RENDER TERRAIN
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(),0);
	context->RSSetState(m_states->CullNone());
//	context->RSSetState(m_states->Wireframe());		//uncomment for wireframe

	//Render the batch,  This is handled in the Display chunk becuase it has the potential to get complex
	m_displayChunk.RenderBatch(m_deviceResources);

	context->OMSetBlendState(m_states->AlphaBlend(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	context->RSSetState(m_states->CullNone());

	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	//render gizmo if selected object
	if (m_SelectedIndex != -1) {
		m_deviceResources->PIXBeginEvent(L"Draw model");
		const XMVECTORF32 scale = { 4, 4, 4 };
		const XMVECTORF32 translate = { m_displayList[m_SelectedIndex].m_position.x, m_displayList[m_SelectedIndex].m_position.y, m_displayList[m_SelectedIndex].m_position.z };

		for (int i = 0; i < 3; i++) {


			XMVECTOR rotate;
			switch (i) {
			case 0:
				// convert degrees into radians for rotation matrix
				rotate = Quaternion::CreateFromYawPitchRoll(0 * 3.1415 / 180,
					0 * 3.1415 / 180,
					0 * 3.1415 / 180);
				break;
			case 1:
				rotate = Quaternion::CreateFromYawPitchRoll(0 * 3.1415 / 180,
					90 * 3.1415 / 180,
					0 * 3.1415 / 180);
				break;
			case 2:
				rotate = Quaternion::CreateFromYawPitchRoll(0 * 3.1415 / 180,
					90 * 3.1415 / 180,
					90 * 3.1415 / 180);
				break;
			}




			XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);



			
			m_gizmoModels[m_gizmoMode][i]->Draw(context, *m_states, local, m_view, m_projection);

			context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
			context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
		}

		
	}
	m_deviceResources->PIXEndEvent();

    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

void XM_CALLCONV Game::DrawGrid(FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color)
{
    m_deviceResources->PIXBeginEvent(L"Draw grid");

    auto context = m_deviceResources->GetD3DDeviceContext();
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);
    context->RSSetState(m_states->CullCounterClockwise());

    m_batchEffect->Apply(context);

    context->IASetInputLayout(m_batchInputLayout.Get());

    m_batch->Begin();

    xdivs = std::max<size_t>(1, xdivs);
    ydivs = std::max<size_t>(1, ydivs);

    for (size_t i = 0; i <= xdivs; ++i)
    {
        float fPercent = float(i) / float(xdivs);
        fPercent = (fPercent * 2.0f) - 1.0f;
        XMVECTOR vScale = XMVectorScale(xAxis, fPercent);
        vScale = XMVectorAdd(vScale, origin);

        VertexPositionColor v1(XMVectorSubtract(vScale, yAxis), color);
        VertexPositionColor v2(XMVectorAdd(vScale, yAxis), color);
        m_batch->DrawLine(v1, v2);
    }

    for (size_t i = 0; i <= ydivs; i++)
    {
        float fPercent = float(i) / float(ydivs);
        fPercent = (fPercent * 2.0f) - 1.0f;
        XMVECTOR vScale = XMVectorScale(yAxis, fPercent);
        vScale = XMVectorAdd(vScale, origin);

        VertexPositionColor v1(XMVectorSubtract(vScale, xAxis), color);
        VertexPositionColor v2(XMVectorAdd(vScale, xAxis), color);
        m_batch->DrawLine(v1, v2);
    }

    m_batch->End();

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

void Game::BuildDisplayList(std::vector<SceneObject> * SceneGraph)
{
	auto device = m_deviceResources->GetD3DDevice();
	auto devicecontext = m_deviceResources->GetD3DDeviceContext();

	if (!m_displayList.empty())		//is the vector empty
	{
		m_displayList.clear();		//if not, empty it
	}

	//for every item in the scenegraph
	int numObjects = SceneGraph->size();
	for (int i = 0; i < numObjects; i++)
	{
		
		//create a temp display object that we will populate then append to the display list.
		DisplayObject newDisplayObject;
		
		//load model
		std::wstring modelwstr = StringToWCHART(SceneGraph->at(i).model_path);							//convect string to Wchar
		newDisplayObject.m_model = Model::CreateFromCMO(device, modelwstr.c_str(), *m_fxFactory, true);	//get DXSDK to load model "False" for LH coordinate system (maya)

		//Load Texture
		std::wstring texturewstr = StringToWCHART(SceneGraph->at(i).tex_diffuse_path);								//convect string to Wchar
		HRESULT rs;
		rs = CreateDDSTextureFromFile(device, texturewstr.c_str(), nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource

		//if texture fails.  load error default
		if (rs)
		{
			CreateDDSTextureFromFile(device, L"database/data/Error.dds", nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource
		}

		//apply new texture to models effect
		newDisplayObject.m_model->UpdateEffects([&](IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{	
			auto lights = dynamic_cast<BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(newDisplayObject.m_texture_diffuse);			
			}
		});

		//set position
		newDisplayObject.m_position.x = SceneGraph->at(i).posX;
		newDisplayObject.m_position.y = SceneGraph->at(i).posY;
		newDisplayObject.m_position.z = SceneGraph->at(i).posZ;
		
		//setorientation
		newDisplayObject.m_orientation.x = SceneGraph->at(i).rotX;
		newDisplayObject.m_orientation.y = SceneGraph->at(i).rotY;
		newDisplayObject.m_orientation.z = SceneGraph->at(i).rotZ;

		//set scale
		newDisplayObject.m_scale.x = SceneGraph->at(i).scaX;
		newDisplayObject.m_scale.y = SceneGraph->at(i).scaY;
		newDisplayObject.m_scale.z = SceneGraph->at(i).scaZ;

		//set wireframe / render flags
		newDisplayObject.m_render		= SceneGraph->at(i).editor_render;
		newDisplayObject.m_wireframe	= SceneGraph->at(i).editor_wireframe;

		newDisplayObject.m_light_type		= SceneGraph->at(i).light_type;
		newDisplayObject.m_light_diffuse_r	= SceneGraph->at(i).light_diffuse_r;
		newDisplayObject.m_light_diffuse_g	= SceneGraph->at(i).light_diffuse_g;
		newDisplayObject.m_light_diffuse_b	= SceneGraph->at(i).light_diffuse_b;
		newDisplayObject.m_light_specular_r = SceneGraph->at(i).light_specular_r;
		newDisplayObject.m_light_specular_g = SceneGraph->at(i).light_specular_g;
		newDisplayObject.m_light_specular_b = SceneGraph->at(i).light_specular_b;
		newDisplayObject.m_light_spot_cutoff = SceneGraph->at(i).light_spot_cutoff;
		newDisplayObject.m_light_constant	= SceneGraph->at(i).light_constant;
		newDisplayObject.m_light_linear		= SceneGraph->at(i).light_linear;
		newDisplayObject.m_light_quadratic	= SceneGraph->at(i).light_quadratic;
		
		m_displayList.push_back(newDisplayObject);
		
	}
		
		
		
}

void Game::UpdateDisplayList(std::vector<SceneObject>* SceneGraph)
{
	auto device = m_deviceResources->GetD3DDevice();
	auto devicecontext = m_deviceResources->GetD3DDeviceContext();

	//for every item in the scenegraph
	int numObjects = m_displayList.size();
	for (int i = 0; i < numObjects; i++)
	{
		m_displayList[i];

		//set position
		m_displayList[i].m_position.x = SceneGraph->at(i).posX;
		m_displayList[i].m_position.y = SceneGraph->at(i).posY;
		m_displayList[i].m_position.z = SceneGraph->at(i).posZ;

		//setorientation
		m_displayList[i].m_orientation.x = SceneGraph->at(i).rotX;
		m_displayList[i].m_orientation.y = SceneGraph->at(i).rotY;
		m_displayList[i].m_orientation.z = SceneGraph->at(i).rotZ;

		//set scale
		m_displayList[i].m_scale.x = SceneGraph->at(i).scaX;
		m_displayList[i].m_scale.y = SceneGraph->at(i).scaY;
		m_displayList[i].m_scale.z = SceneGraph->at(i).scaZ;


	}



}



void Game::BuildDisplayChunk(ChunkObject * SceneChunk)
{
	//populate our local DISPLAYCHUNK with all the chunk info we need from the object stored in toolmain
	//which, to be honest, is almost all of it. Its mostly rendering related info so...
	m_displayChunk.PopulateChunkData(SceneChunk);		//migrate chunk data
	m_displayChunk.LoadHeightMap(m_deviceResources);
	m_displayChunk.m_terrainEffect->SetProjection(m_projection);
	m_displayChunk.InitialiseBatch();
}

void Game::SaveDisplayChunk(ChunkObject * SceneChunk)
{
	m_displayChunk.SaveHeightMap();			//save heightmap to file.
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif


#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);

    m_fxFactory = std::make_unique<EffectFactory>(device);
	m_fxFactory->SetDirectory(L"database/data/"); //fx Factory will look in the database directory
	m_fxFactory->SetSharing(false);	//we must set this to false otherwise it will share effects based on the initial tex loaded (When the model loads) rather than what we will change them to.

    m_sprites = std::make_unique<SpriteBatch>(context);

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

    m_batchEffect = std::make_unique<BasicEffect>(device);
    m_batchEffect->SetVertexColorEnabled(true);

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        DX::ThrowIfFailed(
            device->CreateInputLayout(VertexPositionColor::InputElements,
                VertexPositionColor::InputElementCount,
                shaderByteCode, byteCodeLength,
                m_batchInputLayout.ReleaseAndGetAddressOf())
        );
    }

    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");

//    m_shape = GeometricPrimitive::CreateTeapot(context, 4.f, 8);

    // SDKMESH has to use clockwise winding with right-handed coordinates, so textures are flipped in U
    m_model = Model::CreateFromSDKMESH(device, L"tiny.sdkmesh", *m_fxFactory);

	//store translation gizmo models
	m_gizmoModels[0][0] = Model::CreateFromSDKMESH(device, L"gizmo.sdkmesh", *m_fxFactory);
	m_gizmoModels[0][1] = Model::CreateFromSDKMESH(device, L"gizmoGreen.sdkmesh", *m_fxFactory);
	m_gizmoModels[0][2] = Model::CreateFromSDKMESH(device, L"gizmoBlue.sdkmesh", *m_fxFactory);

	//store rotator gizmo models
	m_gizmoModels[1][0] = Model::CreateFromSDKMESH(device, L"rotateGizmo.sdkmesh", *m_fxFactory);
	m_gizmoModels[1][1] = Model::CreateFromSDKMESH(device, L"rotateGizmo.sdkmesh", *m_fxFactory);
	m_gizmoModels[1][2] = Model::CreateFromSDKMESH(device, L"rotateGizmo.sdkmesh", *m_fxFactory);

	m_gizmoMode = 1;

    // Load textures
    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"seafloor.dds", nullptr, m_texture1.ReleaseAndGetAddressOf())
    );

    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"windowslogo.dds", nullptr, m_texture2.ReleaseAndGetAddressOf())
    );

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        1000.0f
    );

    m_batchEffect->SetProjection(m_projection);
	
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_batch.reset();
    m_batchEffect.reset();
    m_font.reset();
    m_shape.reset();
    m_model.reset();
    m_texture1.Reset();
    m_texture2.Reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}

int Game::MousePicking() {
	int selectedID = -1;
	float pickedDistance = 0;

	float minDistance = 9999999;

	//setup near and far planes with mouse x and y
	const XMVECTOR nearSource = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 0.f, 1.f);
	const XMVECTOR farSource = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 1.f, 1.f);

	//loop through displayed objects and pick with each
	for (int i = 0; i < m_displayList.size();i++) {

		const XMVECTORF32 scale = { m_displayList[i].m_scale.x,
		m_displayList[i].m_scale.y,m_displayList[i].m_scale.z };
		const XMVECTORF32 translate = { m_displayList[i].m_position.x,
		m_displayList[i].m_position.y,m_displayList[i].m_position.z };



		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(
			m_displayList[i].m_orientation.y * 3.1415 / 180,
			m_displayList[i].m_orientation.x * 3.1415 / 180,
			m_displayList[i].m_orientation.z * 3.1415 / 180
		);

		//create set matrix of selected object in the world based on translation scale and rotation
		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);

		//unproject the points on near and far plane
		XMVECTOR nearPoint = XMVector3Unproject(nearSource, 0.f, 0.f,
			m_ScreenDimensions.right, m_ScreenDimensions.bottom,
			m_deviceResources->GetScreenViewport().MinDepth,
			m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);

		XMVECTOR farPoint = XMVector3Unproject(farSource, 0.f, 0.f,
			m_ScreenDimensions.right, m_ScreenDimensions.bottom,
			m_deviceResources->GetScreenViewport().MinDepth,
			m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);

		XMVECTOR pickingVector = farPoint - nearPoint;
		pickingVector = XMVector3Normalize(pickingVector);

		for (int y = 0; y < m_displayList[i].m_model.get()->meshes.size();y++) {
			if (m_displayList[i].m_model.get()->meshes[y]->boundingBox.Intersects(nearPoint, pickingVector, pickedDistance)) {
					

				if (pickedDistance < minDistance) {
					selectedID = i;
				}


			}
		}

	}

	return selectedID;
}

int Game::MouseInteractGizmo() {

	int selectedID = -1;

	float pickedDistance = 0;

	float minDistance = 9999999;

	//setup near and far planes with mouse x and y
	const XMVECTOR nearSource = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 0.f, 1.f);
	const XMVECTOR farSource = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 1.f, 1.f);

	if (m_SelectedIndex != -1)
	{
		for (int i = 0; i < 3; i++) {

			const XMVECTORF32 scale = { 4,2,4 };
			const XMVECTORF32 translate = { m_displayList[m_SelectedIndex].m_position.x,
			m_displayList[m_SelectedIndex].m_position.y,m_displayList[m_SelectedIndex].m_position.z };


			XMVECTOR rotate;
			switch (i) {
			case 0:
				// convert degrees into radians for rotation matrix
				rotate = Quaternion::CreateFromYawPitchRoll(0 * 3.1415 / 180,
					0 * 3.1415 / 180,
					0 * 3.1415 / 180);
				break;
			case 1:
				rotate = Quaternion::CreateFromYawPitchRoll(0 * 3.1415 / 180,
					90 * 3.1415 / 180,
					0 * 3.1415 / 180);
				break;
			case 2:
				rotate = Quaternion::CreateFromYawPitchRoll(0 * 3.1415 / 180,
					90 * 3.1415 / 180,
					90 * 3.1415 / 180);
				break;
			}

			//create set matrix of selected object in the world based on translation scale and rotation
			XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);

			//unproject the points on near and far plane
			XMVECTOR nearPoint = XMVector3Unproject(nearSource, 0.f, 0.f,
				m_ScreenDimensions.right, m_ScreenDimensions.bottom,
				m_deviceResources->GetScreenViewport().MinDepth,
				m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);

			XMVECTOR farPoint = XMVector3Unproject(farSource, 0.f, 0.f,
				m_ScreenDimensions.right, m_ScreenDimensions.bottom,
				m_deviceResources->GetScreenViewport().MinDepth,
				m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);

			XMVECTOR pickingVector = farPoint - nearPoint;
			pickingVector = XMVector3Normalize(pickingVector);

			for (int y = 0; y < m_gizmoModels[m_gizmoMode][i].get()->meshes.size(); y++) {
				if (m_gizmoModels[m_gizmoMode][i].get()->meshes[y]->boundingBox.Intersects(nearPoint, pickingVector, pickedDistance)) {

					if (pickedDistance < minDistance) {
						selectedID = i;
					}

				}
			}
		}
	}

	return selectedID;
}

void Game::ScreenPointToRay(float x, float y, XMMATRIX& view, XMMATRIX& proj, float screenWidth, float screenHeight, XMVECTOR& outOrigin, XMVECTOR& outDir)
{
	XMVECTOR nearPoint = XMVector3Unproject(
	XMVectorSet(x,y,0.f,1.f),
	0.f,0.f,screenWidth,screenHeight,0.f,1.f,
	proj,view,XMMatrixIdentity()
	);
	XMVECTOR farPoint = XMVector3Unproject(
		XMVectorSet(x, y, 1.0f, 1.0f),
		0.0f, 0.0f, screenWidth, screenHeight, 0.0f, 1.0f,
		proj, view, XMMatrixIdentity()
	);

	outOrigin = nearPoint;
	outDir = XMVector3Normalize(XMVectorSubtract(farPoint, nearPoint));
}

XMVECTOR Game::DragGizmo(XMVECTOR axisDir) 
{
	XMMATRIX viewM = m_view;
	XMMATRIX projM = m_projection;

	XMVECTOR axisOrigin = XMVectorSet(m_displayList[m_SelectedIndex].m_position.x,
			m_displayList[m_SelectedIndex].m_position.y,m_displayList[m_SelectedIndex].m_position.z,1);

	XMVECTOR mouseRayOrigin, mouseRayDir;
	ScreenPointToRay(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, viewM, projM, m_ScreenDimensions.right, m_ScreenDimensions.bottom, mouseRayOrigin, mouseRayDir);

	XMVECTOR clickPoint = ClosestPointBetweenLines(axisOrigin, axisDir, mouseRayOrigin, mouseRayDir);

	return clickPoint;
}

XMVECTOR Game::DragRotGizmo(XMVECTOR rotateAxis) {

	XMMATRIX viewM = m_view;
	XMMATRIX projM = m_projection;

	XMVECTOR planeOrigin = XMVectorSet(m_displayList[m_SelectedIndex].m_position.x,
		m_displayList[m_SelectedIndex].m_position.y, m_displayList[m_SelectedIndex].m_position.z, 1);

	XMVECTOR mouseRayOrigin, mouseRayDir;
	ScreenPointToRay(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, viewM, projM, m_ScreenDimensions.right, m_ScreenDimensions.bottom, mouseRayOrigin, mouseRayDir);

	XMVECTOR clickPoint = ClosestPointOnPlane(mouseRayOrigin,mouseRayDir,planeOrigin,rotateAxis);
	return clickPoint;
}

void Game::SetSelectedIndex(int nSelected)
{
	m_SelectedIndex = nSelected;
}

void Game::SetSelectedGizmo(int nSelected)
{
	m_selectedGizmo = nSelected;
}

XMVECTOR Game::ClosestPointBetweenLines(XMVECTOR point1, XMVECTOR dir1, XMVECTOR point2, XMVECTOR dir2)
{
	//get the closes point on the line provided and the line of the axis being moved on

	XMVECTOR ray = XMVectorSubtract(point1, point2);

	//dot products needed for formula
	float a = XMVectorGetX(XMVector3Dot(dir1,dir1));
	float b = XMVectorGetX(XMVector3Dot(dir1, dir2));
	float c = XMVectorGetX(XMVector3Dot(dir2, dir2));
	float d = XMVectorGetX(XMVector3Dot(dir1, ray));
	float e = XMVectorGetX(XMVector3Dot(dir2, ray));


	float denom = a * c - b * b;

	//if denominator is close to zero just return the original point
	if (fabs(denom)< 1e-6f){
		return point1;
	}

	//find the scalar along the direction to get the point
	float t = (b * e - c * d) / denom;
	return XMVectorMultiplyAdd(dir1, XMVectorReplicate(t), point1);
}

XMVECTOR Game::ClosestPointOnPlane(XMVECTOR point1, XMVECTOR dir, XMVECTOR planeOrigin, XMVECTOR planeNormal) 
{
	float denom = XMVectorGetX(XMVector3Dot(planeNormal, dir));
	if (fabs(denom) < 1e-6f) {
		//if parallel, fallback
		return point1;
	}

	float t = XMVectorGetX(XMVector3Dot(planeOrigin - point1, planeNormal)) / denom;
	return point1 + t * dir;
}

float Game::GetAngleDiff(XMVECTOR point1, XMVECTOR point2, XMVECTOR rotAxis) 
{
	XMVECTOR planeOrigin = XMVectorSet(m_displayList[m_SelectedIndex].m_position.x,
		m_displayList[m_SelectedIndex].m_position.y, m_displayList[m_SelectedIndex].m_position.z, 1);

	XMVECTOR v1 = XMVector3Normalize(point1-planeOrigin);
	XMVECTOR v2 = XMVector3Normalize(point2 - planeOrigin);

	XMVECTOR cross = XMVector3Cross(v1, v2);
	float angle = acosf(XMVectorGetX(XMVector3Dot(v1, v2)));

	if (XMVectorGetX(XMVector3Dot(cross, rotAxis)) < 0) {
		angle = -angle;
	}
	angle = -angle;

	float radToDeg = 180.0f / XM_PI;

	angle = angle * radToDeg;

	return angle;
}


#pragma endregion

std::wstring StringToWCHART(std::string s)
{

	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

int Game::TestGizmoRingHit(const XMVECTOR& rayOrigin,const XMVECTOR& rayDirection,const XMVECTOR& ringCenter,const float ringRadius,const float segmentWidth,const float segmentHeight,const float segmentDepth,const int segmentCount,const XMVECTOR& axisNormal, const XMVECTOR& axisRight){   // e.g. (0,0,1) for Z-up ring     // vector perpendicular to axisNormal for ring layout) {



	int hitSegment = -1;
	float closestDist = FLT_MAX;

	for (int i = 0; i < segmentCount; ++i) {
		float angle = XM_2PI * i / segmentCount;

		// Compute circle position
		float x = cosf(angle);
		float y = sinf(angle);
		XMVECTOR offset = axisRight * x * ringRadius +
			XMVector3Cross(axisNormal, axisRight) * y * ringRadius;

		XMVECTOR segmentCenter = ringCenter + offset;

		// Rotation to align box tangential to the ring
		XMVECTOR tangent = XMVector3Normalize(
			axisRight * -sinf(angle) +
			XMVector3Cross(axisNormal, axisRight) * cosf(angle)
		);
		XMVECTOR up = axisNormal;
		XMVECTOR right = XMVector3Cross(up, tangent);

		XMMATRIX rotMatrix = XMMatrixSet(
			XMVectorGetX(right), XMVectorGetX(up), XMVectorGetX(tangent), 0,
			XMVectorGetY(right), XMVectorGetY(up), XMVectorGetY(tangent), 0,
			XMVectorGetZ(right), XMVectorGetZ(up), XMVectorGetZ(tangent), 0,
			0, 0, 0, 1
		);

		XMVECTOR orientation = XMQuaternionRotationMatrix(rotMatrix);

		// Create OBB
		BoundingOrientedBox box;
		box.Center = { 0, 0, 0 };
		box.Extents = XMFLOAT3(segmentWidth * 0.5f, segmentHeight * 0.5f, segmentDepth * 0.5f);
		XMStoreFloat4(&box.Orientation, orientation);

		// Transform OBB to world
		BoundingOrientedBox transformedBox;
		box.Transform(transformedBox, XMMatrixRotationQuaternion(orientation) * XMMatrixTranslationFromVector(segmentCenter));

		// Ray test
		float dist = 0.0f;
		if (transformedBox.Intersects(rayOrigin, rayDirection, dist)) {
			if (dist < closestDist) {
				closestDist = dist;
				hitSegment = i;
			}
		}
	}

	return hitSegment;
}

int Game::TestGizmoRingHitMulti(
	const float ringRadius,
	const float segmentWidth,
	const float segmentHeight,
	const float segmentDepth,
	const int segmentCount
) {
	struct RingInfo {
		int id;
		XMVECTOR normal;
		XMVECTOR right;
	};

	int closestRing = -1;

	RingInfo rings[3] = {
		{ 2, XMVectorSet(1, 0, 0, 0), XMVectorSet(0, 0, 1, 0) }, // X ring (YZ plane)
		{0, XMVectorSet(0, 1, 0, 0), XMVectorSet(1, 0, 0, 0) }, // Y ring (XZ plane)
		{ 1, XMVectorSet(0, 0, 1, 0), XMVectorSet(1, 0, 0, 0) }  // Z ring (XY plane)
	};

	if (m_SelectedIndex != -1)
	{

	XMVECTOR rayOrigin, rayDirection;

	XMMATRIX viewM = m_view;
	XMMATRIX projM = m_projection;
	ScreenPointToRay(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, viewM, projM, m_ScreenDimensions.right, m_ScreenDimensions.bottom, rayOrigin, rayDirection);




	const XMVECTORF32 ringCenter = { m_displayList[m_SelectedIndex].m_position.x,
			m_displayList[m_SelectedIndex].m_position.y,m_displayList[m_SelectedIndex].m_position.z };

	
	float closestDistance = FLT_MAX;

	for (const auto& ring : rings) {
		for (int i = 0; i < segmentCount; ++i) {
			float angle = XM_2PI * i / segmentCount;

			float x = cosf(angle);
			float y = sinf(angle);

			XMVECTOR offset = ring.right * x * ringRadius +
				XMVector3Cross(ring.normal, ring.right) * y * ringRadius;
			XMVECTOR segmentCenter = ringCenter + offset;

			// Rotation
			XMVECTOR tangent = XMVector3Normalize(
				ring.right * -sinf(angle) +
				XMVector3Cross(ring.normal, ring.right) * cosf(angle)
			);
			XMVECTOR up = ring.normal;
			XMVECTOR right = XMVector3Cross(up, tangent);

			XMMATRIX rotMatrix = XMMatrixSet(
				XMVectorGetX(right), XMVectorGetX(up), XMVectorGetX(tangent), 0,
				XMVectorGetY(right), XMVectorGetY(up), XMVectorGetY(tangent), 0,
				XMVectorGetZ(right), XMVectorGetZ(up), XMVectorGetZ(tangent), 0,
				0, 0, 0, 1
			);

			XMVECTOR orientation = XMQuaternionRotationMatrix(rotMatrix);

			BoundingOrientedBox box;
			box.Center = { 0, 0, 0 };
			box.Extents = XMFLOAT3(segmentWidth * 0.5f, segmentHeight * 0.5f, segmentDepth * 0.5f);
			XMStoreFloat4(&box.Orientation, orientation);

			BoundingOrientedBox transformedBox;
			box.Transform(transformedBox, XMMatrixRotationQuaternion(orientation) * XMMatrixTranslationFromVector(segmentCenter));

			float dist = 0.0f;
			if (transformedBox.Intersects(rayOrigin, rayDirection, dist)) {
				if (dist < closestDistance) {
					closestDistance = dist;
					closestRing = ring.id;
				}
			}
		}
	}
	}

	return closestRing; // RING_X, RING_Y, RING_Z, or RING_NONE
}


