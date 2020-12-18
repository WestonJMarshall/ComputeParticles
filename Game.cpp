#include "Game.h"
#include "Vertex.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

#define MAX_LIGHT_COUNT 16

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	camera = new Camera(DirectX::XMFLOAT3(0, 0, -5), DirectX::XM_PIDIV2, (float)this->width / this->height, 0.05f, 1000, 1, 2.5f);
	skyBox = nullptr;
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Note: Since we're using smart pointers (ComPtr),
	// we don't need to explicitly clean up those DirectX objects
	// - If we weren't using smart pointers, we'd need
	//   to call Release() on each DirectX object created in Game
	delete skyBox;

	for (int i = 0; i < meshes.size(); i++)
	{
		delete meshes[i];
	}
	for (int i = 0; i < materials.size(); i++)
	{
		delete materials[i];
	}
	for (int i = 0; i < entities.size(); i++)
	{
		delete entities[i];
	}

	delete camera;

	delete cellVS;
	delete cellPS;

	delete cellShadePS;
	delete postProcessVS;

	delete smokeEmitter;
	delete smokeUpdateCS;
	delete smokeArgumentPassCS;
	delete smokeInitCS;
	delete smokeVS;
	delete smokePS;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	LoadShaders();

	#pragma region Directional Lights
	//Setup directional lights
	Light dlA = Light();

	dlA.ambientColor = DirectX::XMFLOAT4(0.01f, 0.01f, 0.1f, 1.0f);
	dlA.diffuseColor = DirectX::XMFLOAT4(0.1f, 0.1f, 1.0f, 1.0f);
	dlA.direction = DirectX::XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f);
	dlA.position = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	dlA.valid = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	Light dlB = Light();

	dlB.ambientColor = DirectX::XMFLOAT4(0.01f, 0.01f, 0.1f, 1.0f);
	dlB.diffuseColor = DirectX::XMFLOAT4(1.0f, 0.1f, 0.1f, 1.0f);
	dlB.direction = DirectX::XMFLOAT4(1.0f, -0.1f, 0.0f, 1.0f);
	dlB.position = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	dlB.valid = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	Light dlC = Light();

	dlC.ambientColor = DirectX::XMFLOAT4(0.01f, 0.01f, 0.1f, 1.0f);
	dlC.diffuseColor = DirectX::XMFLOAT4(0.1f, 1.0f, 0.1f, 1.0f);
	dlC.direction = DirectX::XMFLOAT4(0.1f, -1.0f, 0.0f, 1.0f);
	dlC.position = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f,1.0f);
	dlC.valid = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	directionalLights.push_back(dlA);
	//directionalLights.push_back(dlB);
	//directionalLights.push_back(dlC);

	for (size_t i = directionalLights.size(); i < MAX_LIGHT_COUNT; i++)
	{
		Light dl = Light();
		dl.valid = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		directionalLights.push_back(dl);
	}
	#pragma endregion

	#pragma region Point Lights
	//Setup point lights
	Light plA = Light();

	plA.ambientColor = DirectX::XMFLOAT4(0.01f, 0.01f, 0.1f, 1.0f);
	plA.diffuseColor = DirectX::XMFLOAT4(0.1f, 0.1f, 1.0f, 1.0f);
	plA.direction = DirectX::XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f);
	plA.position = DirectX::XMFLOAT4(2.0f, 2.0f, 2.0f, 1.0f);
	plA.valid = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	Light plB = Light();

	plB.ambientColor = DirectX::XMFLOAT4(0.01f, 0.01f, 0.1f, 1.0f);
	plB.diffuseColor = DirectX::XMFLOAT4(1.0f, 0.1f, 0.1f, 1.0f);
	plB.direction = DirectX::XMFLOAT4(1.0f, -0.1f, 0.0f, 1.0f);
	plB.position = DirectX::XMFLOAT4(0.0f, -5.0f, 0.0f, 1.0f);
	plB.valid = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	Light plC = Light();

	plC.ambientColor = DirectX::XMFLOAT4(0.01f, 0.01f, 0.1f, 1.0f);
	plC.diffuseColor = DirectX::XMFLOAT4(0.1f, 1.0f, 0.1f, 1.0f);
	plC.direction = DirectX::XMFLOAT4(0.1f, -1.0f, 0.0f, 1.0f);
	plC.position = DirectX::XMFLOAT4(0.0f, 5.0f, 0.0f, 1.0f);
	plC.valid = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//pointLights.push_back(plA);
	//pointLights.push_back(plB);
	pointLights.push_back(plC);

	for (size_t i = pointLights.size(); i < MAX_LIGHT_COUNT; i++)
	{
		Light pl = Light();
		pl.valid = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		pointLights.push_back(pl);
	}
	#pragma endregion

	CreateInitialMeshes();
	CreateInitialMaterials();
	CreateInitialEntities();

	skyBox = new SkyBox(meshes[1],gameSamplerState,device,context, GetFullPathTo_Wide(L"../../Assets/Textures/Skies/SpaceCubeMap2.dds").c_str(), 
		GetFullPathTo_Wide(L"SkyPS.cso").c_str(), GetFullPathTo_Wide(L"SkyVS.cso").c_str());

	// Set up particles
	smokeEmitter = new Emitter(
		device,
		context,
		smokeInitCS,
		smokeUpdateCS,
		smokeArgumentPassCS,
		smokeVS, 
		smokePS,
		GetFullPathTo_Wide(L"../../Assets/Textures/smoke/init.png").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Textures/smoke/definitions.png").c_str()); 
	//OTHER TEXTURES:
	// ../../Assets/Textures/smoke/definitionsWeird.png
	// ../../Assets/Textures/smoke/definitionsWeird2.png

	ResizePostProcessResources();
}

