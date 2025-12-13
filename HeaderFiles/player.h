#pragma once
#include "model.h"
#include "camera.h"
#include <string>

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
	IDLE_WHELL_BARROW,
	JUMP,
	LAND,
	RUN,
	WALK_CARRY,
	WALK_INPLACE,
	WALK_WHELLBARROW,
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
		"idle whell barrow",  // IDLE_WHELL_BARROW
		"jump",               // JUMP
		"land",               // LAND
		"run",                // RUN
		"walk carry",         // WALK_CARRY
		"walk inplace",       // WALK_INPLACE
		"walk whellbarrow",   // WALK_WHELLBARROW
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
	float turn_speed = 5.0f;  // 转向速度

	//bool is_moving = false;
	//bool is_running = false;
	Charactor_State move_state;
	Charactor_State_Helper move_state_helper;
	float current_animation_speed = 1.0f;

	void init(Core* core, Shader_Manager* shader_manager, PSOManager* psos,Texture_Manager* textures, Camera* cam)
	{
		farmer.init(core, shader_manager, psos, textures, name);
		//std::cout << farmer.hitbox.getMax().get_string() << farmer.hitbox.getMin().get_string() << std::endl;
		
		// 调试信息
		//Vec3 min = farmer.hitbox.getMin();
		//Vec3 max = farmer.hitbox.getMax();
		//Vec3 center = farmer.hitbox.get_center();
		//Vec3 size = max - min;

		//std::cout << "Model bounding box:" << std::endl;
		//std::cout << "  Min: " << min.get_string() << std::endl;
		//std::cout << "  Max: " << max.get_string() << std::endl;
		//std::cout << "  Center: " << center.get_string() << std::endl;
		//std::cout << "  Size: " << size.get_string() << std::endl;

		//float ground_offset = -min.z;  // 应该是 18.77

		//float x_offset = -center.x;
		//float y_offset = -center.y;

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

	//void get_world_matrix()
	//{
	//	Matrix T = Matrix::Translate(position);

	//	Vec3 world_up = Vec3(0, 0, 1);

	//	Vec3 right = world_up.Cross(forward).Normalize();
	//	Vec3 up = forward.Cross(right).Normalize();

	//	Matrix R;
	//	R.a[0][0] = right.x;   R.a[0][1] = right.y;   R.a[0][2] = right.z;
	//	R.a[1][0] = up.x;      R.a[1][1] = up.y;      R.a[1][2] = up.z;
	//	R.a[2][0] = forward.x; R.a[2][1] = forward.y; R.a[2][2] = forward.z;

	//	world_matrix =  T.mul(R).Rotate_Y(M_PI / 2);
	//}

	void update_world_matrix()
	{
		// 位置变换
		Matrix T = Matrix::Translate(position);

		// 创建朝向矩阵
		Matrix R;
		R.a[0][0] = right.x;   R.a[0][1] = right.y;   R.a[0][2] = right.z;
		R.a[1][0] = up.x;      R.a[1][1] = up.y;      R.a[1][2] = up.z;
		R.a[2][0] = forward.x; R.a[2][1] = forward.y; R.a[2][2] = forward.z;

		// 由于你的模型可能需要旋转才能正确显示，根据实际情况调整
		// 这里假设模型需要绕X轴旋转90度
		Matrix model_adjust = Matrix::rotateX(-M_PI / 2);
		Matrix zm = Matrix::rotateZ(M_PI);

		//world_matrix = T.mul(R);
		//world_matrix = T.mul(R).mul(model_adjust);
		hitbox_world_matrix = T.mul(R);
		world_matrix = T.mul(R).mul(model_adjust).mul(zm);
		//world_matrix = T.mul(R).Rotate_Z(M_PI / 2);
		//world_matrix = T;
	}


	void update(Core* core, Window* wnd, float dt)
	{
		update_movement(wnd, dt);
		//std::cout << position.get_string() << std::endl;

		// 调试：显示相机信息
		//static int frame = 0;
		//if (frame < 10)  // 只显示前10帧
		//{
		//	std::cout << "Frame " << frame << ":" << std::endl;
		//	std::cout << "  Camera pos: " << camera->position.get_string() << std::endl;
		//	std::cout << "  Camera target: " << camera->target.get_string() << std::endl;
		//	std::cout << "  Camera forward: " << camera->forward.get_string() << std::endl;

		//	// 计算模型的实际世界位置
		//	Vec3 world_min = position + farmer.hitbox.getMin();
		//	Vec3 world_max = position + farmer.hitbox.getMax();
		//	Vec3 world_center = (world_min + world_max) * 0.5f;
		//	std::cout << "  Model world center: " << world_center.get_string() << std::endl;
		//	std::cout << "  Model feet (min Z): " << world_min.z << std::endl;

		//	frame++;
		//}

		third_cam.update(wnd, dt);

		//farmer.update(world_matrix, camera->view_projection, dt, move_state_helper[move_state]);
	}

	void update_movement(Window* window, float dt)
	{
		float s = speed * dt;
		Vec3 move_dir(0, 0, 0);
		move_state = Charactor_State::IDLE_BASIC_01;

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

		// 如果有移动输入
		if (is_moving)
		{
			if (is_running)
			{
				move_state = Charactor_State::RUN;
				speed = 200.0f;
			}
			else
			{
				move_state = Charactor_State::WALK;
				speed = 100.0f;
			}
			move_dir = move_dir.Normalize();

			//// 平滑转向：让角色逐渐转向移动方向
			//Vec3 target_forward = move_dir;
			//target_forward.y = 0;  // 保持在地面平面
			//if (target_forward.length() > 0.001f)
			//{
			//	target_forward = target_forward.Normalize();

			//	// 平滑插值当前朝向到目标朝向
			//	float turn_amount = turn_speed * dt;
			//	forward = lerp(forward, target_forward, turn_amount).Normalize();

			//	// 重新计算右方向和上方向
			//	right = up.Cross(forward).Normalize();
			//}

			Vec3 current_forward = forward;
			Vec3 target_forward = move_dir;
			target_forward.y = 0;
			target_forward = target_forward.Normalize();

			float angle = signed_angle_y(current_forward, target_forward);

			float max_turn = turn_speed * dt;
			angle = clamp(angle, -max_turn, max_turn);

			// 绕 Y 轴旋转 forward
			forward = Matrix::rotateY(angle).mulVec(forward).Normalize();
			right = up.Cross(forward).Normalize();

			// 应用移动
			position = position + move_dir * s;

			// 根据移动速度调整动画速度
			current_animation_speed = move_dir.length() * 2.0f;
		}
		else
		{
			current_animation_speed = 1.0f;
			move_state = Charactor_State::IDLE_BASIC_01;
		}

		// 更新世界矩阵
		update_world_matrix();
	}

	void draw(Core* core, Window* wnd, float dt)
	{
		update(core, wnd, dt);
		farmer.draw(core, world_matrix, camera->view_projection, dt, move_state_helper[move_state]);
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