#pragma once
#include "model.h"
#include "camera.h"
#include <string>
#include "map_item.h"

enum class NPC_State
{
	ATTACK_01 = 0,
	ATTACK_02,
	DANCE,
	DEATH,
	EATING,
	HITREACT,
	IDLE_SITTING,
	IDLE_VARIATION,
	IDLE,
	RUN_FORWARD,
	SIT_TO_STAND,
	STAND_TO_SIT,
	TROT_FORWARD,
	TURN_90_L,
	TURN_90_R,
	WALK_BACKFORWARDS,
	WALK_FORWARD,

	MAX_TYPE
};

class NPC_State_Helper
{
public:
	const std::array<std::string, static_cast<size_t>(NPC_State::MAX_TYPE)> state_names = {
		"attack01",           // ATTACK_01 = 0
		"attack02",           // ATTACK_02 = 1
		"dance",               // DANCE = 2
		"death",               // DEATH = 3
		"eating",              // EATING = 4
		"hitreact",            // HITREACT = 5
		"idle sitting",        // IDLE_SITTING = 6
		"idle variation",      // IDLE_VARIATION = 7
		"idle",                // IDLE = 8
		"run forward",         // RUN_FORWARD = 9
		"sit to stand",        // SIT_TO_STAND = 10
		"stand to sit",        // STAND_TO_SIT = 11
		"trot forward",        // TROT_FORWARD = 12
		"turn 90 l",           // TURN_90_L = 13
		"turn 90 r",           // TURN_90_R = 14
		"walk backwards",      // WALK_BACKFORWARDS = 15
		"walk forward",        // WALK_FORWARD = 16
	};

	inline std::string to_string(NPC_State state) {
		size_t index = static_cast<size_t>(state);
		if (index < state_names.size()) {
			return state_names[index];
		}
		return "idle";
	}

	inline std::string operator[] (NPC_State state) {
		size_t index = static_cast<size_t>(state);
		if (index < state_names.size()) {
			return state_names[index];
		}
		return "idle";
	}

	inline NPC_State from_string(const std::string& name) {
		for (size_t i = 0; i < state_names.size(); ++i) {
			if (state_names[i] == name) {
				return static_cast<NPC_State>(i);
			}
		}
		return NPC_State::IDLE;
	}
};


class NPC_Base
{
public:
	Object_Animation model;

	Vec3 position;
	Vec3 forward;
	Vec3 up;
	Vec3 right;

	Matrix world_matrix;
	Matrix hitbox_world_matrix;

	float speed = 100.0f;
	float turn_speed = 5.0f;

	float health;
	float attack;
	bool is_dead = false;
	float death_time = 0;
	float death_ani_time = 2.0f;

	NPC_State move_state;
	NPC_State_Helper state_helper;

	float current_animation_speed = 1.0f;

	bool is_doing_action = false;
	float action_timer = 0.0f;

	bool is_be_holding = false;
	bool is_be_attacking = false;

	virtual ~NPC_Base() = default;

	void init_data(Vec3 _pos = { 0, 0, 500 }, Vec3 _forward = { 0, 0, -1 }, float _speed = 100.0f, float _health = 20, float _attack = 5)
	{
		position = _pos;
		forward = _forward;
		speed = _speed;
		health = _health;
		attack = _attack;
	}
	void reset_pos_data(Vec3 _pos, Vec3 _forward, Vec3 _right)
	{
		position = _pos;
		forward = _forward;
		right = _right;
	}

	virtual void init(Core* core, Shader_Manager* shader_manager, PSOManager* psos, Texture_Manager* textures, std::string model_name)
	{
		model.init(core, shader_manager, psos, textures, model_name);

		up = Vec3(0, 1, 0);
		right = up.Cross(forward).Normalize();
	}

	virtual void update(float dt, std::vector<Item_Ins_Base*> item_vec) = 0;

	virtual void suffer_attack(float damage)
	{
		if (health > 0 && !is_dead)
		{
			health -= damage;
		}
		if(health <= 0)
		{
			health = 0;
			is_dead = true;
		}
	}

