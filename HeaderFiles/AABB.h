#pragma once
#include "core.h"
#include "vectors.h"

struct CollisionResult
{
	bool hit = false;
	Vec3 normal; 
	float depth = 0;
};


class AABB {
public:
	Vec3 m_min;
	Vec3 m_max;
	Vec3 m_center;
	Vec3 m_halfSize; 
	bool m_dirty; 
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

	//will change the self value
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

	void translate(const Vec3& delta)
	{
		m_min += delta;
		m_max += delta;
		update_cache();
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

	inline static bool AABB_intersect(const AABB& a, const AABB& b)
	{
		// is it separated on the x-axis?
		if (a.m_max.x < b.m_min.x || a.m_min.x > b.m_max.x)
			return false;

		if (a.m_max.y < b.m_min.y || a.m_min.y > b.m_max.y)
			return false;

		if (a.m_max.z < b.m_min.z || a.m_min.z > b.m_max.z)
			return false;

		return true;
	}

	inline bool intersects_toother(const AABB& other) const
	{
		return !(m_max.x < other.m_min.x || m_min.x > other.m_max.x ||
			m_max.y < other.m_min.y || m_min.y > other.m_max.y ||
			m_max.z < other.m_min.z || m_min.z > other.m_max.z);
	}

	inline static bool point_in_AABB(const Vec3& p, const AABB& box)
	{
		return (p.x >= box.m_min.x && p.x <= box.m_max.x) &&
			(p.y >= box.m_min.y && p.y <= box.m_max.y) &&
			(p.z >= box.m_min.z && p.z <= box.m_max.z);
	}

	static CollisionResult AABB_intersect_depth(const AABB& a, const AABB& b)
	{
		CollisionResult result;

		Vec3 delta = b.m_center - a.m_center;

		float overlapX = a.m_halfSize.x + b.m_halfSize.x - std::abs(delta.x);
		if (overlapX <= 0) return result;

		float overlapY = a.m_halfSize.y + b.m_halfSize.y - std::abs(delta.y);
		if (overlapY <= 0) return result;

		float overlapZ = a.m_halfSize.z + b.m_halfSize.z - std::abs(delta.z);
		if (overlapZ <= 0) return result;

		result.hit = true;
		result.depth = overlapX;
		result.normal = Vec3((delta.x > 0) ? -1.f : 1.f, 0, 0);

		if (overlapY < result.depth)
		{
			result.depth = overlapY;
			result.normal = Vec3(0, (delta.y > 0) ? -1.f : 1.f, 0);
		}

		if (overlapZ < result.depth)
		{
			result.depth = overlapZ;
			result.normal = Vec3(0, 0, (delta.z > 0) ? -1.f : 1.f);
		}

		return result;
	}


	static bool ray_intersect_AABB(
		const Vec3& rayOrigin,
		const Vec3& rayDir,
		const AABB& box,
		float& tMinOut)
	{
		float tMin = 0.0f;
		float tMax = FLT_MAX;

		for (unsigned int i = 0; i < 3; i++)
		{
			float origin = rayOrigin.v[i];
			float dir = rayDir.v[i];
			float minB = box.m_min.v[i];
			float maxB = box.m_max.v[i];

			if (std::abs(dir) < 1e-6f)
			{
				if (origin < minB || origin > maxB)
					return false;
			}
			else
			{
				float ood = 1.0f / dir;
				float t1 = (minB - origin) * ood;
				float t2 = (maxB - origin) * ood;

				if (t1 > t2) std::swap(t1, t2);
				tMin = max(tMin, t1);
				tMax = min(tMax, t2);

				if (tMin > tMax)
					return false;
			}
		}

		tMinOut = tMin;
		return true;
	}

};


class HitBox
{
public:
	AABB local_aabb;
	AABB world_aabb;

	bool ifdraw;
	Mesh mesh;
	Shader_Manager* shader_manager = nullptr;
	PSOManager* psos = nullptr;

	std::string vs_name = "VS_Line";
	std::string ps_name = "PS_Line";
	std::string pso_name = "Line_PSO";

public:

	void init_withmesh(Core* core, Shader_Manager* _shader_manager, PSOManager* _psos, const AABB& aabb)
	{
		ifdraw = true;
		shader_manager = _shader_manager;
		psos = _psos;
		local_aabb = aabb;

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
	void init(Core* core, const AABB& aabb)
	{
		ifdraw = false;
		local_aabb = aabb;

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
		if (!ifdraw)
			return;

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