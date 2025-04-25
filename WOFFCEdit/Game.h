//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "SceneObject.h"
#include "DisplayObject.h"
#include "DisplayChunk.h"
#include "ChunkObject.h"
#include "InputCommands.h"
#include <vector>


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:

	Game();
	~Game();

	// Initialization and management
	void Initialize(HWND window, int width, int height);
	void SetGridState(bool state);

	// Basic game loop
	void Tick(InputCommands * Input);
	void Render();

	// Rendering helpers
	void Clear();

	// IDeviceNotify
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowSizeChanged(int width, int height);

	//tool specific
	void BuildDisplayList(std::vector<SceneObject> * SceneGraph); //note vector passed by reference 
	void UpdateDisplayList(std::vector<SceneObject>* SceneGraph);
	void BuildDisplayChunk(ChunkObject *SceneChunk);
	void SaveDisplayChunk(ChunkObject *SceneChunk);	//saves geometry et al
	void ClearDisplayList();

	int MousePicking();

	int MouseInteractGizmo();

	void SetSelectedIndex(int nSelected);

	void SetSelectedGizmo(int nSelected);

	DirectX::XMVECTOR ClosestPointBetweenLines(DirectX::XMVECTOR point1, DirectX::XMVECTOR dir1, DirectX::XMVECTOR point2, DirectX::XMVECTOR dir2);
	DirectX::XMVECTOR ClosestPointOnPlane(DirectX::XMVECTOR point1, DirectX::XMVECTOR dir, DirectX::XMVECTOR planeOrigin, DirectX::XMVECTOR planeNormal);

	float GetAngleDiff(DirectX::XMVECTOR point1, DirectX::XMVECTOR point2, DirectX::XMVECTOR rotAxis);

	void ScreenPointToRay(float x, float y, DirectX::XMMATRIX& view, DirectX::XMMATRIX& proj, float screenWidth, float screenHeight, DirectX::XMVECTOR& outOrigin, DirectX::XMVECTOR& outDir);

	DirectX::XMVECTOR DragGizmo(DirectX::XMVECTOR axisDir);
	DirectX::XMVECTOR DragRotGizmo(DirectX::XMVECTOR rotateAxis);

	int TestGizmoRingHit(
		const DirectX::XMVECTOR& rayOrigin,
		const DirectX::XMVECTOR& rayDirection,
		const DirectX::XMVECTOR& ringCenter,
		const float ringRadius,
		const float segmentWidth,
		const float segmentHeight,
		const float segmentDepth,
		const int segmentCount,
		const DirectX::XMVECTOR& axisNormal,   // e.g. (0,0,1) for Z-up ring
		const DirectX::XMVECTOR& axisRight     // vector perpendicular to axisNormal for ring layout
	);

	int TestGizmoRingHitMulti(
		const float ringRadius,
		const float segmentWidth,
		const float segmentHeight,
		const float segmentDepth,
		const int segmentCount
	);

	int m_SelectedIndex;

	RECT m_ScreenDimensions;

#ifdef DXTK_AUDIO
	void NewAudioDevice();
#endif

private:

	void Update(DX::StepTimer const& timer);

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	void XM_CALLCONV DrawGrid(DirectX::FXMVECTOR xAxis, DirectX::FXMVECTOR yAxis, DirectX::FXMVECTOR origin, size_t xdivs, size_t ydivs, DirectX::GXMVECTOR color);

	//tool specific
	std::vector<DisplayObject>			m_displayList;
	DisplayChunk						m_displayChunk;
	InputCommands						m_InputCommands;

	//functionality
	float								m_movespeed;

	//mouse state
	DirectX::Mouse::State				mouseState;
	DirectX::SimpleMath::Vector2		mousePos;

	//camera
	DirectX::SimpleMath::Vector3		m_camPosition;
	DirectX::SimpleMath::Vector3		m_camOrientation;
	DirectX::SimpleMath::Vector3		m_camLookAt;
	DirectX::SimpleMath::Vector3		m_camLookDirection;
	DirectX::SimpleMath::Vector3		m_camRight;
	float m_camRotRate;

	//control variables
	bool m_grid;							//grid rendering on / off
	// Device resources.
    std::shared_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    // Input devices.
    std::unique_ptr<DirectX::GamePad>       m_gamePad;
    std::unique_ptr<DirectX::Keyboard>      m_keyboard;
    std::unique_ptr<DirectX::Mouse>         m_mouse;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::GeometricPrimitive>                            m_shape;
    std::unique_ptr<DirectX::Model>                                         m_model;
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;

#ifdef DXTK_AUDIO
    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
    std::unique_ptr<DirectX::WaveBank>                                      m_waveBank;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;
#endif

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture2;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  gizRed;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  gizGreen;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  gizBlue;


	std::shared_ptr<DirectX::Model> m_gizmoModels[3][3];


	int m_gizmoMode;
	int m_selectedGizmo;

#ifdef DXTK_AUDIO
    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;

    bool                                                                    m_retryDefault;
#endif

    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_projection;

};

std::wstring StringToWCHART(std::string s);