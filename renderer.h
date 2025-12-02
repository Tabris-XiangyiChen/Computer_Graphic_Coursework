#pragma once
#include <fstream>
#include <sstream>
#include <map>
#include "pipline.h"
#include "constantbuffer.h"
#include "mesh_3.h"

static std::string readfile(std::string filename)
{
	std::ifstream file(filename);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

class Renderer
{
public:
	//ScreenSpaceTriangle vertices;
	Mesh_2 mesh;
	//Mesh mesh;

	//shaders
	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;

	std::string vertexShaderStr;
	std::string pixelShaderStr;

	//not using yet,to implement many shaders
	//std::map<std::string, Shader> shaders;


	//Create instance of Pipeline State Manager
	PSOManager psos;

	//ConstantBuffer constantBuffer;
	//ConstantBuffer_re constantBuffer;
	//ConstantBuffer2 cb;
	std::unordered_map<std::string, ConstantBuffer_re> vs_constantBuffer;
	std::unordered_map<std::string, ConstantBuffer_re> ps_constantBuffer;
	//std::vector<ConstantBuffer_re> ;

	unsigned int reflect(ID3DBlob* shader, std::unordered_map<std::string, ConstantBuffer_re>& _constantbuffer)
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
			_constantbuffer.insert({ buffer.name, buffer });
		}
		return totalSize;
	}

	void init(Core* _core)
	{
		//mesh.init(_core, &vertices.vertices[0], sizeof(PRIM_VERTEX), 3);

		//std::vector<STATIC_VERTEX> vertices;
		////vertices.push_back(Plane::addVertex(Vec3(-10, 0, -10), Vec3(0, 1, 0), 0, 0));
		////vertices.push_back(Plane::addVertex(Vec3(10, 0, -10), Vec3(0, 1, 0), 1, 0));
		////vertices.push_back(Plane::addVertex(Vec3(-10, 0, 10), Vec3(0, 1, 0), 0, 1));
		////vertices.push_back(Plane::addVertex(Vec3(10, 0, 10), Vec3(0, 1, 0), 1, 1));
		//vertices.push_back(Plane::addVertex(Vec3(-1.f, 0, -1.f), Vec3(0, 1, 0), 0, 0));
		//vertices.push_back(Plane::addVertex(Vec3(1.f, 0, -1.f), Vec3(0, 1, 0), 1, 0));
		//vertices.push_back(Plane::addVertex(Vec3(-1.f, 0, 1.f), Vec3(0, 1, 0), 0, 1));
		//vertices.push_back(Plane::addVertex(Vec3(1.f, 0, 1.f), Vec3(0, 1, 0), 1, 1));
		//std::vector<unsigned int> indices;
		//indices.push_back(2); indices.push_back(1); indices.push_back(0);
		//indices.push_back(1); indices.push_back(2); indices.push_back(3);
		//mesh.init(_core, vertices, indices);

		std::vector<STATIC_VERTEX> vertices;
		Vec3 p0 = Vec3(-1.0f, -1.0f, -1.0f);
		Vec3 p1 = Vec3(1.0f, -1.0f, -1.0f);
		Vec3 p2 = Vec3(1.0f, 1.0f, -1.0f);
		Vec3 p3 = Vec3(-1.0f, 1.0f, -1.0f);
		Vec3 p4 = Vec3(-1.0f, -1.0f, 1.0f);
		Vec3 p5 = Vec3(1.0f, -1.0f, 1.0f);
		Vec3 p6 = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 p7 = Vec3(-1.0f, 1.0f, 1.0f);

		vertices.push_back(Plane::addVertex(p0, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f));
		vertices.push_back(Plane::addVertex(p1, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f));
		vertices.push_back(Plane::addVertex(p2, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f));
		vertices.push_back(Plane::addVertex(p3, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f));
		vertices.push_back(Plane::addVertex(p5, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f));
		vertices.push_back(Plane::addVertex(p4, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f));
		vertices.push_back(Plane::addVertex(p7, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 0.0f));
		vertices.push_back(Plane::addVertex(p6, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 0.0f));
		vertices.push_back(Plane::addVertex(p4, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(Plane::addVertex(p0, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(Plane::addVertex(p3, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(Plane::addVertex(p7, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f));

		vertices.push_back(Plane::addVertex(p1, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(Plane::addVertex(p5, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(Plane::addVertex(p6, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(Plane::addVertex(p2, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f));
		vertices.push_back(Plane::addVertex(p3, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(Plane::addVertex(p2, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(Plane::addVertex(p6, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(Plane::addVertex(p7, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f));
		vertices.push_back(Plane::addVertex(p4, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(Plane::addVertex(p5, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(Plane::addVertex(p1, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(Plane::addVertex(p0, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f));

		std::vector<unsigned int> indices;
		indices.push_back(0); indices.push_back(1); indices.push_back(2);
		indices.push_back(0); indices.push_back(2); indices.push_back(3);
		indices.push_back(4); indices.push_back(5); indices.push_back(6);
		indices.push_back(4); indices.push_back(6); indices.push_back(7);
		indices.push_back(8); indices.push_back(9); indices.push_back(10);
		indices.push_back(8); indices.push_back(10); indices.push_back(11);
		indices.push_back(12); indices.push_back(13); indices.push_back(14);
		indices.push_back(12); indices.push_back(14); indices.push_back(15);
		indices.push_back(16); indices.push_back(17); indices.push_back(18);
		indices.push_back(16); indices.push_back(18); indices.push_back(19);
		indices.push_back(20); indices.push_back(21); indices.push_back(22);
		indices.push_back(20); indices.push_back(22); indices.push_back(23);
		mesh.init(_core, vertices, indices);

		vertexShaderStr = readfile("./VertexShader.hlsl");
		pixelShaderStr = readfile("./PixelShader.hlsl");
		//Compile vertex shader
		ID3DBlob* status;
		HRESULT hr = D3DCompile(vertexShaderStr.c_str(), strlen(vertexShaderStr.c_str()), NULL,
			NULL, NULL, "VS", "vs_5_0", 0, 0, &vertexShader, &status);
		if (FAILED(hr))
		{
			(char*)status->GetBufferPointer();
		}
		//Compile pixel shader
		hr = D3DCompile(pixelShaderStr.c_str(), strlen(pixelShaderStr.c_str()), NULL, NULL,
			NULL, "PS", "ps_5_0", 0, 0, &pixelShader, &status);
		if (FAILED(hr))
		{
			(char*)status->GetBufferPointer();
		}

		psos.createPSO(_core, "Triangle", vertexShader, pixelShader, mesh.inputLayoutDesc);

		reflect(vertexShader, vs_constantBuffer);
		reflect(pixelShader, ps_constantBuffer);
		for (auto& pair : vs_constantBuffer)
		{
			//i.init_2(_core, 2);
			pair.second.init_2(_core, 2);
		}
		for (auto& pair : ps_constantBuffer)
		{
			//i.init_2(_core, 2);
			pair.second.init_2(_core, 2);
		}
	}

	void apply(Core* core)
	{
		unsigned int slot = 0;
		for (auto& pair : vs_constantBuffer)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(slot, pair.second.getGPUAddress());
			pair.second.next();
			//core->rootSignature.
			slot++;
		}
		for (auto i :ps_constantBuffer)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(slot, ps_constantBuffer[i.first].getGPUAddress());
			ps_constantBuffer[i.first].next();
			slot++;
		}
	}

	//void draw(Core* core, ConstantBuffer2* cb) {
	//	core->beginRenderPass();
	//	unsigned int index = core->frameIndex();
	//	//ps_constantBuffer["bufferName"].update("time", cb);
	//	//ps_constantBuffer["bufferName"].update("lights", &cb->lights);
	//	ps_constantBuffer["staticMeshBuffer"].update("W", &cb->lights);
	//	ps_constantBuffer["staticMeshBuffer"].update("vp", &cb->lights);
	//	apply(core, cb);
	//	psos.bind(core, "Triangle");
	//	mesh.draw(core);
	//}

	void draw(Core* core, Matrix* w, Matrix* vp) {
		core->beginRenderPass();
		unsigned int index = core->frameIndex();
		//ps_constantBuffer["bufferName"].update("time", cb);
		//ps_constantBuffer["bufferName"].update("lights", &cb->lights);
		vs_constantBuffer["staticMeshBuffer"].update("W", w);
		vs_constantBuffer["staticMeshBuffer"].update("VP", vp);
		apply(core);
		psos.bind(core, "Triangle");
		mesh.draw(core);
	}

}; 
