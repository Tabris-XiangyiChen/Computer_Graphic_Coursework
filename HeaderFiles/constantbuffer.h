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

struct alignas(16) PER_FRAME_BUFFER
{
	float time;
	Vec3 cam_position;
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

	void updateBuffer(const void* data, unsigned int dataSize)
	{
		if (!buffer) {
			std::cerr << "ERROR: Buffer not mapped!" << std::endl;
			return;
		}

		if (dataSize > cbSizeInBytes) {
			std::cerr << "ERROR: Data size exceeds buffer size!" << std::endl;
			return;
		}

		unsigned int offset = offsetIndex * cbSizeInBytes;
		memcpy(&buffer[offset], data, dataSize);

		// 可选：输出调试信息
		// std::cout << "Updated buffer at offset " << offset 
		//           << ", size " << dataSize << std::endl;
	}

	void updateAtOffset(unsigned int offset, const void* data, unsigned int dataSize)
	{
		if (!buffer) {
			std::cerr << "ERROR: Buffer not mapped!" << std::endl;
			return;
		}

		if (offset + dataSize > cbSizeInBytes) {
			std::cerr << "ERROR: Update exceeds buffer bounds!" << std::endl;
			return;
		}

		unsigned int bufferOffset = offsetIndex * cbSizeInBytes + offset;
		memcpy(&buffer[bufferOffset], data, dataSize);
	}

	// 添加：设置buffer大小（在init之前调用）
	void setSize(unsigned int size)
	{
		cbSizeInBytes = size;
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
};

class ConstantBuffer_Manager
{
public:
	std::unordered_map<std::string, ConstantBuffer> constantBuffers;
	PER_FRAME_BUFFER frameData;

	void update(std::string cbname, std::string data_name, void* data)
	{
		if (find(cbname))
		{
			constantBuffers[cbname].update(data_name, data);
		}
	}

	void insert(std::string cbname, std::pair<std::string, ConstantBuffer> buffer)
	{
		if (find(cbname))
		{
			return;
		}
		else
		{
			constantBuffers.insert(buffer);
		}
	}

	bool find(std::string cbname)
	{
		if (constantBuffers.find(cbname) != constantBuffers.end())
			return true;
		else
			return false;
	}

	// for per frame buffer
	void init_perframe_data()
	{
		frameData.time = 0.0f;
		frameData.cam_position = Vec3(0.0f, 0.0f, 0.0f);

		std::cout << "Per-frame buffer initialized. Size: "
			<< sizeof(PER_FRAME_BUFFER) << " bytes" << std::endl;
	}

	void update_frameData(Core* core, float currentTime, Vec3 cam_pos)
	{
		frameData.time += currentTime;
		frameData.cam_position = cam_pos;
		if (!find("PerFrameBuffer"))
			return;

		constantBuffers["PerFrameBuffer"].update("time", &frameData.time);
		constantBuffers["PerFrameBuffer"].update("cam_pos", &frameData.cam_position);

		core->getCommandList()->SetGraphicsRootConstantBufferView(4, constantBuffers["PerFrameBuffer"].getGPUAddress());
		constantBuffers["PerFrameBuffer"].next();
	}

};