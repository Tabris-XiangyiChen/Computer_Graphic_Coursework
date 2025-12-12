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
	std::string name;
	Mesh mesh;
	Shader_Manager* shader_manager;
	PSOManager* psos;

	std::string vs_name = "VS_Static";
	std::string ps_name = "PS_OnlyALB";
	std::string pso_name = "StaticModel_OnlyALB_PSO";

	std::string textureFilename;
	Texture_Manager* textures;

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
	void init(Core* core, PSOManager* _psos, Shader_Manager* _shader_manager, Texture_Manager* _textures)
	{
		shader_manager = _shader_manager;
		psos = _psos;
		textures = _textures;
		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-1000, 0, -1000), Vec3(0, 1000, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(1000, 0, -1000), Vec3(0, 1000, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-1000, 0, 1000), Vec3(0, 1000, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(1000, 0, 1000), Vec3(0, 1000, 0), 1, 1));
		std::vector<unsigned int> indices;
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);
		mesh.init(core, vertices, indices);

		shader_manager->load(core, vs_name, Shader_Type::VERTEX);
		shader_manager->load(core, ps_name, Shader_Type::PIXEL);
		name = "StaticModelUntextured";
		psos->createPSO(core, pso_name, shader_manager->shaders[vs_name].shader, shader_manager->shaders[ps_name].shader, VertexLayoutCache::getStaticLayout());

		std::string tex_root_ALB = "Models/Textures/bark05_ALB.png";
		textureFilename = tex_root_ALB;
		// Load texture with filename: gemmeshes[i].material.find("albedo").getValue()
		textures->load_onlyALB(core, tex_root_ALB, tex_root_ALB);
	}

	void update(Matrix planeWorld, Matrix vp) {
		shader_manager->update(vs_name, "staticMeshBuffer", "W", &planeWorld);
		shader_manager->update(vs_name, "staticMeshBuffer", "VP", &vp);
	}

	void apply(Core* core)
	{
		unsigned int slot = 0;
		for (auto& vs : shader_manager->shaders[vs_name].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(3, vs.second.getGPUAddress());
			vs.second.next();
			slot++;
		}
		for (auto& ps : shader_manager->shaders[ps_name].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(1, ps.second.getGPUAddress());
			ps.second.next();
			slot++;
		}
	}
	void draw(Core* core, Matrix planeWorld, Matrix vp) {
		update(planeWorld, vp);
		core->beginRenderPass();
		apply(core);
		psos->bind(core, pso_name);

		//shader_manager->updateTexturePS(core, ps_name, "tex", textures->find(textureFilenames[0]));
		textures->updateTexturePS(core, textureFilename);
		mesh.draw(core);
		
	}
};

class Sphere
{
public:
	std::string name = "Sphere";
	Mesh mesh;
	Shader_Manager* shader_manager;
	PSOManager* psos;

	std::string vs_name = "VS_Static";
	std::string ps_name = "PS_OnlyALB";
	std::string pso_name = "StaticModel_OnlyALB_PSO";

	std::string textureFilename;
	Texture_Manager* textures;

	int rings= 500; 
	int segments = 500;
	float radius = 2000.f;

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