	bool check_map_collision(const AABB& next_aabb, const std::vector<Item_Ins_Base*>& items)
	{
		for (auto* item : items)
		{
			if (item->collide(next_aabb))
				return true;
		}
		return false;
	}

protected:
	//reset the action state
	void update_action(float dt)
	{
		if (!is_doing_action) return;

		action_timer -= dt;
		if (action_timer <= 0.0f)
		{
			is_doing_action = false;
			current_animation_speed = 1.0f;
		}
	}

	float signed_angle_y(const Vec3& from, const Vec3& to)
	{
		Vec3 f = from.Normalize();
		Vec3 t = to.Normalize();

		float dot = clamp(f.Dot(t), -1.0f, 1.0f);
		float angle = acosf(dot); // 0 ~ PI

		Vec3 cross = f.Cross(t);
		if (cross.y < 0)
			angle = -angle;

		return angle; // 弧度
	}
};


enum class Charactor_State
{
	ATTACK_A = 0,
	ATTACK_B,
	DEATH,
	FALL_LOOP,
	GRAB_LOW,
	GRAB_MEDIUM,
	HIT_REACTION,
	IDLE_BASIC_01,
	IDLE_BASIC_02,
	IDLE_COMBAT,
	IDLE_SIT_CHAIR,
	IDLE_WHEELBARROW,
	JUMP,
	LAND,
	RUN,
	WALK_CARRY,
	WALK_INPLACE,
	WALK_WHEELBARROW,
	WALK,
	WAVING_L_HAND,
	WAVING_R_HAND,

	MAX_TYPE
};

class Charactor_State_Helper
{
public:
	const std::array<std::string, static_cast<size_t>(Charactor_State::MAX_TYPE)> state_names = {
		"attack a",           // ATTACK_A
		"attack b",           // ATTACK_B
		"death",              // DEATH
		"fall loop",          // FALL_LOOP
		"grab low",           // GRAB_LOW
		"grab medium",        // GRAB_MEDIUM
		"hit reaction",       // HIT_REACTION
		"idle basic 01",      // IDLE_BASIC_01
		"idle basic 02",      // IDLE_BASIC_02
		"idle combat",        // IDLE_COMBAT
		"idle sit chair",     // IDLE_SIT_CHAIR
		"idle wheelbarrow",   // IDLE_WHELL_BARROW
		"jump",               // JUMP
		"land",               // LAND
		"run",                // RUN
		"walk carry",         // WALK_CARRY
		"walk inplace",       // WALK_INPLACE
		"walk wheelbarrow",   // WALK_WHELLBARROW
		"walk",               // WALK
		"waving l hand",      // WAVING_L_HAND
		"waving r hand",      // WAVING_R_HAND
	};

	inline std::string to_string(Charactor_State state) {
		size_t index = static_cast<size_t>(state);
		if (index < state_names.size()) {
			return state_names[index];
		}
		return "idle basic 01";  // 默认值
	}

	inline std::string operator[] (Charactor_State state) {
		size_t index = static_cast<size_t>(state);
		if (index < state_names.size()) {
			return state_names[index];
		}
		return "idle basic 01";  // 默认值
	}

	inline Charactor_State from_string(const std::string & name) {
		for (size_t i = 0; i < state_names.size(); ++i) {
			if (state_names[i] == name) {
				return static_cast<Charactor_State>(i);
			}
		}
		return Charactor_State::IDLE_BASIC_01;
	}
};

class Main_Charactor
{
public:
	std::string name = "Farmer-male";
	Object_Animation farmer;

	Camera* camera;
	ThirdPersonCameraController third_cam;

	Vec3 position;
	Vec3 forward;
	Vec3 up;
	Vec3 right;
	Matrix world_matrix;
	Matrix hitbox_world_matrix;

	float speed = 100;
	float turn_speed = 5.0f;
	float attack = 10.0f;

	//bool is_moving = false;
	//bool is_running = false;
	Charactor_State move_state;
	Charactor_State_Helper move_state_helper;
	float current_animation_speed = 1.0f;

