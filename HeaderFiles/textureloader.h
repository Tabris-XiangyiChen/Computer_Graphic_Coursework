#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "core.h"
#include <vector>

class Texture
{
public:
	ID3D12Resource* tex;
	int heapOffset;

	void load(Core* core, std::string filename)
	{
		int width = 0;
		int height = 0;
		int channels = 0;
		unsigned char* texels = stbi_load(filename.c_str(), &width, &height, &channels, 0);
		if (channels == 3) {
			channels = 4;
			unsigned char* texelsWithAlpha = new unsigned char[width * height * channels];
			for (int i = 0; i < (width * height); i++) {
				texelsWithAlpha[i * 4] = texels[i * 3];
				texelsWithAlpha[(i * 4) + 1] = texels[(i * 3) + 1];
				texelsWithAlpha[(i * 4) + 2] = texels[(i * 3) + 2];
				texelsWithAlpha[(i * 4) + 3] = 255;
			}
			// Initialize texture using width, height, channels, and texelsWithAlpha
			upload(core, width, height, channels, texelsWithAlpha);
			delete[] texelsWithAlpha;
		}
		else {
			// Initialize texture using width, height, channels, and texels
			upload(core, width, height, channels, texels);
		}
		stbi_image_free(texels);
	}

	void upload(Core* core, int width, int height, int channals, const void* data)
	{
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		D3D12_HEAP_PROPERTIES heapDesc;
		memset(&heapDesc, 0, sizeof(D3D12_HEAP_PROPERTIES));
		heapDesc.Type = D3D12_HEAP_TYPE_DEFAULT;
		D3D12_RESOURCE_DESC textureDesc;
		memset(&textureDesc, 0, sizeof(D3D12_RESOURCE_DESC));
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.MipLevels = 1;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		HRESULT hr = core->device->CreateCommittedResource(&heapDesc, D3D12_HEAP_FLAG_NONE, &textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&tex));
		HRESULT hr2 = core->device->GetDeviceRemovedReason();

		D3D12_RESOURCE_DESC desc = tex->GetDesc();
		unsigned long long size;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
		core->device->GetCopyableFootprints(&desc, 0, 1, 0, &footprint, NULL, NULL, &size);

		//unsigned int alignedWidth = ((width * channels) + 255) & ~255;
		unsigned int alignedWidth = ((width * 4) + 255) & ~255;
		core->uploadResource(tex, data, alignedWidth * height,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &footprint);

		HRESULT hr3 = core->device->GetDeviceRemovedReason();

		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = core->srvHeap.getNextCPUHandle();
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		core->device->CreateShaderResourceView(tex, &srvDesc, srvHandle);
		heapOffset = core->srvHeap.used - 1;
	}

};

class Material
{
public:
	std::string name;
	Texture albedo;
	Texture normalmapping;
	Texture rmax;

	unsigned int heapoffset;

	void load(Core* core,std::string _name, std::vector<std::string> filenames)
	{
		if (filenames.size() == 3)
		{
			name = _name;
			albedo.load(core, filenames[0]);
			normalmapping.load(core, filenames[1]);
			rmax.load(core, filenames[2]);
			heapoffset = albedo.heapOffset;
		}
	}
	void load_onlyALB(Core* core,std::string _name,std::string filenames)
	{

			name = _name;
			albedo.load(core, filenames);
			heapoffset = albedo.heapOffset;
	}

	//get the material first texture's gpu ptr
	D3D12_GPU_DESCRIPTOR_HANDLE get_GPU_handle(Core* core)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = core->srvHeap.gpuHandle;
		handle.ptr = handle.ptr + (UINT64)(heapoffset) * (UINT64)core->srvHeap.incrementSize;
		return handle;
	}
};

class Texture_Manager
{
public:
	std::unordered_map<std::string, Material* > materials;
	void load(Core* core, std::string name, std::vector<std::string> filenames)
	{
		if (materials.find(name) != materials.end())
			return;

		Material* material = new Material;
		material->load(core, name, filenames);
		materials.insert({ name, material });
	}

	void load_onlyALB(Core* core, std::string name,std::string filenames)
	{
		if (materials.find(name) != materials.end())
			return;

		Material* material = new Material;
		material->load_onlyALB(core, name, filenames);
		materials.insert({ name, material });
	}

	bool find(std::string name)
	{
		if (materials.find(name) != materials.end())
			return true;
		else
		{
			std::cout << "Didnt find material, name:" << name << std::endl;
			return false;
		}
	}
	D3D12_GPU_DESCRIPTOR_HANDLE get_material_GPU_handle(Core* core, std::string name)
	{
		if (find(name));
		return materials[name]->get_GPU_handle(core);
	}
	
	void updateTexturePS(Core* core, std::string material)
	{
		core->getCommandList()->SetGraphicsRootDescriptorTable(2, get_material_GPU_handle(core, material));
	}
};
