#pragma once
#include "model.h"
#include "camera.h"
#include "player.h"
#include <string>

class AnimalNPC : public NPC_Base
{
public:
	std::string name;
	Main_Charactor* target = nullptr;

	float vision_range = 500.0f;
	float attack_range = 180.0f;
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
