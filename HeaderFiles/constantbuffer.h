#pragma once
#include "core.h"
#include <map>
#include <vector>
#include "vectors.h"

struct ConstantBufferVariable
{
	unsigned int offset;
	unsigned int size;
};

struct alignas(16) ConstantBuffer2
{
	float time;
	float padding[3];
	Vec4 lights[4];
};

//Shader Reflection(Code Reflection
class ConstantBuffer
{
public:
	//Create resource to store constant buffer
	//And CPU memory + size
	ID3D12Resource* constantBuffer;
	unsigned char* buffer;
	unsigned int cbSizeInBytes;
	unsigned int maxDrawCalls;
	unsigned int offsetIndex;

	std::string name;
	std::map<std::string, ConstantBufferVariable> constantBufferData;

	void init(Core* core, unsigned int _maxDrawCalls = 1024)
	{
		cbSizeInBytes = (cbSizeInBytes + 255) & ~255;
		maxDrawCalls = _maxDrawCalls;
		unsigned int cbSizeInBytesAligned = cbSizeInBytes * maxDrawCalls;
		offsetIndex = 0;
		HRESULT hr;
		D3D12_HEAP_PROPERTIES heapprops = {};
		heapprops.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapprops.CreationNodeMask = 1;
		heapprops.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC cbDesc = {};
		cbDesc.Width = cbSizeInBytesAligned;
		cbDesc.Height = 1;
		cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbDesc.DepthOrArraySize = 1;
		cbDesc.MipLevels = 1;
		cbDesc.SampleDesc.Count = 1;
		cbDesc.SampleDesc.Quality = 0;
		cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &cbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
			IID_PPV_ARGS(&constantBuffer));
		constantBuffer->Map(0, NULL, (void**)&buffer);
	}

	//Update via a memcpy
	void update(std::string name, void* data)
	{
		if (constantBufferData.find(name) != constantBufferData.end())
		{
			ConstantBufferVariable cbVariable = constantBufferData[name];
			unsigned int offset = offsetIndex * cbSizeInBytes;
			memcpy(&buffer[offset + cbVariable.offset], data, cbVariable.size);
		}
		else
		{
			return;
		}
	}

	//Get address of constant buffer in use
	D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const
	{
		return (constantBuffer->GetGPUVirtualAddress() + (offsetIndex * cbSizeInBytes));
	}

	//– Functionality to move to next constant buffer for each draw call
	//– Ring buffer(very simple)
	void next()
	{
		offsetIndex++;
		if (offsetIndex >= maxDrawCalls)
		{
			offsetIndex = 0;
		}
	}

	//unsigned int reflect(ID3DBlob* shader)
	//{
	//	std::unordered_map<std::string, unsigned int>  map;
	//	ID3D12ShaderReflection* reflection;
	//	D3DReflect(shader->GetBufferPointer(), shader->GetBufferSize(), IID_PPV_ARGS(&reflection));
	//	D3D12_SHADER_DESC desc;
	//	reflection->GetDesc(&desc);
	//	unsigned int totalSize = 0;
	//	for (int i = 0; i < desc.ConstantBuffers; i++)
	//	{
	//		//ConstantBuffer_re buffer;
	//		ID3D12ShaderReflectionConstantBuffer* constantBuffer = reflection->GetConstantBufferByIndex(i);
	//		D3D12_SHADER_BUFFER_DESC cbDesc;
	//		constantBuffer->GetDesc(&cbDesc);
	//		name = cbDesc.Name;
	//		//unsigned int totalSize = 0;
	//		for (int j = 0; j < cbDesc.Variables; j++)
	//		{
	//			ID3D12ShaderReflectionVariable* var = constantBuffer->GetVariableByIndex(j);
	//			D3D12_SHADER_VARIABLE_DESC vDesc;
	//			var->GetDesc(&vDesc);
	//			ConstantBufferVariable bufferVariable;
	//			bufferVariable.offset = vDesc.StartOffset;
	//			bufferVariable.size = vDesc.Size;
	//			constantBufferData.insert({ vDesc.Name, bufferVariable });
	//			totalSize += bufferVariable.size;
	//		}
	//	}
	//	return totalSize;
	//}
};