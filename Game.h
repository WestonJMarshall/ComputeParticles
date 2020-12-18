#pragma once

#include "DXCore.h"
#include "Lights.h"
#include "Emitter.h"
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "SkyBox.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include <wrl/client.h> 
#include<WICTextureLoader.h>
#include <vector>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void ResizePostProcessResources();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders();

	void CreateInitialMeshes();
	void CreateInitialEntities();
	void CreateInitialMaterials();

	Camera* camera;

	SkyBox* skyBox;

	std::vector<Light> directionalLights;
	std::vector<Light> pointLights;

	std::vector<Mesh*> meshes;
	std::vector<Entity*> entities;
	std::vector<Material*> materials;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> postProcessRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> postProcessSRV;	

	Microsoft::WRL::ComPtr<ID3D11SamplerState> clampSampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cellRampA;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cellRampB;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> sceneDepthRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sceneDepthSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> sceneNormalsRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sceneNormalsSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gameTextureSRVA;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gameTextureSRVB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gameNormalSRVA;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gameNormalSRVB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gameRoughSRVA;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gameRoughSRVB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gameMetalSRVA;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gameMetalSRVB;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> gameSamplerState;

	SimpleVertexShader* cellVS;
	SimplePixelShader* cellPS;
	SimpleVertexShader* postProcessVS;
	SimplePixelShader* cellShadePS;

	SimpleVertexShader* smokeVS;
	SimplePixelShader* smokePS;
	SimpleComputeShader* smokeUpdateCS;
	SimpleComputeShader* smokeArgumentPassCS;
	SimpleComputeShader* smokeInitCS;
	Emitter* smokeEmitter;
};