	bool is_carrying = false;
	NPC_Base* carrying_item = nullptr;
	bool is_doing_action = false;   // is attacking / grabing
	float action_timer = 0.0f;

	Object_Animation* carrying_object;

	void init(Core* core, Shader_Manager* shader_manager, PSOManager* psos,Texture_Manager* textures, Camera* cam)
	{
		farmer.init(core, shader_manager, psos, textures, name);
		//std::cout << farmer.hitbox.local_aabb.getMax().get_string() << farmer.hitbox.local_aabb.getMin().get_string() << std::endl;
		farmer.hitbox.local_aabb.m_min.x = -40.0f;
		farmer.hitbox.local_aabb.m_max.x = 40.0f;
		farmer.init_hitbox(core, shader_manager, psos, true);

		// 设置位置，使模型站在原点，面朝Y轴正方向
		//position = Vec3(x_offset, y_offset, ground_offset);
		position = Vec3(0, 0, 0);

		// 初始化朝向
		forward = Vec3(0, 0, 1);  // 初始面朝Z轴正方向
		up = Vec3(0, 1, 0);       // Y轴向上
		right = up.Cross(forward).Normalize();
		//std::cout << "Adjusted position: " << position.get_string() << std::endl;

		camera = cam;

		//camera->init_pos(Vec3(0, -100, 0), Vec3(0, 1, 0), Vec3(0, 0, 1));

		third_cam.init(camera, &position);


	}

	void update_world_matrix()
	{
		Matrix T = Matrix::Translate(position);
		//rotate matrix
		Matrix R;
		R.a[0][0] = right.x;   R.a[0][1] = right.y;   R.a[0][2] = right.z;
		R.a[1][0] = up.x;      R.a[1][1] = up.y;      R.a[1][2] = up.z;
		R.a[2][0] = forward.x; R.a[2][1] = forward.y; R.a[2][2] = forward.z;

		Matrix model_adjust = Matrix::rotateX(-M_PI / 2);
		Matrix zm = Matrix::rotateZ(M_PI);

		//world_matrix = T.mul(R);
		//world_matrix = T.mul(R).mul(model_adjust);
		hitbox_world_matrix = T.mul(R);
		world_matrix = T.mul(R).mul(model_adjust).mul(zm);
		//world_matrix = T.mul(R).Rotate_Z(M_PI / 2);
		//world_matrix = T;
	}

	Matrix return_next_world_matrix(Vec3& next_position)
	{
		Matrix T = Matrix::Translate(next_position);
		Matrix R;
		R.a[0][0] = right.x;   R.a[0][1] = right.y;   R.a[0][2] = right.z;
		R.a[1][0] = up.x;      R.a[1][1] = up.y;      R.a[1][2] = up.z;
		R.a[2][0] = forward.x; R.a[2][1] = forward.y; R.a[2][2] = forward.z;

		Matrix model_adjust = Matrix::rotateX(-M_PI / 2);
		model_adjust = model_adjust.mul(Matrix::rotateZ(M_PI));

		return T.mul(R);
		//world_matrix = T.mul(R).mul(model_adjust);
	}


	void update(Core* core, Window* wnd, float dt, std::vector<NPC_Base*>& npcs, std::vector<Item_Ins_Base*> item_vec)
	{
		//handle_action_input(wnd);

		handle_attack_input(wnd, npcs);
		handle_pickup_input(wnd, npcs);

		update_action(dt);
		
		// movement is only permitted when no action is being performed.
		if (!is_doing_action)
		{
			update_movement(wnd, dt, item_vec);
		}


		third_cam.update(wnd, dt);
	}

	bool check_map_collision( const AABB& next_aabb, const std::vector<Item_Ins_Base*>& items)
	{
		for (auto* item : items)
		{
			if (item->collide(next_aabb))
				return true;
		}
		return false;
	}
	
