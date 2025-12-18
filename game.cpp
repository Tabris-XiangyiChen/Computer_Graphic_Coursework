#pragma once
#include "HeaderFiles/window.h"
//#include "core.h"
//#include "renderer.h"
#include "HeaderFiles/GamesEngineeringBase.h"
//#include "mesh.h"
#include "HeaderFiles/model.h"
#include "HeaderFiles/camera.h"
#include "HeaderFiles/player.h"
#include "HeaderFiles/npcs.h"
#include "HeaderFiles/map_item.h"
#include "HeaderFiles/ui.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);

	//create_matrix_files();

	Window win;
	Core core;
	win.create("My Window", WINDOW_WIDTH, WINDOW_HEIGHT, 100, 0);
	core.init(win.hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);

	GamesEngineeringBase::Timer timer;
	ConstantBuffer_Manager cm;
	cm.init_perframe_data();
	Shader_Manager sm;
	sm.init(&core, &cm);
	PSOManager psos;
	Texture_Manager tm;

	Object tree;
	tree.init(&core, &sm, &psos, &tm, "acacia");

	//Object_Instance flower;
	////Object flower;
	//flower.init(&core, &sm, &psos, &tm, "flower4", true);

	//Object_Animation trex;
	//trex.init(&core, &sm, &psos, &tm, "Trex");
	//Object_Animation fam;
	//fam.init(&core, &sm, &psos, &tm,"Farmer-male");

	//Plane plane;
	//plane.init(&core, &psos, &sm, &tm, "brown_mud_leaves_alb");
	Ground_Grid ground;
	ground.init(&core, &psos, &sm, &tm, "grass_path", FILE_NAME_GROUND_MATRIX, false, false);
	Sphere sphere;
	sphere.init(&core, &psos, &sm, &tm, "sunny_rose_garden");
	 
	UI_Manager um;
	UI_Number ui_num;
	ui_num.init_digits(&core, &sm, &psos, &tm, &um, -0.9, 0.9, 0.1, 0.15, 0.005);


	//Camera camera(Vec3(10, 5, 10), Vec3(0, 1, 0), Vec3(0, 1, 0));
	//camera.init((float)(WINDOW_WIDTH) / (float)(WINDOW_HEIGHT));
	Camera camera_;
	camera_.init_aspect((float)(WINDOW_WIDTH) , (float)(WINDOW_HEIGHT));
	//camera_.init_pos(Vec3(0, 0, 100), Vec3(0, 1, 0), Vec3(0, 0, 1));

	//Camera free;
	//free.init_aspect((float)(WINDOW_WIDTH), (float)(WINDOW_HEIGHT));
	//free.init_pos(Vec3(0, 0, -100), Vec3(0, 0, 1), Vec3(0, 1, 0));

	Main_Charactor farmer;
	farmer.init(&core, &sm, &psos, &tm, &camera_);

	AnimalNPC bull;
	bull.init_data();
	bull.init(&core, &sm, &psos, &tm, "Bull-dark");
	bull.set_target(&farmer);

	float time = 0;
	int fps = 0;
	//std::cout << "out" << std::endl;


	// items with hitbox
	Item_Ins_Base fence;
	fence.init(&core, &sm, &psos, &tm, "Fence_Wooden_Old_Full_26h", FILE_NAME_FENCE_MATRIX, false, true);
	Item_Ins_Base fence2;
	fence2.init(&core, &sm, &psos, &tm, "pine2", FILE_NAME_MAP_BOUNDRAY_MATRIX, false, true);
	Item_Ins_Base metal_fence;
	metal_fence.init(&core, &sm, &psos, &tm, "Fence_Metal_Full_14g", FILE_NAME_METAL_FENCE_MATRIX, false, true);

	// no hitbox items
	Item_Ins_Base flower;
	flower.init(&core, &sm, &psos, &tm, "flower4", FILE_NAME_FLOWER_MATRIX, true, false);
	Item_Ins_Base grass1;
	grass1.init(&core, &sm, &psos, &tm, "Grass_Sets_Full_01e", FILE_NAME_GRASS_SET_MATRIX, true, false);
	Item_Ins_Base grass2;
	grass2.init(&core, &sm, &psos, &tm, "Dead_Plants_01d", FILE_NAME_GRASS_DEAD_MATRIX, true, false);

	std::vector<NPC_Base*> npc_vec;
	npc_vec.push_back(&bull);
	std::vector<Item_Ins_Base*> item_vec;
	item_vec.push_back(&fence);
	item_vec.push_back(&fence2);
	item_vec.push_back(&metal_fence);


	while (1) {
		float dt = timer.dt();

		Matrix world;
		//world = Matrix::Scaling(Vec3(0.01f, 0.01f, 0.01f));
		//Matrix vp = p.mul(v);

		//std::cout << camera_.position.get_string() << std::endl;

		//free.update_free(&win, dt);
		time += dt;
		if (time > 1.0)
		{
			std::cout << camera_.position.get_string() << std::endl;
			fps = static_cast<int>(1 / dt);
			time = 0;
		}
			

		core.beginFrame();
		win.processMessages();

		core.beginRenderPass();

		cm.update_frameData(&core, dt, camera_.position);

		//plane.draw(&core, world, camera_.view_projection);
		sphere.draw(&core, world, camera_.view_projection);
		ground.draw(&core, camera_.view_projection);

		//tree.update(world, camera_.view_projection);
		tree.draw(&core, world, camera_.view_projection);

		//flower.update(world, camera_.view_projection);
		//flower.draw(&core, camera_.view_projection);

		//trex.update(world, camera_.view_projection, dt, "attack");
		//trex.draw(&core, world, camera_.view_projection, dt, "attack");

		//farmer.update(&core, &win, dt);
		farmer.draw(&core, &win, dt, npc_vec, item_vec);
		bull.draw(&core, camera_.view_projection, dt, item_vec);

		fence.draw(&core, camera_.view_projection);
		fence2.draw(&core, camera_.view_projection);
		metal_fence.draw(&core, camera_.view_projection);
		flower.draw(&core, camera_.view_projection);
		grass1.draw(&core, camera_.view_projection);
		grass2.draw(&core, camera_.view_projection);

		ui_num.draw_number(&core, fps);

		if (win.keys[VK_ESCAPE] == 1)
		{
			break;
		}

		core.finishFrame();
	}
	core.flushGraphicsQueue();

	FreeConsole();
}