	void init_mesh(Core* core)
	{
		std::vector<STATIC_VERTEX> vertices;
		for (int lat = 0; lat <= rings; lat++) {
			float theta = lat * M_PI / rings;
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);
			for (int lon = 0; lon <= segments; lon++) {
				float phi = lon * 2.0f * M_PI / segments;
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);
				Vec3 position(radius * sinTheta * cosPhi, radius * cosTheta,
					radius * sinTheta * sinPhi);
				Vec3 normal = position.Normalize();
				float tu = 1.0f - (float)lon / segments;
				float tv = 1.0f - (float)lat / rings;
				vertices.push_back(addVertex(position, normal, tu, tv));
			}
		}
		std::vector<unsigned int> indices;
		for (int lat = 0; lat < rings; lat++)
		{
			for (int lon = 0; lon < segments; lon++)
			{
				int current = lat * (segments + 1) + lon;
				int next = current + segments + 1;
				indices.push_back(current);
				indices.push_back(next);
				indices.push_back(current + 1);
				indices.push_back(current + 1);
				indices.push_back(next);
				indices.push_back(next + 1);
			}
		}
		mesh.init(core, vertices, indices);

		std::string tex_root_ALB = "Models/Textures/farmland_2k.png";
		textureFilename = tex_root_ALB;
		// Load texture with filename: gemmeshes[i].material.find("albedo").getValue()
		textures->load_onlyALB(core, tex_root_ALB, tex_root_ALB);
	}
	void init(Core* core, PSOManager* _psos, Shader_Manager* _shader_manager, Texture_Manager* _textures)
	{
		shader_manager = _shader_manager;
		psos = _psos;
		textures = _textures;
		init_mesh(core);
		shader_manager->load(core, vs_name, Shader_Type::VERTEX);
		shader_manager->load(core, ps_name, Shader_Type::PIXEL);
		psos->createPSO(core, pso_name, shader_manager->shaders[vs_name].shader, shader_manager->shaders[ps_name].shader, VertexLayoutCache::getStaticLayout());
	}

	void update(Matrix planeWorld, Matrix vp) {
		shader_manager->update(vs_name, "staticMeshBuffer", "W", &planeWorld);
		shader_manager->update(vs_name, "staticMeshBuffer", "VP", &vp);
	}

	void apply(Core* core)
	{
		unsigned int slot = 0;
		for (auto& vs : shader_manager->shaders[vs_name].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(3, vs.second.getGPUAddress());
			vs.second.next();
			slot++;
		}
		for (auto& ps : shader_manager->shaders[ps_name].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(1, ps.second.getGPUAddress());
			ps.second.next();
			slot++;
		}
	}
	void draw(Core* core, Matrix planeWorld, Matrix vp) {
		update(planeWorld, vp);
		core->beginRenderPass();
		apply(core);
		psos->bind(core, pso_name);

		//shader_manager->updateTexturePS(core, ps_name, "tex", textures->find(textureFilenames[0]));
		textures->updateTexturePS(core, textureFilename);
		mesh.draw(core);

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

	std::string vs_name = "VS_Static";
	std::string ps_name = "PS_Trans";
	std::string pso_name = "StaticModel_Trans_PSO";

	//std::vector<std::vector<std::string>> textureFilenames;
	std::vector<std::string> textureFilenames;
	//std::vector<std::string> textureFilenames2;
	Texture_Manager* textures;
	//Texture_Manager2* textures2;

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

			//get all three textures file roots,
			std::string tex_root_alb = gemmeshes[i].material.find("albedo").getValue();
			std::string tex_root_nh = gemmeshes[i].material.find("nh").getValue();
			std::string tex_root_rmax = gemmeshes[i].material.find("rmax").getValue();
			//textureFilenames[i].push_back(tex_root_alb);

			//use the albedo texture name as the matarial name
			textureFilenames.push_back(tex_root_alb);
			std::vector<std::string> filenames;
			filenames.push_back(tex_root_alb);
			filenames.push_back(tex_root_nh);
			filenames.push_back(tex_root_rmax);
			// load 3 textures in the same matrial.
			textures->load(core, tex_root_alb, filenames);

			mesh->init(core, vertices, gemmeshes[i].indices);
			meshes.push_back(mesh);
		}
		hitbox.update_cache();
	}

	void init(Core* core, Shader_Manager* _shader_manager, PSOManager* _psos, Texture_Manager* _textures, std::string filename)
	{
		shader_manager = _shader_manager;
		psos = _psos;
		textures = _textures;
		name = filename;
		init_meshes(core, filename);
		shader_manager->load(core, vs_name, Shader_Type::VERTEX);
		shader_manager->load(core, ps_name, Shader_Type::PIXEL);
		psos->createPSO(core, pso_name, shader_manager->shaders[vs_name].shader, shader_manager->shaders[ps_name].shader, VertexLayoutCache::getStaticLayout());
	}

	void update(Matrix planeWorld, Matrix vp) {
		shader_manager->update(vs_name, "staticMeshBuffer", "W", &planeWorld);
		shader_manager->update(vs_name, "staticMeshBuffer", "VP", &vp);
	}

	void apply(Core* core)
	{
		unsigned int slot = 0;
		for (auto& vs : shader_manager->shaders[vs_name].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(3, vs.second.getGPUAddress());
			vs.second.next();
			slot++;
		}
		for (auto& ps : shader_manager->shaders[ps_name].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(1, ps.second.getGPUAddress());
			ps.second.next();
			slot++;
		}
	}

	void draw(Core* core) {
		core->beginRenderPass();
		apply(core);
		psos->bind(core, pso_name);
		for (int i = 0; i < meshes.size(); i++)
		{
			//shader_manager->updateTexturePS(core, "PixelShaderWithTransparence", "tex", textures->find(textureFilenames[i]));
			textures->updateTexturePS(core, textureFilenames[i]);
			meshes[i]->draw(core);
		}
	}
};

