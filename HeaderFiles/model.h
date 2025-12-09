#pragma once
#include <array>
#include "vectors.h"
#include "vertexLayoutCache.h"
#include "mesh.h"
#include "shader.h"
#include "pipline.h"
#include "GEMLoader.h"
#include "animation.h"
#include "textureloader.h"

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

class AABB {
private:
	Vec3 m_min;
	Vec3 m_max;
	Vec3 m_center;      // 缓存中心点
	Vec3 m_halfSize;    // 缓存半长
	bool m_dirty;       // 脏标记

public:
	AABB() : m_min(FLT_MAX, FLT_MAX, FLT_MAX),
		m_max(-FLT_MAX, -FLT_MAX, -FLT_MAX),
		m_dirty(true) {
	}

	AABB(const Vec3& min, const Vec3& max) :
		m_min(min), m_max(max), m_dirty(true) {
	}

	AABB(const std::vector<Vec3>& points) {
		reset();
		for (const auto& point : points) {
			expand(point);
		}
	}

	// reset to orignal state
	void reset() {
		m_min = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
		m_max = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		m_dirty = true;
	}

	// check the point value to update
	void expand(const Vec3& point) {
		m_min.x = min(m_min.x, point.x);
		m_min.y = min(m_min.y, point.y);
		m_min.z = min(m_min.z, point.z);

		m_max.x = max(m_max.x, point.x);
		m_max.y = max(m_max.y, point.y);
		m_max.z = max(m_max.z, point.z);

		m_dirty = true;
	}

	// update from another object
	void expand(const AABB& other) {
		expand(other.m_min);
		expand(other.m_max);
	}

	// caculate value
	void update_cache() {
		if (m_dirty) {
			m_center = (m_min + m_max) * 0.5f;
			m_halfSize = (m_max - m_min) * 0.5f;
			m_dirty = false;
		}
	}

	const Vec3& getMin() const { return m_min; }
	const Vec3& getMax() const { return m_max; }

	Vec3 get_center() {
		update_cache();
		return m_center;
	}

	Vec3 get_halfSize() {
		update_cache();
		return m_halfSize;
	}

	float get_volume() const {
		Vec3 size = m_max - m_min;
		return size.x * size.y * size.z;
	}

	// get 8 vertex
	std::array<Vec3, 8> get_vertices() const {
		return {
			Vec3(m_min.x, m_min.y, m_min.z),
			Vec3(m_max.x, m_min.y, m_min.z),
			Vec3(m_min.x, m_max.y, m_min.z),
			Vec3(m_max.x, m_max.y, m_min.z),
			Vec3(m_min.x, m_min.y, m_max.z),
			Vec3(m_max.x, m_min.y, m_max.z),
			Vec3(m_min.x, m_max.y, m_max.z),
			Vec3(m_max.x, m_max.y, m_max.z)
		};
	}

	// transform AABB box
	AABB transform(const Matrix& transform) const {
		std::array<Vec3, 8> vertices = get_vertices();
		AABB result;

		for (const auto& vertex : vertices) {
			Vec3 transformed = transform.mulVec(vertex);
			result.expand(transformed);
		}

		return result;
	}
};

