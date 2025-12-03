#pragma once
#include "vectors.h"
#include "vertexLayoutCache.h"
#include "mesh.h"
#include "shader.h"
#include "pipline.h"
#include "GEMLoader.h"

static STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
{
	STATIC_VERTEX v;
	v.pos = p;
	v.normal = n;
	v.tangent = Vec3(0, 0, 0); // For now
	v.tu = tu;
	v.tv = tv;
	return v;
}

//class Plane
//{
//public:
//
//};

class Cube
{
public:
	Mesh mesh;

	void init(Core* core)
	{
		std::vector<STATIC_VERTEX> vertices;
		Vec3 p0 = Vec3(-1.0f, -1.0f, -1.0f);
		Vec3 p1 = Vec3(1.0f, -1.0f, -1.0f);
		Vec3 p2 = Vec3(1.0f, 1.0f, -1.0f);
		Vec3 p3 = Vec3(-1.0f, 1.0f, -1.0f);
		Vec3 p4 = Vec3(-1.0f, -1.0f, 1.0f);
		Vec3 p5 = Vec3(1.0f, -1.0f, 1.0f);
		Vec3 p6 = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 p7 = Vec3(-1.0f, 1.0f, 1.0f);

		vertices.push_back(addVertex(p0, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p1, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p2, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p3, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f));
		vertices.push_back(addVertex(p5, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p4, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p7, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p6, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 0.0f));
		vertices.push_back(addVertex(p4, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p0, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p3, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p7, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f));


		vertices.push_back(addVertex(p1, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p5, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p6, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p2, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f));
		vertices.push_back(addVertex(p3, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p2, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p6, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p7, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f));
		vertices.push_back(addVertex(p4, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p5, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p1, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p0, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f));


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

		mesh.init(core, vertices, indices);
	}
};

class Object
{
public:
	StaticMeshs meshs;
	Shader_Manager* shader_manager;
	PSOManager psos;

	void init(Core* core, Shader_Manager* _shader_manager, std::string filename)
	{
		shader_manager = _shader_manager;
		meshs.init(core, filename);
		cbinit(core);
		psos.createPSO(core, "Object", shader_manager->shaders["VertexShader"].shader, shader_manager->shaders["PixelShader"].shader, meshs.meshes[0]->inputLayoutDesc);
	}
	//constantbuffer initalize
	void cbinit(Core* core)
	{
		for (auto& shader : shader_manager->shaders)
		{
			for (auto& cb : shader.second.constantBuffers)
			{
				cb.second.init(core, 1024);
			}
		}
	}

	void update(Matrix planeWorld, Matrix vp) {
		shader_manager->update("VertexShader", "staticMeshBuffer", "W", &planeWorld);
		shader_manager->update("VertexShader", "staticMeshBuffer", "VP", &vp);
	}

	void apply(Core* core)
	{
		unsigned int slot = 0;
		for (auto& vs : shader_manager->shaders["VertexShader"].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(slot, vs.second.getGPUAddress());
			vs.second.next();
			//core->rootSignature.
			slot++;
		}
		for (auto& ps : shader_manager->shaders["PixelShader"].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(slot, ps.second.getGPUAddress());
			ps.second.next();
			slot++;
		}
	}

	void draw(Core* core) {
		core->beginRenderPass();
		apply(core);
		psos.bind(core, "Object");
		meshs.draw(core);
	}
};

class Object_Manager
{
public:
	Cube cube;
	Shader_Manager* shader_manager;
	PSOManager psos;

	void init(Core* core, Shader_Manager* _shader_manager)
	{
		shader_manager = _shader_manager;
		cube.init(core);
		cbinit(core);
		psos.createPSO(core, "Cube", shader_manager->shaders["VertexShader"].shader, shader_manager->shaders["PixelShader"].shader, cube.mesh.inputLayoutDesc);
	}
	//constantbuffer initalize
	void cbinit(Core* core)
	{
		for (auto& shader : shader_manager->shaders)
		{
			for (auto& cb : shader.second.constantBuffers)
			{
				cb.second.init(core, 1024);
			}
		}
	}

	void update(Matrix planeWorld, Matrix vp) {
		shader_manager->update("VertexShader", "staticMeshBuffer", "W", &planeWorld);
		shader_manager->update("VertexShader", "staticMeshBuffer", "VP", &vp);
	}

	void apply(Core* core)
	{
		unsigned int slot = 0;
		for (auto& vs : shader_manager->shaders["VertexShader"].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(slot, vs.second.getGPUAddress());
			vs.second.next();
			//core->rootSignature.
			slot++;
		}
		for (auto& ps : shader_manager->shaders["PixelShader"].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(slot, ps.second.getGPUAddress());
			ps.second.next();
			slot++;
		}
	}

	void draw(Core* core) {
		core->beginRenderPass();
		apply(core);
		psos.bind(core, "Cube");
		cube.mesh.draw(core);
	}
};