class Object_Instance
{
public:
	std::string name;
	std::vector<Mesh_Istancing*> meshes;
	Shader_Manager* shader_manager;
	PSOManager* psos;

	std::vector<INSTANCE> instances_matix;

	std::string vs_name = "VS_Static_Ins";
	std::string ps_name = "PS_Trans";
	std::string pso_name = "StaticModel_Trans_Ins_PSO";

	//std::vector<std::vector<std::string>> textureFilenames;
	std::vector<std::string> textureFilenames;
	//std::vector<std::string> textureFilenames2;
	Texture_Manager* textures;
	//Texture_Manager2* textures2;

	AABB hitbox;

	void create_matixes()
	{
		for (unsigned int i = 0; i < 5; i++)
		{
			INSTANCE matrix;
			matrix.w = Matrix();
			matrix.w.m[3] = i * 10;
			instances_matix.push_back(matrix);
		}
	}

	void init_meshes(Core* core, std::string filename)
	{
		create_matixes();
		
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		GEMLoader::GEMAnimation gemanimation;
		std::string root = "Models/" + filename + ".gem";
		//std::string gem_root = root + filename + ".gem";
		loader.load(root, gemmeshes, gemanimation);
		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh_Istancing* mesh = new Mesh_Istancing();
			std::vector<STATIC_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
				STATIC_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
				vertices.push_back(v);
				hitbox.expand(v.pos);
			}

			//get all three textures file roots,
			std::string tex_root_alb = gemmeshes[i].material.find("albedo").getValue();
			std::string tex_root_nh = gemmeshes[i].material.find("nh").getValue();
			std::string tex_root_rmax = gemmeshes[i].material.find("rmax").getValue();
			//textureFilenames[i].push_back(tex_root_alb);

			//use the albedo texture name as the matarial name
			textureFilenames.push_back(tex_root_alb);
			std::vector<std::string> filenames;
			filenames.push_back(tex_root_alb);
			filenames.push_back(tex_root_nh);
			filenames.push_back(tex_root_rmax);
			// load 3 textures in the same matrial.
			textures->load(core, tex_root_alb, filenames);
			