	//try to not stop when collide
	Vec3 try_move_with_slide( Vec3 position, Vec3& delta, AABB& localAABB, std::vector<Item_Ins_Base*>& items)
	{
		Vec3 final_delta = delta;

		AABB worldAABB = localAABB;
		//worldAABB = worldAABB.transform(Matrix::Translate(position));

		//x forward, if not 0
		if (final_delta.x != 0)
		{
			AABB test = worldAABB;
			// try move
			test.translate(Vec3(final_delta.x, 0, 0));

			// try move
			if (Item_Ins_Base::collide_with_map(test, items))
				final_delta.x = 0;
		}

		// z
		if (final_delta.z != 0)
		{
			AABB test = worldAABB;
			test.translate(Vec3(0, 0, final_delta.z));

			if (Item_Ins_Base::collide_with_map(test, items))
				final_delta.z = 0;
		}

		Vec3 new_pos = position + final_delta;

		return new_pos;
	}


	void update_movement(Window* window, float dt, std::vector<Item_Ins_Base*> item_vec)
	{
		float s = speed * dt;
		Vec3 move_dir(0, 0, 0);
		move_state = is_carrying ? Charactor_State::IDLE_WHEELBARROW : Charactor_State::IDLE_BASIC_01;

		// 获取相机在地面平面上的方向
		Vec3 cam_forward_flat = camera->get_forward_flat();
		Vec3 cam_right_flat = camera->get_right_flat();

		// 基于相机方向的移动输入
		if (window->keys['W'])
		{
			move_dir = move_dir + cam_forward_flat;
		}
		if (window->keys['S'])
		{
			move_dir = move_dir - cam_forward_flat;
		}
		if (window->keys['A'])
		{
			move_dir = move_dir - cam_right_flat;
		}
		if (window->keys['D'])
		{
			move_dir = move_dir + cam_right_flat;
		}


		bool is_moving = move_dir.length() > 0.001f;
		bool is_running = window->keys[VK_SHIFT];

		// if move
		if (is_moving)
		{
			if (is_running && !is_carrying)
			{
				move_state = Charactor_State::RUN;
				speed = 200.0f;
			}
			else
			{
				move_state = is_carrying
					? Charactor_State::WALK_CARRY
					: Charactor_State::WALK;

				speed = is_carrying ? 70.0f : 100.0f;
			}
			move_dir = move_dir.Normalize();

			Vec3 current_forward = forward;
			Vec3 target_forward = move_dir;
			target_forward.y = 0;
			target_forward = target_forward.Normalize();

			float angle = signed_angle_y(current_forward, target_forward);

			float max_turn = turn_speed * dt;
			angle = clamp(angle, -max_turn, max_turn);

			// rotate by Y
			forward = Matrix::rotateY(angle).mulVec(forward).Normalize();
			right = up.Cross(forward).Normalize();

			Vec3 next_pos =  position + move_dir * s;

			AABB next_aabb = farmer.hitbox.local_aabb;
			Matrix next_world = return_next_world_matrix(next_pos);
			next_aabb = next_aabb.transform(next_world);

			bool blocked = check_map_collision(next_aabb, item_vec);

			if (!blocked)
			{
				//Vec3 delta = move_dir * s;
				//position = try_move_with_slide(position, delta, farmer.hitbox.world_aabb, item_vec);

				position = next_pos;
			}
			//current_animation_speed = move_dir.length() * 2.0f;
		}
		else
		{
			//current_animation_speed = 1.0f;
			move_state = is_carrying? Charactor_State::IDLE_WHEELBARROW : Charactor_State::IDLE_BASIC_01;
			//move_state = Charactor_State::IDLE_BASIC_01;
		}

		// 更新世界矩阵
		update_world_matrix();
	}

	void update_action(float dt)
	{
		if (!is_doing_action)
			return;

		action_timer -= dt;
		if (action_timer <= 0.0f)
		{
			is_doing_action = false;
			current_animation_speed = 1.0f;

			//state after carrying
			if (is_carrying)
				move_state = Charactor_State::IDLE_WHEELBARROW;
			else
				move_state = Charactor_State::IDLE_BASIC_01;
		}
	}

