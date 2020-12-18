#include "Emitter.h"

using namespace DirectX;

#define PARTICLE_COUNT 512
#define PARTICLE_COUNT_F 512.0f

Emitter::Emitter(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, SimpleComputeShader* initCS,
	SimpleComputeShader* updateCS, SimpleComputeShader* argumentPassCS, SimpleVertexShader* particleVS, SimplePixelShader* particlePS, const wchar_t* initPath, const wchar_t* definitionsPath)
{
	//Init
	{
		this->context = context;
		this->updateCS = updateCS;
		this->argumentPassCS = argumentPassCS;
		this->particleVS = particleVS;
		this->particleVS = particleVS;
		this->particlePS = particlePS;

		//Load the particle definitions texture 
		CreateWICTextureFromFile(device.Get(), context.Get(), definitionsPath, 0, particleDefinitionsSRV.GetAddressOf());
		D3D11_SAMPLER_DESC samplerInfo = {};
		samplerInfo.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerInfo.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerInfo.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerInfo.MinLOD = 0;
		samplerInfo.MaxLOD = D3D11_FLOAT32_MAX;
		device->CreateSamplerState(&samplerInfo, particleDefinitionsSamplerState.GetAddressOf());
	}

	// Particle index buffer
	{
		// Buffer
		D3D11_BUFFER_DESC ibDesc = {};
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.ByteWidth = sizeof(unsigned long) * PARTICLE_COUNT * 6;
		ibDesc.CPUAccessFlags = 0;
		ibDesc.MiscFlags = 0;
		ibDesc.StructureByteStride = 0;
		ibDesc.Usage = D3D11_USAGE_DEFAULT;

		// Fill it with data
		unsigned long* indices = new unsigned long[PARTICLE_COUNT * 6];
		for (unsigned long i = 0; i < PARTICLE_COUNT; i++)
		{
			unsigned long indexCounter = i * 6;
			indices[indexCounter + 0] = 0 + i * 4;
			indices[indexCounter + 1] = 1 + i * 4;
			indices[indexCounter + 2] = 2 + i * 4;
			indices[indexCounter + 3] = 0 + i * 4;
			indices[indexCounter + 4] = 2 + i * 4;
			indices[indexCounter + 5] = 3 + i * 4;
		}

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = indices;
		device->CreateBuffer(&ibDesc, &data, &indexBuffer);

		delete[] indices;
	}

	// Particles
	{
		// Buffer
		ID3D11Buffer* particlePoolBuffer;
		D3D11_BUFFER_DESC poolDesc = {};
		poolDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		poolDesc.ByteWidth = sizeof(Particle) * PARTICLE_COUNT;
		poolDesc.CPUAccessFlags = 0;
		poolDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		poolDesc.StructureByteStride = sizeof(Particle);
		poolDesc.Usage = D3D11_USAGE_DEFAULT;
		device->CreateBuffer(&poolDesc, 0, &particlePoolBuffer);

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC poolUAVDesc = {};
		poolUAVDesc.Format = DXGI_FORMAT_UNKNOWN; // Needed for RW structured buffers
		poolUAVDesc.Buffer.FirstElement = 0;
		poolUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
		poolUAVDesc.Buffer.NumElements = PARTICLE_COUNT;
		poolUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		device->CreateUnorderedAccessView(particlePoolBuffer, &poolUAVDesc, particlesUAV.GetAddressOf());

		// SRV (for indexing in VS)
		D3D11_SHADER_RESOURCE_VIEW_DESC poolSRVDesc = {};
		poolSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		poolSRVDesc.Buffer.FirstElement = 0;
		poolSRVDesc.Buffer.NumElements = PARTICLE_COUNT;
		poolSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		device->CreateShaderResourceView(particlePoolBuffer, &poolSRVDesc, particlesSRV.GetAddressOf());

		// Done
		particlePoolBuffer->Release();
	}

	//Draw Args
	{
		// Buffer
		D3D11_BUFFER_DESC argsDesc = {};
		argsDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		argsDesc.ByteWidth = sizeof(unsigned int) * 5; // Need 5 if using an index buffer!
		argsDesc.CPUAccessFlags = 0;
		argsDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		argsDesc.StructureByteStride = 0;
		argsDesc.Usage = D3D11_USAGE_DEFAULT;
		device->CreateBuffer(&argsDesc, 0, drawArgsBuffer.GetAddressOf());

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC argsUAVDesc = {};
		argsUAVDesc.Format = DXGI_FORMAT_R32_UINT; // Actually UINT's in here!
		argsUAVDesc.Buffer.FirstElement = 0;
		argsUAVDesc.Buffer.Flags = 0;  // Nothing special
		argsUAVDesc.Buffer.NumElements = 5; // Need 5 if using an index buffer
		argsUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		device->CreateUnorderedAccessView(drawArgsBuffer.Get(), &argsUAVDesc, &drawArgsUAV);
	}

	//Initialize using specialized shader
	{
		//Load neccessary init texture
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> initSRV;
		CreateWICTextureFromFile(device.Get(), context.Get(), initPath, 0, initSRV.GetAddressOf());

		//Create init sampler
		Microsoft::WRL::ComPtr<ID3D11SamplerState> initSamplerState;

		D3D11_SAMPLER_DESC samplerInfo = {};
		samplerInfo.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerInfo.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerInfo.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerInfo.MinLOD = 0;
		samplerInfo.MaxLOD = D3D11_FLOAT32_MAX;
		device->CreateSamplerState(&samplerInfo, initSamplerState.GetAddressOf());

		// Launch the init shader
		initCS->SetShaderResourceView("smokeAgeLookup", initSRV.Get());
		initCS->SetSamplerState("smokeAgeSampler", initSamplerState.Get());
		initCS->SetUnorderedAccessView("Particles", particlesUAV.Get());
		initCS->SetShader();
		initCS->SetFloat("TotalParticles", PARTICLE_COUNT_F);
		initCS->CopyAllBufferData();
		initCS->DispatchByThreads(PARTICLE_COUNT, 1, 1);

		D3D11_DEPTH_STENCIL_DESC depth = {};
		depth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth.DepthFunc = D3D11_COMPARISON_LESS;
		depth.DepthEnable = true;
		device->CreateDepthStencilState(&depth, &depthStencil);
	}
}