			mesh->init(core, vertices, gemmeshes[i].indices, instances_matix);
			meshes.push_back(mesh);
		}
		hitbox.update_cache();
	}

	void init(Core* core, Shader_Manager* _shader_manager, PSOManager* _psos, Texture_Manager* _textures, std::string filename)
	{
		shader_manager = _shader_manager;
		psos = _psos;
		textures = _textures;
		name = filename;
		init_meshes(core, filename);
		shader_manager->load(core, vs_name, Shader_Type::VERTEX);
		shader_manager->load(core, ps_name, Shader_Type::PIXEL);
		psos->createPSO(core, pso_name, shader_manager->shaders[vs_name].shader, shader_manager->shaders[ps_name].shader, VertexLayoutCache::getStatictLayoutInstanced());
	}

	void update(Matrix planeWorld, Matrix vp) {
		shader_manager->update(vs_name, "staticMeshBuffer", "W", &planeWorld);
		shader_manager->update(vs_name, "staticMeshBuffer", "VP", &vp);
	}

	void apply(Core* core)
	{
		unsigned int slot = 0;
		for (auto& vs : shader_manager->shaders[vs_name].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(3, vs.second.getGPUAddress());
			vs.second.next();
			slot++;
		}
		for (auto& ps : shader_manager->shaders[ps_name].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(1, ps.second.getGPUAddress());
			ps.second.next();
			slot++;
		}
	}

	void draw(Core* core) {
		core->beginRenderPass();
		apply(core);
		psos->bind(core, pso_name);
		for (int i = 0; i < meshes.size(); i++)
		{
			//shader_manager->updateTexturePS(core, "PixelShaderWithTransparence", "tex", textures->find(textureFilenames[i]));
			textures->updateTexturePS(core, textureFilenames[i]);
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

	std::string vs_name = "VS_Ani";
	std::string ps_name = "PS";
	std::string pso_name = "AnimationModel_PSO";

	std::vector<std::string> textureFilenames;
	Texture_Manager* textures;

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
			std::vector<ANIMATED_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++) {
				ANIMATED_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));
				vertices.push_back(v);
				hitbox.expand(v.pos);
			}

			std::string tex_root_alb = gemmeshes[i].material.find("albedo").getValue();
			std::string tex_root_nh = gemmeshes[i].material.find("nh").getValue();
			std::string tex_root_rmax = gemmeshes[i].material.find("rmax").getValue();
			//textureFilenames[i].push_back(tex_root_alb);

			//use the albedo texture name as the matarial name
			textureFilenames.push_back(tex_root_alb);
			std::vector<std::string> filenames;
			filenames.push_back(tex_root_alb);
			filenames.push_back(tex_root_nh);
			filenames.push_back(tex_root_rmax);
			textures->load(core, tex_root_alb, filenames);

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
	void init(Core* core, Shader_Manager* _shader_manager, PSOManager* _psos, Texture_Manager* _textures, std::string filename)
	{
		shader_manager = _shader_manager;
		psos = _psos;
		textures = _textures;
		name = filename;
		init_meshes(core, filename);
		shader_manager->load(core, vs_name, Shader_Type::VERTEX);
		shader_manager->load(core, ps_name, Shader_Type::PIXEL);
		psos->createPSO(core, pso_name, shader_manager->find(vs_name)->shader, shader_manager->find(ps_name)->shader, VertexLayoutCache::getAnimatedLayout());
		
		
	}

	void updateWorld( Matrix& w)
	{
		shader_manager->update(vs_name, "animatedMeshBuffer", "W", &w);
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
		shader_manager->update(vs_name, "animatedMeshBuffer", "W", &planeWorld);
		shader_manager->update(vs_name, "animatedMeshBuffer", "VP", &vp);
		shader_manager->update(vs_name, "animatedMeshBuffer", "bones", animation_instance.matrices);
		//hitbox.
	}

	void apply(Core* core)
	{
		unsigned int slot = 0;
		for (auto& vs : shader_manager->shaders[vs_name].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(0, vs.second.getGPUAddress());
			vs.second.next();
			//core->rootSignature.
			slot++;
		}
		for (auto& ps : shader_manager->shaders[ps_name].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(1, ps.second.getGPUAddress());
			ps.second.next();
			slot++;
		}
	}

	void draw(Core* core) {
		core->beginRenderPass();
		apply(core);
		psos->bind(core, pso_name);

		for (int i = 0; i < meshes.size(); i++)
		{
			//shader_manager->updateTexturePS(core, ps_name, "tex", textures->find(textureFilenames[i]));
			textures->updateTexturePS(core, textureFilenames[i]);
			meshes[i]->draw(core);
		}

	}
};