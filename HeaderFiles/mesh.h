#pragma once
#pragma once
#include "vectors.h"
#include "core.h"
#include "vertexLayoutCache.h"
#include "GEMLoader.h"

struct INSTANCE
{
	Matrix w;
};

class Mesh
{
public:

	ID3D12Resource* vertexBuffer;
	ID3D12Resource* indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	unsigned int numMeshIndices;

	Mesh() {}

	void init(Core* core, void* vertices, int vertexSizeInBytes, int numVertices,
		unsigned int* indices, int numIndices)
	{
		//Specify vertex buffer will be in GPU memory heap
		D3D12_HEAP_PROPERTIES heapprops = {};
		heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapprops.CreationNodeMask = 1;
		heapprops.VisibleNodeMask = 1;

		//Create vertex buffer on heap
		D3D12_RESOURCE_DESC vbDesc = {};
		vbDesc.Width = numVertices * vertexSizeInBytes;
		vbDesc.Height = 1;
		vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vbDesc.DepthOrArraySize = 1;
		vbDesc.MipLevels = 1;
		vbDesc.SampleDesc.Count = 1;
		vbDesc.SampleDesc.Quality = 0;
		vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		//Allocate memory
		core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &vbDesc,
			D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&vertexBuffer));
		//Copy vertices using our helper function
		core->uploadResource(vertexBuffer, vertices, numVertices * vertexSizeInBytes,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		//Fill in view in helper function
		vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vbView.StrideInBytes = vertexSizeInBytes;
		vbView.SizeInBytes = numVertices * vertexSizeInBytes;

		D3D12_RESOURCE_DESC ibDesc;
		memset(&ibDesc, 0, sizeof(D3D12_RESOURCE_DESC));
		ibDesc.Width = numIndices * sizeof(unsigned int);
		ibDesc.Height = 1;
		ibDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ibDesc.DepthOrArraySize = 1;
		ibDesc.MipLevels = 1;
		ibDesc.SampleDesc.Count = 1;
		ibDesc.SampleDesc.Quality = 0;
		ibDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		HRESULT hr = core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &ibDesc,
			D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&indexBuffer));
		core->uploadResource(indexBuffer, indices, numIndices * sizeof(unsigned int),
			D3D12_RESOURCE_STATE_INDEX_BUFFER);

		ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		ibView.Format = DXGI_FORMAT_R32_UINT;
		ibView.SizeInBytes = numIndices * sizeof(unsigned int);
		numMeshIndices = numIndices;
	}

	void init(Core* core, std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices)
	{
		init(core, &vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size());
		inputLayoutDesc = VertexLayoutCache::getStaticLayout();
	}

	void init_animation(Core* core, std::vector<ANIMATED_VERTEX> vertices, std::vector<unsigned int> indices)
	{
		init(core, &vertices[0], sizeof(ANIMATED_VERTEX), vertices.size(), &indices[0], indices.size());
		inputLayoutDesc = VertexLayoutCache::getAnimatedLayout();
	}

	void init_line(Core* core, std::vector<LINE_VERTEX> vertices, std::vector<unsigned int> indices)
	{
		init(core, &vertices[0], sizeof(LINE_VERTEX), vertices.size(), &indices[0], indices.size());
		inputLayoutDesc = VertexLayoutCache::getStaticLineLayout();
	}

	void draw(Core* core)
	{
		core->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		core->getCommandList()->IASetVertexBuffers(0, 1, &vbView);
		core->getCommandList()->IASetIndexBuffer(&ibView);
		core->getCommandList()->DrawIndexedInstanced(numMeshIndices, 1, 0, 0, 0);
	}

	void draw_line(Core* core)
	{
		core->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		core->getCommandList()->IASetVertexBuffers(0, 1, &vbView);
		core->getCommandList()->IASetIndexBuffer(&ibView);
		core->getCommandList()->DrawIndexedInstanced(numMeshIndices, 1, 0, 0, 0);
	}
};

class Mesh_Istancing
{
public:

	ID3D12Resource* vertexBuffer;
	ID3D12Resource* indexBuffer;
	ID3D12Resource* instanceBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_VERTEX_BUFFER_VIEW instanceView;
	D3D12_INDEX_BUFFER_VIEW ibView;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	unsigned int numMeshIndices;
	unsigned int instanceSizeInBytes = sizeof(INSTANCE);
	unsigned int numInstances;
	unsigned int maxInstances;

