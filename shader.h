#pragma once
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "core.h"
#include "constantbuffer.h"
enum class Shader_Type
{
	VERTEX,
	PIXEL,
	MAX_TPYES
};
class Shader
{
	std::string name;
	ID3DBlob* shader;

	//different types struct of constantbuffer
	std::unordered_map<std::string, ConstantBuffer_re> constantBuffers;

	static std::string readfile(std::string filename)
	{
		std::ifstream file(filename);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	void init(std::string filename, Shader_Type shader_type, std::string shader_name)
	{
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
				(char*)status->GetBufferPointer();
			}
			break;
		case Shader_Type::PIXEL :
			hr = D3DCompile(shader_str.c_str(), strlen(shader_str.c_str()), NULL, NULL,
				NULL, "PS", "ps_5_0", 0, 0, &shader, &status);
			if (FAILED(hr))
			{
				(char*)status->GetBufferPointer();
			}
			break;
		}
	}

	unsigned int reflect()
	{
		std::unordered_map<std::string, unsigned int>  map;
		ID3D12ShaderReflection* reflection;
		D3DReflect(shader->GetBufferPointer(), shader->GetBufferSize(), IID_PPV_ARGS(&reflection));
		D3D12_SHADER_DESC desc;
		reflection->GetDesc(&desc);
		unsigned int totalSize = 0;
		for (int i = 0; i < desc.ConstantBuffers; i++)
		{
			ConstantBuffer_re buffer;
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
			constantBuffers.insert(std::pair<std::string, ConstantBuffer_re>(buffer.name, buffer));
		}
		return totalSize;
	}
};

class Shader_Manager
{
	std::unordered_map<std::string, Shader> shaders;

	void init()
	{

	}

	void load(std::string shader_name, Shader_Type type)
	{
		Shader shader;
		
	}
};