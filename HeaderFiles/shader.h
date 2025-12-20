#pragma once
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "core.h"
#include "constantbuffer.h"
#include "textureloader.h"


enum class Shader_Type
{
	VERTEX,
	PIXEL,
	MAX_TPYES
};
class Shader
{
public:
	std::string name;
	ID3DBlob* shader;

	//different types struct of constantbuffer
	std::unordered_map<std::string, ConstantBuffer> constantBuffers;
	ConstantBuffer_Manager* cb_manager;

	std::map<std::string, int> textureBindPoints;

	static std::string readfile(std::string filename)
	{
		std::ifstream file(filename);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	void init(std::string filename, Shader_Type shader_type, std::string shader_name, ConstantBuffer_Manager* _cb_manager)
	{
		cb_manager = _cb_manager;
		
		ID3DBlob* status;
		std::string shader_str = readfile(filename);
		name = shader_name;
		HRESULT hr;
		switch (shader_type)
		{
		case Shader_Type::VERTEX :
			hr = D3DCompile(shader_str.c_str(), strlen(shader_str.c_str()), NULL,
				NULL, NULL, "VS", "vs_5_0", 0, 0, &shader, &status);
			if (FAILED(hr))
			{
				if (status)
				{
					std::string errorMsg(
						(char*)status->GetBufferPointer(),
						status->GetBufferSize()
					);
					std::cout << "Shader compile error:\n" << errorMsg << std::endl;
				}
				else
				{
					std::cout << "Shader compile failed, no error message." << std::endl;
				}
			}
			break;
		case Shader_Type::PIXEL :
			hr = D3DCompile(shader_str.c_str(), strlen(shader_str.c_str()), NULL, NULL,
				NULL, "PS", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &shader, &status);
			if (FAILED(hr))
			{
				if (status)
				{
					std::string errorMsg(
						(char*)status->GetBufferPointer(),
						status->GetBufferSize()
					);
					std::cout << "Shader compile error:\n" << errorMsg << std::endl;
				}
				else
				{
					std::cout << "Shader compile failed, no error message." << std::endl;
				}
			}
			break;
		}
	}

	unsigned int reflect(Core* core)
	{
		ID3D12ShaderReflection* reflection;
		D3DReflect(shader->GetBufferPointer(), shader->GetBufferSize(), IID_PPV_ARGS(&reflection));
		D3D12_SHADER_DESC desc;
		reflection->GetDesc(&desc);
		unsigned int totalSize = 0;
		for (int i = 0; i < desc.ConstantBuffers; i++)
		{
			ConstantBuffer buffer;
			ID3D12ShaderReflectionConstantBuffer* constantBuffer = reflection->GetConstantBufferByIndex(i);
			D3D12_SHADER_BUFFER_DESC cbDesc;
			constantBuffer->GetDesc(&cbDesc);
			buffer.name = cbDesc.Name;
			unsigned int ConstantBuffer_totalSize = 0;
			for (int j = 0; j < cbDesc.Variables; j++)
			{
				ID3D12ShaderReflectionVariable* var = constantBuffer->GetVariableByIndex(j);
				D3D12_SHADER_VARIABLE_DESC vDesc;
				var->GetDesc(&vDesc);
				ConstantBufferVariable bufferVariable;
				bufferVariable.offset = vDesc.StartOffset;
				bufferVariable.size = vDesc.Size;
				buffer.constantBufferData.insert({ vDesc.Name, bufferVariable });
				totalSize += bufferVariable.size;
				ConstantBuffer_totalSize += bufferVariable.size;
			}
			buffer.cbSizeInBytes = ConstantBuffer_totalSize;
			buffer.init(core, 1024);

			//insert the preframe buffer into manager
			if (buffer.name == "PerFrameBuffer")
				cb_manager->insert(buffer.name, std::pair<std::string, ConstantBuffer>(buffer.name, buffer));
			else
				constantBuffers.insert(std::pair<std::string, ConstantBuffer>(buffer.name, buffer));
		}

		for (int i = 0; i < desc.BoundResources; i++)
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			reflection->GetResourceBindingDesc(i, &bindDesc);
			if (bindDesc.Type == D3D_SIT_TEXTURE)
			{
				textureBindPoints.insert({ bindDesc.Name, bindDesc.BindPoint });
			}
		}
		return totalSize;
	}


	//void updateTexturePS(Core* core, std::string name, int heapOffset) {
	//	UINT bindPoint = textureBindPoints[name];
	//	D3D12_GPU_DESCRIPTOR_HANDLE handle = core->srvHeap.gpuHandle;
	//	handle.ptr = handle.ptr + (UINT64)(heapOffset - bindPoint) * (UINT64)core->srvHeap.incrementSize;
	//	core->getCommandList()->SetGraphicsRootDescriptorTable(2, handle);
	//}

};

class Shader_Manager
{
	
public:
	std::unordered_map<std::string, Shader> shaders;
	//didnt use
	ConstantBuffer_Manager* cb_manager;

	void init(Core* core, ConstantBuffer_Manager* _cb_manager)
	{
		cb_manager = _cb_manager;
		//load(core, "VertexShader", Shader_Type::VERTEX);
		//load(core, "PixelShader", Shader_Type::PIXEL);
		load(core, "VS_Static_Ins_VAni", Shader_Type::VERTEX);
		//load(core, "PixelShaderWithTransparence", Shader_Type::PIXEL);
	}


	void load(Core* core, std::string shader_name, Shader_Type type)
	{
		if (shaders.find(shader_name) == shaders.end())
		{
			Shader shader;
			std::string folder_path = "Shaders/";
			std::string fileName = folder_path + shader_name + ".cso";
			std::wstring wideName = std::wstring(fileName.begin(), fileName.end());
			std::ifstream compiledFile(fileName);

			//if (compiledFile.is_open()) {//check if there is compiled shader code
			//	D3DReadFileToBlob(wideName.c_str(), &shader.shader);
			//}
			//else {//if not, generate a .cso file
				shader.init(std::string(folder_path + shader_name + ".hlsl"), type, shader_name, cb_manager);
				D3DWriteBlobToFile(shader.shader, wideName.c_str(), false);
			//}
			shader.reflect(core);
			shaders.insert(std::pair<std::string, Shader>(shader_name, shader));
		}
	}

	void update(std::string shader_name, std::string cb_name, std::string var_name, void* data)
	{
		if (shaders.find(shader_name) != shaders.end())
		{
			if (shaders[shader_name].constantBuffers.find(cb_name) != shaders[shader_name].constantBuffers.end())
			{
				shaders[shader_name].constantBuffers[cb_name].update(var_name, data);
			}
		}
		//if (shaders.find(shader_name) != shaders.end())
		//{
		//	if (cb_manager->find(cb_name))
		//	{
		//		cb_manager->update(cb_name, var_name, data);
		//	}
		//}
	}

	//void updateTexturePS(Core* core, std::string shader_name, std::string t_bindpoint, Texture* texture)
	//{
	//	shaders[shader_name].updateTexturePS(core, t_bindpoint, texture->heapOffset);
	//}

	Shader* find(std::string name)
	{
		if (shaders.find(name) != shaders.end())
			return &shaders[name];
		else
		{
			std::cout << "no such shader" << std::endl;
			return nullptr;
		}
	}

};