	void init_instance_buffer(Core* core, std::vector<INSTANCE>& instances, unsigned int maxInstanceCount)
	{
		maxInstances = maxInstanceCount;

		// 指定实例缓冲区在GPU内存堆中
		D3D12_HEAP_PROPERTIES heapProps = {};
		//– Can be only in upload heap if instance data changes often
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		// 创建实例缓冲区描述
		D3D12_RESOURCE_DESC instanceBufferDesc = {};
		instanceBufferDesc.Width = maxInstanceCount * instanceSizeInBytes;
		instanceBufferDesc.Height = 1;
		instanceBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		instanceBufferDesc.DepthOrArraySize = 1;
		instanceBufferDesc.MipLevels = 1;
		instanceBufferDesc.SampleDesc.Count = 1;
		instanceBufferDesc.SampleDesc.Quality = 0;
		instanceBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		// 创建实例缓冲区资源
		HRESULT hr = core->device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&instanceBufferDesc,
			D3D12_RESOURCE_STATE_COMMON,
			NULL,
			IID_PPV_ARGS(&instanceBuffer)
		);

		if (FAILED(hr)) {
			return;
		}

		// 设置实例缓冲区视图
		instanceView.BufferLocation = instanceBuffer->GetGPUVirtualAddress();
		instanceView.StrideInBytes = instanceSizeInBytes;
		instanceView.SizeInBytes = maxInstances * instanceSizeInBytes;

		update_instance_matix(core, instances);
	}

	void init(Core* core, void* vertices, int vertexSizeInBytes, int numVertices,
		unsigned int* indices, int numIndices, std::vector<INSTANCE>& instances, unsigned int maxInstanceNum)
	{
		//Specify vertex buffer will be in GPU memory heap
		D3D12_HEAP_PROPERTIES heapprops = {};
		heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapprops.CreationNodeMask = 1;
		heapprops.VisibleNodeMask = 1;

		//Create vertex buffer on heap
		D3D12_RESOURCE_DESC vbDesc = {};
		vbDesc.Width = numVertices * vertexSizeInBytes;
		vbDesc.Height = 1;
		vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vbDesc.DepthOrArraySize = 1;
		vbDesc.MipLevels = 1;
		vbDesc.SampleDesc.Count = 1;
		vbDesc.SampleDesc.Quality = 0;
		vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		//Allocate memory
		core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &vbDesc,
			D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&vertexBuffer));
		//Copy vertices using our helper function
		core->uploadResource(vertexBuffer, vertices, numVertices * vertexSizeInBytes,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		//Fill in view in helper function
		vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vbView.StrideInBytes = vertexSizeInBytes;
		vbView.SizeInBytes = numVertices * vertexSizeInBytes;

		D3D12_RESOURCE_DESC ibDesc;
		memset(&ibDesc, 0, sizeof(D3D12_RESOURCE_DESC));
		ibDesc.Width = numIndices * sizeof(unsigned int);
		ibDesc.Height = 1;
		ibDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ibDesc.DepthOrArraySize = 1;
		ibDesc.MipLevels = 1;
		ibDesc.SampleDesc.Count = 1;
		ibDesc.SampleDesc.Quality = 0;
		ibDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		HRESULT hr = core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &ibDesc,
			D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&indexBuffer));
		core->uploadResource(indexBuffer, indices, numIndices * sizeof(unsigned int),
			D3D12_RESOURCE_STATE_INDEX_BUFFER);

		ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		ibView.Format = DXGI_FORMAT_R32_UINT;
		ibView.SizeInBytes = numIndices * sizeof(unsigned int);
		numMeshIndices = numIndices;

		init_instance_buffer(core, instances, maxInstanceNum);
	}

	void init(Core* core, std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices, std::vector<INSTANCE>& instances, unsigned int maxInstanceNum = 100)
	{
		init(core, &vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size(), instances, maxInstanceNum);
		inputLayoutDesc = VertexLayoutCache::getStatictLayoutInstanced();
	}

	void init_animation(Core* core, std::vector<ANIMATED_VERTEX> vertices, std::vector<unsigned int> indices, std::vector<INSTANCE>& instances, unsigned int maxInstanceNum = 100)
	{
		init(core, &vertices[0], sizeof(ANIMATED_VERTEX), vertices.size(), &indices[0], indices.size(), instances, maxInstanceNum);
		inputLayoutDesc = VertexLayoutCache::getStatictLayoutInstanced();
	}

	void update_instance_matix(Core* core, std::vector<INSTANCE>& instances)
	{
		numInstances = instances.size();
		core->uploadResource(instanceBuffer, instances.data(), numInstances * instanceSizeInBytes,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}
	void draw(Core* core)
	{
		D3D12_VERTEX_BUFFER_VIEW bufferViews[2];
		bufferViews[0] = vbView;
		bufferViews[1] = instanceView;
		core->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		core->getCommandList()->IASetVertexBuffers(0, 2, bufferViews);
		core->getCommandList()->IASetIndexBuffer(&ibView);
		core->getCommandList()->DrawIndexedInstanced(numMeshIndices, numInstances, 0, 0, 0);
	}
};