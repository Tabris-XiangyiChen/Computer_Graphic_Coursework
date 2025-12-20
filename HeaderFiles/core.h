#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <vector>
#include <string>
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler.lib")


//make a barrier helper to create barriers
class Barrier {
public:
	static void add(ID3D12Resource* res, D3D12_RESOURCE_STATES first, D3D12_RESOURCE_STATES second,
		ID3D12GraphicsCommandList4* commandList) {
		D3D12_RESOURCE_BARRIER rb = {};
		rb.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		rb.Transition.pResource = res;
		rb.Transition.StateBefore = first;
		rb.Transition.StateAfter = second;
		rb.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &rb);
	}
};

class GPUFence {
public:
	ID3D12Fence* fence;
	HANDLE eventHandle;
	UINT64 value = 0;
	void create(ID3D12Device5* device) {
		device->CreateFence(value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		eventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	void signal(ID3D12CommandQueue* queue) {
		queue->Signal(fence, ++value);
	}
	void wait() {
		if (fence->GetCompletedValue() < value) {
			fence->SetEventOnCompletion(value, eventHandle);
			WaitForSingleObject(eventHandle, INFINITE);
		}
	}
	~GPUFence() {
		CloseHandle(eventHandle);
		fence->Release();
	}
};

class DescriptorHeap
{
public:

	ID3D12DescriptorHeap* heap;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	unsigned int incrementSize;
	int used;

	void init(ID3D12Device5* device, int num)
	{
		D3D12_DESCRIPTOR_HEAP_DESC uavcbvHeapDesc = {};
		uavcbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		uavcbvHeapDesc.NumDescriptors = num;
		uavcbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		device->CreateDescriptorHeap(&uavcbvHeapDesc, IID_PPV_ARGS(&heap));
		cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
		gpuHandle = heap->GetGPUDescriptorHandleForHeapStart();
		incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		used = 0;
	}

	//Get and increment index into heap
	D3D12_CPU_DESCRIPTOR_HANDLE getNextCPUHandle()
	{
		if (used > 0)
		{
			cpuHandle.ptr += incrementSize;
		}
		used++;
		return cpuHandle;
	}

};

class Core
{
public:
	IDXGIAdapter1* adapter;
	//Core interfacess
	ID3D12Device5* device;
	//Command Queues
	//for GPU to execute the command
	ID3D12CommandQueue* graphicsQueue;
	ID3D12CommandQueue* copyQueue;
	ID3D12CommandQueue* computeQueue;
	IDXGISwapChain3* swapchain;

	//One for each frame
	//List to record the command,and store it into the allocator,
	//Allocator used to storet the command,
	/*CPU:
    commandAllocator->Reset()
    commandList->Reset(commandAllocator)

    record commands → commandList
    commandList->Close()
	GPU:
    commandQueue->ExecuteCommandLists(&commandList)
    GPU executes commands asynchronously*/
	ID3D12CommandAllocator* graphicsCommandAllocator[2];
	ID3D12GraphicsCommandList4* graphicsCommandList[2];
	//Backbuffer
	ID3D12DescriptorHeap* backbufferHeap;
	ID3D12Resource** backbuffers;
	//fences
	GPUFence graphicsQueueFence[2];
	//Depth Buffer
	ID3D12DescriptorHeap* dsvHeap;
	ID3D12Resource* dsv;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	//viewport and scissor rect
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	//Root Signatures
	//– Add to Core class
	//– We will only use one Root Signature
	//	• Games usually do this
	//– Will build on it
	ID3D12RootSignature* rootSignature;
	//std::unordered_map<std::string, int> rootSignatureSlots;

	DescriptorHeap srvHeap;

	void init(HWND hwnd, int _width, int _height)
	{
		ID3D12Debug1* debug;
		HRESULT hr_debug = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
		debug->EnableDebugLayer();
		debug->Release();

		//Enumerate adapters
		IDXGIAdapter1* adapterf;
		std::vector<IDXGIAdapter1*> adapters;
		IDXGIFactory6* factory = NULL;
		CreateDXGIFactory(__uuidof(IDXGIFactory6), (void**)&factory);
		int i = 0;
		while (factory->EnumAdapters1(i, &adapterf) != DXGI_ERROR_NOT_FOUND)
		{
			adapters.push_back(adapterf);
			i++;
		}

		//Find the best adapter
		long long maxVideoMemory = 0;
		int useAdapterIndex = 0;
		for (int i = 0; i < adapters.size(); i++)
		{
			DXGI_ADAPTER_DESC desc;
			adapters[i]->GetDesc(&desc);
			if (desc.DedicatedVideoMemory > maxVideoMemory)
			{
				maxVideoMemory = desc.DedicatedVideoMemory;
				useAdapterIndex = i;
			}
		}
		adapter = adapters[useAdapterIndex];

		//Create DX12 Device on Adapter
		D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device));

		//Create Command Queues
		D3D12_COMMAND_QUEUE_DESC graphicsQueueDesc = {};
		graphicsQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		device->CreateCommandQueue(&graphicsQueueDesc, IID_PPV_ARGS(&graphicsQueue));
		D3D12_COMMAND_QUEUE_DESC copyQueueDesc = {};
		copyQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		device->CreateCommandQueue(&copyQueueDesc, IID_PPV_ARGS(&copyQueue));
		D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
		computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&computeQueue));

		//SwapChain
		//Fill in structure describing SwapChain
		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.Width = _width;
		scDesc.Height = _height;
		scDesc.SampleDesc.Count = 1; // MSAA here
		scDesc.SampleDesc.Quality = 0;
		scDesc.BufferCount = 2;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		//Create the swapchain
		IDXGISwapChain1* swapChain1;
		factory->CreateSwapChainForHwnd(graphicsQueue, hwnd, &scDesc, NULL, NULL, &swapChain1);
		swapChain1->QueryInterface(&swapchain);
		swapChain1->Release();

		//Create Command Allocators and Command Lists
		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&graphicsCommandAllocator[0]));
		device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE,
			IID_PPV_ARGS(&graphicsCommandList[0]));
		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&graphicsCommandAllocator[1]));
		device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE,
			IID_PPV_ARGS(&graphicsCommandList[1]));

		//Create Heap
		D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc = {};
		renderTargetViewHeapDesc.NumDescriptors = scDesc.BufferCount;
		renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		device->CreateDescriptorHeap(&renderTargetViewHeapDesc, IID_PPV_ARGS(&backbufferHeap));

		//Allocate memory for Backbuffer array
		backbuffers = new ID3D12Resource * [scDesc.BufferCount];

		//Get backbuffers and create views on heap
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = backbufferHeap->GetCPUDescriptorHandleForHeapStart();
		unsigned int renderTargetViewDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		for (unsigned int i = 0; i < 2; i++)
		{
			swapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffers[i]));
			device->CreateRenderTargetView(backbuffers[i], nullptr, renderTargetViewHandle);
			renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;
		}

		//Create fences
		graphicsQueueFence[0].create(device);
		graphicsQueueFence[1].create(device);

		//Create Descriptor Heap
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		memset(&dsvHeapDesc, 0, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
		dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();

		//Create Depth Buffer 
		// – Fill in structs
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
		D3D12_CLEAR_VALUE depthClearValue = {};
		depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		depthClearValue.DepthStencil.Depth = 1.0f;
		depthClearValue.DepthStencil.Stencil = 0;

		// - Specify which type of memory to allocate Depth Buffer.
		// - Want fast on chip memory
		D3D12_HEAP_PROPERTIES heapprops = {};
		heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapprops.CreationNodeMask = 1;
		heapprops.VisibleNodeMask = 1;

		//Specify resource information
		D3D12_RESOURCE_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.Width = _width;
		dsvDesc.Height = _height;
		dsvDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsvDesc.DepthOrArraySize = 1;
		dsvDesc.MipLevels = 1;
		dsvDesc.SampleDesc.Count = 1;
		dsvDesc.SampleDesc.Quality = 0;
		dsvDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		dsvDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

		//– Allocate memory for Resource
		device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &dsvDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthClearValue, IID_PPV_ARGS(&dsv));

		//– Create DepthStencilView to Resource on descriptor heap
		device->CreateDepthStencilView(dsv, &depthStencilDesc, dsvHandle);

		//Define viewport and scissor rect
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = (float)_width;
		viewport.Height = (float)_height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		scissorRect.left = 0;
		scissorRect.top = 0;
		scissorRect.right = _width;
		scissorRect.bottom = _height;

		//Create Root Signature
		// The role of the Root Signature:

		//It tells the GPU :
		// "What resources does my shader need? In which binding slot (b# / t# / u#) 
		// should these resources be placed?"
		//– Allows drawing
		//Update Root Signature (GPU Pipiline(2) page103)
		std::vector<D3D12_ROOT_PARAMETER> parameters;
		D3D12_ROOT_PARAMETER rootParameterCBVS;
		rootParameterCBVS.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameterCBVS.Descriptor.ShaderRegister = 0; // Register(b0)
		rootParameterCBVS.Descriptor.RegisterSpace = 0;
		rootParameterCBVS.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		parameters.push_back(rootParameterCBVS);
		D3D12_ROOT_PARAMETER rootParameterCBPS;
		rootParameterCBPS.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameterCBPS.Descriptor.ShaderRegister = 0; // Register(b0)
		rootParameterCBPS.Descriptor.RegisterSpace = 0;
		rootParameterCBPS.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		parameters.push_back(rootParameterCBPS);

		//descriptor
		D3D12_DESCRIPTOR_RANGE srvRange = {};
		srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		srvRange.NumDescriptors = 3; // number of SRVs (t0–t7)
		srvRange.BaseShaderRegister = 0; // starting at t0
		srvRange.RegisterSpace = 0;
		srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		D3D12_ROOT_PARAMETER rootParameterTex;
		rootParameterTex.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameterTex.DescriptorTable.NumDescriptorRanges = 1;
		rootParameterTex.DescriptorTable.pDescriptorRanges = &srvRange;
		rootParameterTex.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		parameters.push_back(rootParameterTex);

		D3D12_ROOT_PARAMETER rootParameterCBVS2;
		rootParameterCBVS2.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameterCBVS2.Descriptor.ShaderRegister = 1; // Register(b1)
		rootParameterCBVS2.Descriptor.RegisterSpace = 0;
		rootParameterCBVS2.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		parameters.push_back(rootParameterCBVS2);

		D3D12_ROOT_PARAMETER rootParameterCBVS_Time;
		rootParameterCBVS_Time.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameterCBVS_Time.Descriptor.ShaderRegister = 2; // Register(b2)
		rootParameterCBVS_Time.Descriptor.RegisterSpace = 0;
		rootParameterCBVS_Time.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		parameters.push_back(rootParameterCBVS_Time);

		D3D12_ROOT_PARAMETER rootParameterCBVS_UI;
		rootParameterCBVS_UI.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameterCBVS_UI.Descriptor.ShaderRegister = 3; // Register(b2)
		rootParameterCBVS_UI.Descriptor.RegisterSpace = 0;
		rootParameterCBVS_UI.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		parameters.push_back(rootParameterCBVS_UI);

		D3D12_STATIC_SAMPLER_DESC staticSampler = {};
		staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSampler.MipLODBias = 0;
		staticSampler.MaxAnisotropy = 1;
		staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		staticSampler.MinLOD = 0.0f;
		staticSampler.MaxLOD = D3D12_FLOAT32_MAX;
		staticSampler.ShaderRegister = 0;
		staticSampler.RegisterSpace = 0;
		staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


		//Update Root Signature Description
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.NumParameters = parameters.size();
		desc.pParameters = &parameters[0];
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		desc.NumStaticSamplers = 1;
		desc.pStaticSamplers = &staticSampler;

		ID3DBlob* serialized;
		ID3DBlob* error;
		D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized, &error);
		HRESULT hr = device->CreateRootSignature(0, serialized->GetBufferPointer(), serialized -> GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		serialized->Release();

		srvHeap.init(device, 16384);
	}

	//Add method to reset command allocator and command list
	//	– Find correct command list from Back Buffer in use
	void resetCommandList()
	{
		unsigned int frameIndex = swapchain->GetCurrentBackBufferIndex();
		graphicsCommandAllocator[frameIndex]->Reset();
		graphicsCommandList[frameIndex]->Reset(graphicsCommandAllocator[frameIndex], NULL);
	}

	//Add method to get current Command List
	//– Will need this when issuing commands
	ID3D12GraphicsCommandList4* getCommandList()
	{
		unsigned int frameIndex = swapchain->GetCurrentBackBufferIndex();
		return graphicsCommandList[frameIndex];
	}

	//Close and execute the list
	void runCommandList()
	{
		getCommandList()->Close();
		ID3D12CommandList* lists[] = { getCommandList() };
		//submit the commands to the gpu
		graphicsQueue->ExecuteCommandLists(1, lists);
	}

	//Ensures all work completed before moving on
	void flushGraphicsQueue() {
		graphicsQueueFence[0].signal(graphicsQueue);
		graphicsQueueFence[0].wait();
	}

	void beginFrame()
	{
		//Get Backbuffer to draw to
		//– Find Backbuffer index
		unsigned int frameIndex = swapchain->GetCurrentBackBufferIndex();
		//Ensure the GPU has finished and present current backbuffer
		graphicsQueueFence[frameIndex].wait();

		//Get Backbuffer to draw to
		//Find RenderTargetView at index
		//  • Find value from heap
		//	• Increment by index
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = backbufferHeap -> GetCPUDescriptorHandleForHeapStart();
		unsigned int renderTargetViewDescriptorSize = device -> GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		renderTargetViewHandle.ptr += frameIndex * renderTargetViewDescriptorSize;

		//Clear Backbuffer and Depth Buffer
		//– Issue commands on the command list
		resetCommandList();
		Barrier::add(backbuffers[frameIndex], D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET, getCommandList());
		getCommandList()->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, &dsvHandle);
		float color[4];
		color[0] = 0;
		color[1] = 0;
		color[2] = 1.0;
		color[3] = 1.0;
		getCommandList()->ClearRenderTargetView(renderTargetViewHandle, color, 0, NULL);
		getCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
	}

	void finishFrame()
	{
		unsigned int frameIndex = swapchain->GetCurrentBackBufferIndex();
		Barrier::add(backbuffers[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT, getCommandList());
		runCommandList();
		graphicsQueueFence[frameIndex].signal(graphicsQueue);
		swapchain->Present(1, 0);
	}

	//Copy to upload heap. Copy from upload heap to GPU memory
	void uploadResource(ID3D12Resource* dstResource, const void* data, unsigned int size,
		D3D12_RESOURCE_STATES targetState, D3D12_PLACED_SUBRESOURCE_FOOTPRINT* texFootprint = NULL)
	{
		//Allocate memory in upload heap
		ID3D12Resource* uploadBuffer;
		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Width = size;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer));

		//Get pointer to allocated memory on upload heap(Map)
		//Memcpy vertex data(or any data)
		//Tell driver we are done(Unmap)
		void* mappeddata = NULL;
		uploadBuffer->Map(0, NULL, &mappeddata);
		memcpy(mappeddata, data, size);
		uploadBuffer->Unmap(0, NULL);

		//Allocate commands to copy
		//– Reset command list
		resetCommandList();

		//Issue copy command
		//• Handle buffers (vertices etc)
		//• Handle textures
		//	– Later lecture
		if (texFootprint != NULL)
		{
			D3D12_TEXTURE_COPY_LOCATION src = {};
			src.pResource = uploadBuffer;
			src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			src.PlacedFootprint = *texFootprint;
			D3D12_TEXTURE_COPY_LOCATION dst = {};
			dst.pResource = dstResource;
			dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dst.SubresourceIndex = 0;
			getCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, NULL);
		}
		else
		{
			getCommandList()->CopyBufferRegion(dstResource, 0, uploadBuffer, 0, size);
		}

		//Transition buffer to final state after copying
		Barrier::add(dstResource, D3D12_RESOURCE_STATE_COPY_DEST, targetState, getCommandList());

		//Close and execute command lists
		runCommandList();
		//Wait for the command to finish
		flushGraphicsQueue();
		//Release upload heap memory
		uploadBuffer->Release();

	}

	//Functionality to set common draw functionality
	void beginRenderPass()
	{
		getCommandList()->RSSetViewports(1, &viewport);
		getCommandList()->RSSetScissorRects(1, &scissorRect);
		getCommandList()->SetGraphicsRootSignature(rootSignature);
		getCommandList()->SetDescriptorHeaps(1, &srvHeap.heap);
	}

	//the frame index
	unsigned int frameIndex()
	{
		return swapchain->GetCurrentBackBufferIndex();
	}

	//Release() DirectX objects
	~Core()
	{
		rootSignature->Release();
		graphicsCommandList[0]->Release();
		graphicsCommandAllocator[0]->Release();
		graphicsCommandList[1]->Release();
		graphicsCommandAllocator[1]->Release();
		swapchain->Release();
		computeQueue->Release();
		copyQueue->Release();
		graphicsQueue->Release();
		device->Release();
	}
};
