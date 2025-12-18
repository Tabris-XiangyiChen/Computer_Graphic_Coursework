#pragma once
#include <unordered_map>
#include <string>
#include "core.h"

//Blending
/*
Blending
– Combining the color of a pixel being drawn
(the source) with the color already present in
the render target (the destination)
– Uses: transparency and color mixing
– Can blend colour, alpha, or both
– Range of blend operations

struct
	D3D12_RENDER_TARGET_BLEND_DESC
{
	BOOL BlendEnable;
	BOOL LogicOpEnable;
	D3D12_BLEND SrcBlend;
	D3D12_BLEND DestBlend;
	D3D12_BLEND_OP BlendOp;
	D3D12_BLEND SrcBlendAlpha;
	D3D12_BLEND DestBlendAlpha;
	D3D12_BLEND_OP BlendOpAlpha;
	D3D12_LOGIC_OP LogicOp;
	UINT8 RenderTargetWriteMask;
};

typedef
enum D3D12_BLEND_OP
{
	D3D12_BLEND_OP_ADD = 1,
	D3D12_BLEND_OP_SUBTRACT = 2,
	D3D12_BLEND_OP_REV_SUBTRACT = 3,
	D3D12_BLEND_OP_MIN = 4,
	D3D12_BLEND_OP_MAX = 5
};

typedef
enum D3D12_BLEND {
	D3D12_BLEND_ZERO = 1,
	D3D12_BLEND_ONE = 2,
	D3D12_BLEND_SRC_COLOR = 3,
	D3D12_BLEND_INV_SRC_COLOR = 4,
	D3D12_BLEND_SRC_ALPHA = 5,
	D3D12_BLEND_INV_SRC_ALPHA = 6,
	D3D12_BLEND_DEST_ALPHA = 7,
	D3D12_BLEND_INV_DEST_ALPHA = 8,
	D3D12_BLEND_DEST_COLOR = 9,
	D3D12_BLEND_INV_DEST_COLOR = 10,
	D3D12_BLEND_SRC_ALPHA_SAT = 11,
	D3D12_BLEND_BLEND_FACTOR = 14,
	D3D12_BLEND_INV_BLEND_FACTOR = 15,
	D3D12_BLEND_SRC1_COLOR = 16,
	D3D12_BLEND_INV_SRC1_COLOR = 17,
	D3D12_BLEND_SRC1_ALPHA = 18,
	D3D12_BLEND_INV_SRC1_ALPHA = 19,
	D3D12_BLEND_ALPHA_FACTOR = 20,
	D3D12_BLEND_INV_ALPHA_FACTOR = 21
};*/


//Pipeline State Objects
/*
Unique PSO:
• Each shader combination + layout
• Rasteriser + Blend + Depth/Stencil State
– Will keep these constant for the moment
– Update the code if needed
*/

/*
PROGRAM START
│
├── Core::init()
│     ├── Create Device
│     ├── Create Command Queue
│     ├── Create Swapchain
│     ├── Create RTV/DSV Heaps
│     ├── Create Fence
│     └── ⭐ Create Root Signature (only once)
│
├── Renderer::init()
│     ├── Compile shaders
│     ├── Create PSOs (with core->rootSignature)
│     ├── Create primitives
│     ├── Create constant buffers
│     └── Create RenderItems
│
└── GAME LOOP
      ├── beginRenderPass()
      ├── RenderItem.draw()
      └── present()
*/
//PSO is simply a status packet that uses the root signature as a "binding rule".
class PSOManager
{
public:
	std::unordered_map<std::string, ID3D12PipelineState*> psos;

	//Pipeline State Objects
	//– Call after creating shaders and layout
	void createPSO(Core* core, std::string name, ID3DBlob* vs, ID3DBlob* ps, D3D12_INPUT_LAYOUT_DESC layout)
	{
		//– Check if PSO in map
		//• Avoid creating extra state

		if (psos.find(name) != psos.end())
		{
			return;
		}

		//Configure GPU pipeline with shaders, layout and Root Signature
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = layout;
		//
		desc.pRootSignature = core->rootSignature;
		desc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
		desc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };

		//Rasterizer State
		//– Responsible for configuring the rasterizer
		D3D12_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
		rasterDesc.FrontCounterClockwise = FALSE;
		rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterDesc.DepthClipEnable = TRUE;
		rasterDesc.MultisampleEnable = FALSE;
		rasterDesc.AntialiasedLineEnable = FALSE;
		rasterDesc.ForcedSampleCount = 0;
		rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		desc.RasterizerState = rasterDesc;

		//Depth Stencil State
		//– Responsible for configuring the depth buffer
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		depthStencilDesc.StencilEnable = FALSE;
		desc.DepthStencilState = depthStencilDesc;

		//Blend State
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlend = {
			FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		{
			blendDesc.RenderTarget[i] = defaultRenderTargetBlend;
		}
		desc.BlendState = blendDesc;

		//Render Target State + Topology
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count = 1;

		//desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		//desc.NodeMask = 0;

		//Create Pipeline State Object
		ID3D12PipelineState* pso;
		HRESULT hr = core->device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));
		
		if (FAILED(hr)) {

		}

		//Insert into map
		psos.insert({ name, pso });

	}

	void createPSO_Line(Core* core, std::string name, ID3DBlob* vs, ID3DBlob* ps, D3D12_INPUT_LAYOUT_DESC layout)
	{
		//– Check if PSO in map
		//• Avoid creating extra state

		if (psos.find(name) != psos.end())
		{
			return;
		}

		//Configure GPU pipeline with shaders, layout and Root Signature
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = layout;
		//
		desc.pRootSignature = core->rootSignature;
		desc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
		desc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };

		//Rasterizer State
		//– Responsible for configuring the rasterizer
		D3D12_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
		rasterDesc.FrontCounterClockwise = FALSE;
		rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterDesc.DepthClipEnable = TRUE;
		rasterDesc.MultisampleEnable = FALSE;
		rasterDesc.AntialiasedLineEnable = FALSE;
		rasterDesc.ForcedSampleCount = 0;
		rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		desc.RasterizerState = rasterDesc;

		//Depth Stencil State
		//– Responsible for configuring the depth buffer
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		depthStencilDesc.StencilEnable = FALSE;
		desc.DepthStencilState = depthStencilDesc;

		//Blend State
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlend = {
			FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		{
			blendDesc.RenderTarget[i] = defaultRenderTargetBlend;
		}
		desc.BlendState = blendDesc;

		//Render Target State + Topology
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count = 1;

		//desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		//desc.NodeMask = 0;

		//Create Pipeline State Object
		ID3D12PipelineState* pso;
		HRESULT hr = core->device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));

		if (FAILED(hr)) {

		}

		//Insert into map
		psos.insert({ name, pso });

	}
	//Add functionality to bind based on name
	void bind(Core* core, std::string name) {
		core->getCommandList()->SetPipelineState(psos[name]);
	}

	void createPSO_UI(Core* core, std::string name, ID3DBlob* vs, ID3DBlob* ps, D3D12_INPUT_LAYOUT_DESC layout)
	{
		//– Check if PSO in map
		//• Avoid creating extra state

		if (psos.find(name) != psos.end())
		{
			return;
		}

		//Configure GPU pipeline with shaders, layout and Root Signature
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = layout;
		//
		desc.pRootSignature = core->rootSignature;
		desc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
		desc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };

		D3D12_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
		rasterDesc.FrontCounterClockwise = FALSE;
		rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterDesc.DepthClipEnable = TRUE;
		rasterDesc.MultisampleEnable = FALSE;
		rasterDesc.AntialiasedLineEnable = FALSE;
		rasterDesc.ForcedSampleCount = 0;
		rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		desc.RasterizerState = rasterDesc;

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		depthStencilDesc.StencilEnable = FALSE;
		desc.DepthStencilState = depthStencilDesc;

		//Blend State
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlend = {
			FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		{
			blendDesc.RenderTarget[i] = defaultRenderTargetBlend;
		}
		desc.BlendState = blendDesc;

		//Render Target State + Topology
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count = 1;

		//desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		//desc.NodeMask = 0;

		//Create Pipeline State Object
		ID3D12PipelineState* pso;
		HRESULT hr = core->device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));

		if (FAILED(hr)) {

		}

		//Insert into map
		psos.insert({ name, pso });

	}
};