void Game::ResizePostProcessResources()
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = (float)this->width;
	textureDesc.Height = (float)this->height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	// Create the color and normals textures
	ID3D11Texture2D* postProcessTexture;

	device->CreateTexture2D(&textureDesc, 0, &postProcessTexture);

	// Adjust the description for scene normals
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	ID3D11Texture2D* sceneNormalsTexture;
	device->CreateTexture2D(&textureDesc, 0, &sceneNormalsTexture);

	// Adjust the description for the scene depths
	textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	ID3D11Texture2D* sceneDepthsTexture;
	device->CreateTexture2D(&textureDesc, 0, &sceneDepthsTexture);

	// Create the Render Target Views
	device->CreateRenderTargetView(postProcessTexture, 0, postProcessRTV.ReleaseAndGetAddressOf());
	device->CreateRenderTargetView(sceneNormalsTexture, 0, sceneNormalsRTV.ReleaseAndGetAddressOf());
	device->CreateRenderTargetView(sceneDepthsTexture, 0, sceneDepthRTV.ReleaseAndGetAddressOf());

	// Create the Shader Resource Views
	device->CreateShaderResourceView(postProcessTexture, 0, postProcessSRV.ReleaseAndGetAddressOf());
	device->CreateShaderResourceView(sceneNormalsTexture, 0, sceneNormalsSRV.ReleaseAndGetAddressOf());
	device->CreateShaderResourceView(sceneDepthsTexture, 0, sceneDepthSRV.ReleaseAndGetAddressOf());

	// Release the extra texture references
	postProcessTexture->Release();
	sceneNormalsTexture->Release();
	sceneDepthsTexture->Release();
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	cellVS = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"CellVS.cso").c_str());
	cellPS = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"CellPS.cso").c_str());

	postProcessVS = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PostProcessVS.cso").c_str());
	cellShadePS = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"CellShadePS.cso").c_str());

	smokeUpdateCS = new SimpleComputeShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"SmokeParticleCS.cso").c_str());
	smokeArgumentPassCS = new SimpleComputeShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"SmokeArgumentPassCS.cso").c_str());
	smokeInitCS = new SimpleComputeShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"SmokeParticleInitCS.cso").c_str());
	
	smokeVS = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"SmokeVS.cso").c_str());
	smokePS = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"SmokePS.cso").c_str());
}

void Game::CreateInitialMeshes()
{
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/sphere.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/helix.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/cylinder.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/cone.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/torus.obj").c_str(), device));
}

void Game::CreateInitialEntities()
{
	Entity* floorEntity = new Entity(meshes[1], materials[0]);
	floorEntity->GetTransform()->SetScale(5, 5, 5);
	Entity* spawnEntity = new Entity(meshes[0], materials[1]);
	spawnEntity->GetTransform()->SetScale(2, 2, 2);

	floorEntity->GetTransform()->SetPosition(3.35f, -2.5f, 3.35f);
	spawnEntity->GetTransform()->SetPosition(3.35f, 0.75f, 3.35f);

	entities.push_back(floorEntity);
	entities.push_back(spawnEntity);
}

