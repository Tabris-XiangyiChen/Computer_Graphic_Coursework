#pragma once
#include "model.h"
#include "camera.h"
#include "player.h"
#include <string>

/*
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

	NPC_State move_state;
	NPC_State_Helper state_helper;

	float current_animation_speed = 1.0f;

	bool is_doing_action = false;
	float action_timer = 0.0f;

	virtual ~NPC_Base() = default;

	void init_data(Vec3 _pos = {0, 0, 500}, Vec3 _forward = {0, 0, -1}, float _speed = 100.0f, float _health = 20, float _attack = 5)
	{
		position = _pos;
		forward = _forward;
		speed = _speed;
		health = _health;
		attack = _attack;
	}

	virtual void init(Core* core, Shader_Manager* shader_manager, PSOManager* psos, Texture_Manager* textures, std::string model_name)
	{
		model.init(core, shader_manager, psos, textures, model_name);

		up = Vec3(0, 1, 0);
		right = up.Cross(forward).Normalize();
	}

	virtual void update(float dt) = 0;

	virtual void suffer_attack(float damage)
	{
		if (health > 0)
		{
			health -= damage;
		}
		is_dead = true;
	}

protected:

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
};*/

class AnimalNPC : public NPC_Base
{
public:
	std::string name;
	Main_Charactor* target = nullptr;

	float vision_range = 500.0f;
	float attack_range = 200.0f;
	Matrix model_adjust;
	float time;

	void init(Core* core, Shader_Manager* shader_manager, PSOManager* psos, Texture_Manager* textures, std::string model_name)
	{
		model.init(core, shader_manager, psos, textures, model_name);
		model.init_hitbox(core, shader_manager, psos, true);

		up = Vec3(0, 1, 0);
		right = up.Cross(forward).Normalize();
		model_adjust = Matrix::rotateY(M_PI);
	}

	void update_world_matrix()
	{
		if(is_be_holding)
		{
			Vec3 handle_pos = Vec3(target->position.x, target->position.y + 100.0f, target->position.z);
			Matrix T = Matrix::Translate(handle_pos);

			Matrix R;
			R.a[0][0] = target->right.x;   R.a[0][1] = target->right.y;   R.a[0][2] = target->right.z;
			R.a[1][0] = target->up.x;      R.a[1][1] = target->up.y;      R.a[1][2] = target->up.z;
			R.a[2][0] = target->forward.x; R.a[2][1] = target->forward.y; R.a[2][2] = target->forward.z;

			R = R.mul(Matrix::rotateY(-M_PI / 2));


			hitbox_world_matrix = T.mul(R).mul(model_adjust);
			world_matrix = T.mul(R).mul(model_adjust);
		}
		else
		{
			Matrix T = Matrix::Translate(position);

			Matrix R;
			R.a[0][0] = right.x;   R.a[0][1] = right.y;   R.a[0][2] = right.z;
			R.a[1][0] = up.x;      R.a[1][1] = up.y;      R.a[1][2] = up.z;
			R.a[2][0] = forward.x; R.a[2][1] = forward.y; R.a[2][2] = forward.z;

			//Matrix handle_m = Matrix::rotateZ(M_PI / 2);

			hitbox_world_matrix = T.mul(R).mul(model_adjust);
			world_matrix = T.mul(R).mul(model_adjust);
		}
	}

	Matrix return_next_world_matrix(Vec3& next_position)
	{
		Matrix T = Matrix::Translate(next_position);
		Matrix R;
		R.a[0][0] = right.x;   R.a[0][1] = right.y;   R.a[0][2] = right.z;
		R.a[1][0] = up.x;      R.a[1][1] = up.y;      R.a[1][2] = up.z;
		R.a[2][0] = forward.x; R.a[2][1] = forward.y; R.a[2][2] = forward.z;

		Matrix handle_m = Matrix::rotateZ(M_PI / 2);

		Matrix model_adjust = Matrix::rotateX(-M_PI / 2);


		return T.mul(R).mul(model_adjust);
		//world_matrix = T.mul(R).mul(model_adjust);
	}

	void update(float dt, std::vector<Item_Ins_Base*> item_vec)
	{

		update_ai(dt, item_vec);

		update_world_matrix();
	}

	void update_ai(float dt, std::vector<Item_Ins_Base*> item_vec)
	{
		if (!target) return;
		if (is_dead)
		{
			move_state = NPC_State::DEATH;
			if (death_ani_time >= 0)
			{
				death_ani_time -= dt;
			}
			else
				current_animation_speed = 0.0f;
			return;
		}

		float dist = (target->position - position).length();
		time += dt;
		if (time > 1.0)
		{
			std::cout << dist << std::endl;
			time = 0;
		}

		// set the is_doing_action back to the origin state
		update_action(dt);

		if (is_be_attacking)
		{
			be_attack();
			return;
		}

		if (dist < attack_range && AABB::AABB_intersect(model.hitbox.world_aabb, target->farmer.hitbox.world_aabb))
		{
			do_attack();
			return;
		}

		if (dist < vision_range)
		{
			chase_target(dt, item_vec);
			return;
		}

		standby(dt);
		
	}

	void suffer_attack(float damage)
	{
		if (health > 0 && !is_dead)
		{
			health -= damage;
		}
		if (health <= 0)
		{
			health = 0;
			is_dead = true;
		}
	}

	void draw(Core* core, Matrix& vp, float dt, std::vector<Item_Ins_Base*> item_vec)
	{
		update(dt, item_vec);
		float ani_dt = dt * current_animation_speed;
		model.draw(core, world_matrix, vp, ani_dt, state_helper[move_state]);

		model.hitbox.update_from_world(hitbox_world_matrix);
		model.hitbox.draw(core, hitbox_world_matrix, vp);

	}

	void set_target(Main_Charactor* _target)
	{
		target = _target;
	}
private:
	void chase_target(float dt, std::vector<Item_Ins_Base*> item_vec)
	{
		if (is_doing_action) return;

		//is_doing_action = false;
		move_state = NPC_State::RUN_FORWARD;

		Vec3 move_dir = target->position - position;
		move_dir.y = 0;

		if (move_dir.length() < 0.001f)
			return;

		move_dir = move_dir.Normalize();

		// caculate trun angle
		Vec3 current_forward = forward;
		Vec3 target_forward = move_dir;

		float angle = signed_angle_y(current_forward, target_forward);

		float max_turn = turn_speed * dt;
		angle = clamp(angle, -max_turn, max_turn);

		forward = Matrix::rotateY(angle).mulVec(forward).Normalize();
		right = up.Cross(forward).Normalize();

		Vec3 next_pos = position + move_dir * speed * dt;
		AABB next_aabb = model.hitbox.local_aabb;
		next_aabb = next_aabb.transform(return_next_world_matrix(next_pos));

		if (!check_map_collision(next_aabb, item_vec))
		{
			// make it slow down when turning
			if (fabs(angle) < 0.2f)
			{
				position += move_dir * speed * dt;
			}
		}

	}

	void do_attack()
	{
		if (is_doing_action) return;

		is_doing_action = true;
		action_timer = 1.2f;
		move_state = NPC_State::ATTACK_01;
	}

	void standby(float dt)
	{
		if (is_doing_action) return;

		move_state = NPC_State::IDLE;
		current_animation_speed = 1.0f;
	}

	void be_attack()
	{

		is_doing_action = true;
		is_be_attacking = false;
		action_timer = 0.5f;
		current_animation_speed = 1.0f;
		move_state = NPC_State::HITREACT;
	}
};
