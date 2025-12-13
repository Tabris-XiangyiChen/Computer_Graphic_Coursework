#pragma once
#include "core.h"
#include "vectors.h"

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
			Vec3 transformed = transform.mulPoint(vertex);
			result.expand(transformed);
		}

		return result;
	}

	//static bool intersect(const AABB& a, const AABB& b)
	//{
	//	return (a.m_min <= b.max.x && a.max.x >= b.min.x) &&
	//		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
	//		(a.min.z <= b.max.z && a.max.z >= b.min.z);
	//}
};


class HitBox
{
public:
	AABB local_aabb;
	AABB world_aabb;

	Mesh mesh;
	Shader_Manager* shader_manager = nullptr;
	PSOManager* psos = nullptr;

	std::string vs_name = "VS_Line";
	std::string ps_name = "PS_Line";
	std::string pso_name = "Line_PSO";

public:
	// 初始化（一次）
	void init(Core* core,
		Shader_Manager* _shader_manager,
		PSOManager* _psos,
		const AABB& aabb)
	{
		shader_manager = _shader_manager;
		psos = _psos;
		local_aabb = aabb;

		// 先创建一个“空 mesh”
		build_mesh_from_aabb(core, local_aabb);

		shader_manager->load(core, vs_name, Shader_Type::VERTEX);
		shader_manager->load(core, ps_name, Shader_Type::PIXEL);

		psos->createPSO_Line(
			core,
			pso_name,
			shader_manager->shaders[vs_name].shader,
			shader_manager->shaders[ps_name].shader,
			VertexLayoutCache::getStaticLineLayout()
		);
	}

	void build_mesh_from_aabb(Core* core, const AABB& aabb)
	{
		std::array<Vec3, 8> v = aabb.get_vertices();

		static const int edges[24] =
		{
			0,1, 1,3, 3,2, 2,0,
			4,5, 5,7, 7,6, 6,4,
			0,4, 1,5, 2,6, 3,7
		};

		std::vector<LINE_VERTEX> vertices;
		std::vector<unsigned int> indices;

		for (int i = 0; i < 24; ++i)
		{
			vertices.push_back({ v[edges[i]] });
			indices.push_back(i);
		}

		mesh.init_line(core, vertices, indices);
	}

	void update_from_world(const Matrix& world)
	{
		world_aabb = local_aabb.transform(world);
		world_aabb.update_cache();
	}

	void update(Matrix& world, Matrix& vp)
	{
		shader_manager->update(vs_name, "staticMeshBuffer", "W", &world);
		shader_manager->update(vs_name, "staticMeshBuffer", "VP", &vp);
	}

	void draw(Core* core, Matrix& world, Matrix& vp)
	{
		update_from_world(world);
		update(world, vp);

		core->beginRenderPass();

		for (auto& cb : shader_manager->shaders[vs_name].constantBuffers)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(
				3, cb.second.getGPUAddress());
			cb.second.next();
		}

		psos->bind(core, pso_name);

		mesh.draw_line(core);
	}
};