void Game::CreateInitialMaterials()
{
	//Create game-wide sampler
	D3D11_SAMPLER_DESC samplerInfo = {};
	samplerInfo.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerInfo.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerInfo.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerInfo.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerInfo.MaxAnisotropy = 16;
	samplerInfo.MinLOD = 0;
	samplerInfo.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerInfo, gameSamplerState.GetAddressOf());

	samplerInfo.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerInfo.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerInfo.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device->CreateSamplerState(&samplerInfo, clampSampler.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/pbr/scratched_albedo.png").c_str(), 0, gameTextureSRVA.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/pbr/scratched_normals.png").c_str(), 0, gameNormalSRVA.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/pbr/scratched_metal.png").c_str(), 0, gameMetalSRVA.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/pbr/scratched_roughness.png").c_str(), 0, gameRoughSRVA.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/pbr/bronze_albedo.png").c_str(), 0, gameTextureSRVB.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/pbr/bronze_normals.png").c_str(), 0, gameNormalSRVB.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/pbr/bronze_metal.png").c_str(), 0, gameMetalSRVB.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/pbr/bronze_roughness.png").c_str(), 0, gameRoughSRVB.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/smoke/cellRampA.png").c_str(), 0, cellRampA.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/smoke/cellRampB.png").c_str(), 0, cellRampB.GetAddressOf());

	Material* floorMaterial = new Material(cellPS, cellVS, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), gameTextureSRVA, gameNormalSRVA, gameMetalSRVA, gameRoughSRVA, gameSamplerState);
	Material* wallMaterial = new Material(cellPS, cellVS, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), gameTextureSRVB, gameNormalSRVB, gameMetalSRVB, gameRoughSRVB, gameSamplerState);

	materials.push_back(floorMaterial);
	materials.push_back(wallMaterial);
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	if (camera != nullptr)
	{
		camera->UpdateProjectionMatrix((float)this->width / this->height, 90, 0.05f, 1000);
	}

	ResizePostProcessResources();
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	smokeEmitter->Update(deltaTime);

	camera->Update(deltaTime, hWnd);

	entities[0]->GetTransform()->RotateYPR(0, (float)deltaTime * -0.30f, 0);
	entities[1]->GetTransform()->RotateYPR(0, (float)deltaTime * 0.30f, 0);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	//Pre Process
	{
		const float color[4] = { 0.65f, 0.75f, 0.65f, 0.0f };

		context->ClearRenderTargetView(backBufferRTV.Get(), color);
		context->ClearDepthStencilView(
			depthStencilView.Get(),
			D3D11_CLEAR_DEPTH,
			1.0f,
			0);

		// Ensure we're clearing all of the render targets
		context->ClearRenderTargetView(postProcessRTV.Get(), color);
		context->ClearRenderTargetView(sceneNormalsRTV.Get(), color);
		context->ClearRenderTargetView(sceneDepthRTV.Get(), color);

		ID3D11RenderTargetView* rtvs[3] =
		{
			postProcessRTV.Get(),
			sceneNormalsRTV.Get(),
			sceneDepthRTV.Get()
		};
		context->OMSetRenderTargets(3, rtvs, depthStencilView.Get());
	}

	// Draw particles
	smokeEmitter->Draw(camera->GetViewMatrix(), camera->GetProjectionMatrix(), &directionalLights[0], clampSampler, cellRampA);

	XMFLOAT3* cameraPos = camera->GetTransform().GetPosition();

	cellPS->SetData("cameraPosition", cameraPos, sizeof(DirectX::XMFLOAT3));
	cellPS->SetData("directionalLights", &directionalLights[0], sizeof(Light) * MAX_LIGHT_COUNT);
	cellPS->SetData("pointLights", &pointLights[0], sizeof(Light) * MAX_LIGHT_COUNT);
	cellPS->CopyAllBufferData();

	cameraPos = nullptr;
	delete cameraPos;

	//Draw entities
	for (int i = 0; i < entities.size(); i++)
	{
		entities[i]->Draw(context,camera->GetViewMatrix(), camera->GetProjectionMatrix(), clampSampler, cellRampA);
	}

	skyBox->Draw(context, camera->GetViewMatrix(), camera->GetProjectionMatrix());

	//Post Process
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		ID3D11Buffer* nothing = 0;

		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), 0);

		postProcessVS->SetShader();

		cellShadePS->SetShaderResourceView("pixels", postProcessSRV.Get());
		cellShadePS->SetShaderResourceView("normals", sceneNormalsSRV.Get());
		cellShadePS->SetShaderResourceView("depth", sceneDepthSRV.Get());
		cellShadePS->SetSamplerState("samplerOptions", clampSampler.Get());
		cellShadePS->SetShader();

		cellShadePS->SetFloat("pixelWidth", 1.0f / width);
		cellShadePS->SetFloat("pixelHeight", 1.0f / height);
		cellShadePS->SetFloat("depthAdjust", 5.0f);
		cellShadePS->SetFloat("normalAdjust", 5.0f);
		cellShadePS->CopyAllBufferData();

		context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
		context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);

		context->Draw(3, 0);

		ID3D11ShaderResourceView* nullSRVs[16] = {};
		context->PSSetShaderResources(0, 16, nullSRVs);
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}