Emitter::~Emitter()
{

}

void Emitter::Update(float dt)
{
	// Reset UAVs
	ID3D11UnorderedAccessView* none[8] = {};
	context->CSSetUnorderedAccessViews(0, 8, none, 0);

	updateCS->SetShader();
	updateCS->SetFloat("TimeStep", dt);
	updateCS->SetFloat("TotalParticles", PARTICLE_COUNT_F);
	updateCS->SetUnorderedAccessView("Particles", particlesUAV.Get());
	updateCS->SetShaderResourceView("smokeDefinitions", particleDefinitionsSRV.Get());
	updateCS->SetSamplerState("smokeDefinitionsSampler", particleDefinitionsSamplerState.Get());

	updateCS->CopyAllBufferData();
	updateCS->DispatchByThreads(PARTICLE_COUNT, 1, 1);

	context->CSSetUnorderedAccessViews(0, 8, none, 0);

	// Get draw data
	argumentPassCS->SetShader();
	argumentPassCS->SetInt("VertsPerParticle", 6);
	argumentPassCS->SetInt("TotalParticles", PARTICLE_COUNT);
	argumentPassCS->CopyAllBufferData();
	argumentPassCS->SetUnorderedAccessView("DrawArgs", drawArgsUAV.Get());
	argumentPassCS->SetUnorderedAccessView("Particles", particlesUAV.Get()); // Don't reset counter!!!
	argumentPassCS->DispatchByThreads(1, 1, 1);

	context->CSSetUnorderedAccessViews(0, 8, none, 0);
}

void Emitter::Draw(DirectX::XMFLOAT4X4* view, DirectX::XMFLOAT4X4* proj, Light* light, Microsoft::WRL::ComPtr<ID3D11SamplerState> clampSampler, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cellRamp)
{
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->VSSetShaderResources(0, 1, particlesSRV.GetAddressOf());

	particleVS->SetShader();
	particleVS->SetMatrix4x4("world", XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
	particleVS->SetMatrix4x4("view", *view);
	particleVS->SetMatrix4x4("projection", *proj);
	particleVS->CopyAllBufferData();

	particlePS->SetShader();

	// Draw using indirect args
	context->DrawIndexedInstancedIndirect(drawArgsBuffer.Get(), 0);

	ID3D11ShaderResourceView* none[16] = {};
	context->VSSetShaderResources(0, 16, none);

	context->OMSetDepthStencilState(0, 0);
}
