#pragma once
#include "vectors.h"
#include "vertexLayoutCache.h"
#include "mesh.h"
#include "shader.h"
#include "pipline.h"
#include "GEMLoader.h"
#include "animation.h"

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
	std::string name;
	StaticMeshs meshes;
	Shader_Manager* shader_manager;
	PSOManager* psos;

	void init(Core* core, Shader_Manager* _shader_manager, PSOManager* _psos, std::string filename)
	{
		shader_manager = _shader_manager;
		psos = _psos;
		name = filename;
		meshes.init(core, filename);
		//cbinit(core);
		psos->createPSO(core, "Object", shader_manager->shaders["VertexShader"].shader, shader_manager->shaders["PixelShader"].shader, meshes.meshes[0]->inputLayoutDesc);
	}
	//constantbuffer initalize
	//void cbinit(Core* core)
	//{
	//	for (auto& shader : shader_manager->shaders)
	//	{
	//		for (auto& cb : shader.second.constantBuffers)
	//		{
	//			cb.second.init(core, 1024);
	//		}
	//	}
	//}

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
		psos->bind(core, "Object");
		meshes.draw(core);
	}
};

class Object_Animation
{
public:
	std::string name;
	std::vector<Mesh*> meshes;
	Animation animation;

	Shader_Manager* shader_manager;
	PSOManager* psos;

	void init(Core* core, Shader_Manager* _shader_manager, PSOManager* _psos, std::string filename)
	{
		shader_manager = _shader_manager;
		psos = _psos;
		name = filename;

		shader_manager->load(core, "AnimationShader", Shader_Type::VERTEX);
		psos->createPSO(core, "AnimationModelPSO", shader_manager->find("AnimationShader")->shader, shader_manager->find("PixelShader")->shader, VertexLayoutCache::getAnimatedLayout());
		
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		GEMLoader::GEMAnimation gemanimation;
		loader.load(filename, gemmeshes, gemanimation);
		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh* mesh = new Mesh();
			std::vector<ANIMATED_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++) {
				ANIMATED_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));
				vertices.push_back(v);
			}
			mesh->init_animation(core, vertices, gemmeshes[i].indices);
			meshes.push_back(mesh);
		}
		//Bones
		memcpy(&animation.skeleton.globalInverse, &gemanimation.globalInverse, 16 * sizeof(float));
		for (int i = 0; i < gemanimation.bones.size(); i++)
		{
			Bone bone;
			bone.name = gemanimation.bones[i].name;
			memcpy(&bone.offset, &gemanimation.bones[i].offset, 16 * sizeof(float));
			bone.parentIndex = gemanimation.bones[i].parentIndex;
			animation.skeleton.bones.push_back(bone);
		}

		for (int i = 0; i < gemanimation.animations.size(); i++)
		{
			std::string name = gemanimation.animations[i].name;
			AnimationSequence aseq;
			aseq.ticksPerSecond = gemanimation.animations[i].ticksPerSecond;
			for (int n = 0; n < gemanimation.animations[i].frames.size(); n++)
			{
				AnimationFrame frame;
				for (int index = 0; index < gemanimation.animations[i].frames[n].positions.size(); index++)
				{
					Vec3 p;
					Quaternion q;
					Vec3 s;
					memcpy(&p, &gemanimation.animations[i].frames[n].positions[index], sizeof(Vec3));
					frame.positions.push_back(p);
					memcpy(&q, &gemanimation.animations[i].frames[n].rotations[index], sizeof(Quaternion));
					frame.rotations.push_back(q);
					memcpy(&s, &gemanimation.animations[i].frames[n].scales[index], sizeof(Vec3));
					frame.scales.push_back(s);
				}
				aseq.frames.push_back(frame);
			}
			animation.animations.insert({ name, aseq });
		}

	}

	void updateWorld( Matrix& w)
	{
		shader_manager->update("AnimationShader", "animatedMeshBuffer", "W", &w);
	}

	void update(Matrix& planeWorld, Matrix& vp, AnimationInstance* animat) {
		shader_manager->update("AnimationShader", "animatedMeshBuffer", "W", &planeWorld);
		shader_manager->update("AnimationShader", "animatedMeshBuffer", "VP", &vp);
		shader_manager->update("AnimationShader", "animatedMeshBuffer", "bones", animat->matrices);
	}

	void apply(Core* core)
	{
		unsigned int slot = 0;
		for (auto& vs : shader_manager->shaders["AnimationShader"].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(0, vs.second.getGPUAddress());
			vs.second.next();
			//core->rootSignature.
			slot++;
		}
		for (auto& ps : shader_manager->shaders["PixelShader"].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(1, ps.second.getGPUAddress());
			ps.second.next();
			slot++;
		}
	}

	void draw(Core* core) {
		core->beginRenderPass();
		apply(core);
		psos->bind(core, "AnimationModelPSO");
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i]->draw(core);
		}
	}
};