	void handle_action_input(Window* window)
	{
		if (is_doing_action)
			return;

		if (window->mouseButtons[0])
		{
			is_doing_action = true;
			action_timer = 1.6f;

			current_animation_speed = 2.0f;

			move_state = Charactor_State::ATTACK_A;
			return;
		}

		if (window->keys['E'] && !is_carrying)
		{
			is_doing_action = true;
			action_timer = 2.5f;

			current_animation_speed = 2.0f;

			move_state = Charactor_State::GRAB_LOW;
			is_carrying = true;
			return;
		}
	}

	AABB make_attack_aabb(float range = 60.0f)
	{
		AABB atk;
		Vec3 center = farmer.hitbox.world_aabb.get_center();

		Vec3 forward_offset = forward * range;

		atk.m_min = center + forward_offset - Vec3(30, 30, 30);
		atk.m_max = center + forward_offset + Vec3(30, 30, 30);
		atk.update_cache();

		return atk;
	}

	bool try_attack(const std::vector<NPC_Base*>& enemies)
	{
		AABB attack_box = make_attack_aabb();

		for (NPC_Base* enemy : enemies)
		{
			if (AABB::AABB_Intersect(attack_box, enemy->model.hitbox.world_aabb))
			{
				enemy->suffer_attack(attack);
				enemy->is_be_attacking = true;
				return true;
			}
		}
		return false;
	}

	void handle_attack_input(Window* window, const std::vector<NPC_Base*>& enemies)
	{
		if (is_doing_action)
			return;

		if (window->mouseButtons[0] && !is_carrying)
		{
			is_doing_action = true;
			action_timer = 1.0f;

			current_animation_speed = 2.0f;
			move_state = Charactor_State::ATTACK_A;

			try_attack(enemies);
		}
	}


	bool try_pickup(const std::vector<NPC_Base*>& items)
	{
		AABB pickup_box = farmer.hitbox.world_aabb;

		for (NPC_Base* item : items)
		{
			if (AABB::AABB_Intersect(pickup_box, item->model.hitbox.world_aabb) && item->is_dead)
			{
				// TODO: 绑定物体到角色（先只标记）
				item->is_be_holding = true;
				carrying_item = item;
				return true;
			}
		}
		return false;
	}

	void handle_pickup_input(Window* window, const std::vector<NPC_Base*>& items)
	{
		static bool lastE = false;
		bool nowE = window->keys['E'];

		if (nowE && !lastE && !is_doing_action)
		{
			// 当前没拿东西 → 尝试拾取
			if (!is_carrying)
			{
				is_doing_action = true;
				action_timer = 1.0f;

				current_animation_speed = 2.0f;

				move_state = Charactor_State::GRAB_LOW;
				if (try_pickup(items))
				{
					is_carrying = true;
				}
			}
			// 当前拿着 → 放下
			else
			{
				is_doing_action = true;
				action_timer = 0.5f;

				move_state = Charactor_State::GRAB_LOW;
				is_carrying = false;

				if(carrying_item != nullptr)
				{
					carrying_item->is_be_holding = false;
					carrying_item->reset_pos_data(position, forward.rotateY(-M_PI / 2), right.rotateY(-M_PI / 2));
					carrying_item = nullptr;
				}
			}
		}

		lastE = nowE;
	}


	void draw(Core* core, Window* wnd, float dt, std::vector<NPC_Base*>& npcs, std::vector<Item_Ins_Base*> items)
	{
		update(core, wnd, dt, npcs, items);
		float ani_dt =  dt * current_animation_speed;
		farmer.draw(core, world_matrix, camera->view_projection, ani_dt, move_state_helper[move_state]);
		// update hitbox pos
		farmer.hitbox.update_from_world(hitbox_world_matrix);
		farmer.hitbox.draw(core, hitbox_world_matrix, camera->view_projection);
	}

	private:
		float signed_angle_y(const Vec3& from, const Vec3& to)
		{
			Vec3 f = from.Normalize();
			Vec3 t = to.Normalize();

			float dot = clamp(f.Dot(t), -1.0f, 1.0f);
			float angle = acosf(dot); // 0 ~ PI

			Vec3 cross = f.Cross(t);
			if (cross.y < 0)
				angle = -angle;

			return angle; // 弧度
		}
};