class Plane
{
public:
	Mesh mesh;
	Shader_Manager* shader_manager;
	PSOManager* psos;
	std::string shaderName;
	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		Frame frame;
		frame.fromVector(n);
		v.tangent = frame.u;
		v.tu = tu;
		v.tv = tv;
		return v;
	}
	void init(Core* core, PSOManager* _psos, Shader_Manager* _shader_manager)
	{
		shader_manager = _shader_manager;
		psos = _psos;
		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-1000, 0, -1000), Vec3(0, 1000, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(1000, 0, -1000), Vec3(0, 1000, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-1000, 0, 1000), Vec3(0, 1000, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(1000, 0, 1000), Vec3(0, 1000, 0), 1000, 1000));
		std::vector<unsigned int> indices;
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);
		mesh.init(core, vertices, indices);
		//shaders->load(core, "StaticModelUntextured", "VS.txt", "PSUntextured.txt");
		shader_manager->load(core, "PixelShader1", Shader_Type::PIXEL);
		shaderName = "StaticModelUntextured";
		psos->createPSO(core, "StaticModelUntexturedPSO", shader_manager->shaders["VertexShader"].shader, shader_manager->shaders["PixelShader1"].shader, VertexLayoutCache::getStaticLayout());
	}
	void draw(Core* core, PSOManager* psos, Shader_Manager* shader_manager, Matrix& vp)
	{
		Matrix planeWorld;
		shader_manager->update("VertexShader", "staticMeshBuffer", "W", &planeWorld);
		shader_manager->update("VertexShader", "staticMeshBuffer", "VP", &vp);
		apply(core);
		psos->bind(core, "StaticModelUntexturedPSO");
		mesh.draw(core);
	}
	void apply(Core* core)
	{
		unsigned int slot = 0;
		for (auto& vs : shader_manager->shaders["VertexShader"].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(0, vs.second.getGPUAddress());
			vs.second.next();
			slot++;
		}
		for (auto& ps : shader_manager->shaders["PixelShader1"].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(slot, ps.second.getGPUAddress());
			ps.second.next();
			slot++;
		}
	}
};

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
	std::vector<Mesh*> meshes;
	Shader_Manager* shader_manager;
	PSOManager* psos;

	std::vector<std::string> textureFilenames;
	Texture_Manager textures;

	AABB hitbox;

	void init_meshes(Core* core, std::string filename)
	{
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		GEMLoader::GEMAnimation gemanimation;
		std::string root = "Models/" + filename + ".gem";
		//std::string gem_root = root + filename + ".gem";
		loader.load(root, gemmeshes, gemanimation);
		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh* mesh = new Mesh();
			std::vector<STATIC_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
				STATIC_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
				vertices.push_back(v);
				hitbox.expand(v.pos);
			}

			std::string tex_root = gemmeshes[i].material.find("albedo").getValue();
			textureFilenames.push_back(tex_root);
			// Load texture with filename: gemmeshes[i].material.find("albedo").getValue()
			textures.load(core, tex_root);

			mesh->init(core, vertices, gemmeshes[i].indices);
			meshes.push_back(mesh);
		}
		hitbox.update_cache();
	}
	void init(Core* core, Shader_Manager* _shader_manager, PSOManager* _psos, std::string filename)
	{
		shader_manager = _shader_manager;
		psos = _psos;
		name = filename;
		init_meshes(core, filename);
		psos->createPSO(core, "Object", shader_manager->shaders["VertexShader"].shader, shader_manager->shaders["PixelShader"].shader, VertexLayoutCache::getStaticLayout());
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
		for (int i = 0; i < meshes.size(); i++)
		{
			shader_manager->updateTexturePS(core, "PixelShader", "tex", textures.find(textureFilenames[i]));
			meshes[i]->draw(core);
		}
	}
};

class Object_Animation
{
public:
	std::string name;
	std::vector<Mesh*> meshes;
	Animation animation;
	AnimationInstance animation_instance;

	Shader_Manager* shader_manager;
	PSOManager* psos;

	std::vector<std::string> textureFilenames;
	Texture_Manager textures;

	AABB hitbox;

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
		std::string root = "Models/" + filename + ".gem";
		//std::string gem_root = root + filename + ".gem";
		loader.load(root, gemmeshes, gemanimation);
		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh* mesh = new Mesh();
			std::vector<ANIMATED_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++) {
				ANIMATED_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));
				vertices.push_back(v);
				hitbox.expand(v.pos);
			}

			std::string tex_root = gemmeshes[i].material.find("albedo").getValue();
			textureFilenames.push_back(tex_root);
			// Load texture with filename: gemmeshes[i].material.find("albedo").getValue()
			textures.load(core, tex_root);

			mesh->init_animation(core, vertices, gemmeshes[i].indices);
			meshes.push_back(mesh);
		}
		hitbox.update_cache();

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

		animation_instance.init(&animation, 0);
	}

	void updateWorld( Matrix& w)
	{
		shader_manager->update("AnimationShader", "animatedMeshBuffer", "W", &w);
	}

	void update_animation_instance(AnimationInstance* ani_in, float dt, std::string move)
	{
		ani_in->update(move, dt);
		if (ani_in->animationFinished() == true)
		{
			ani_in->resetAnimationTime();
		}
	}
	void update(Matrix& planeWorld, Matrix& vp, float dt, std::string move) {
		update_animation_instance(&animation_instance, dt, move);
		shader_manager->update("AnimationShader", "animatedMeshBuffer", "W", &planeWorld);
		shader_manager->update("AnimationShader", "animatedMeshBuffer", "VP", &vp);
		shader_manager->update("AnimationShader", "animatedMeshBuffer", "bones", animation_instance.matrices);
		//hitbox.
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
			shader_manager->updateTexturePS(core, "PixelShader", "tex", textures.find(textureFilenames[i]));
			meshes[i]->draw(core);
		}

	}
};