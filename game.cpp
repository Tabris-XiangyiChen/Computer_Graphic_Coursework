#pragma once
#include "HeaderFiles/window.h"
//#include "core.h"
//#include "renderer.h"
#include "HeaderFiles/GamesEngineeringBase.h"
//#include "mesh.h"
#include "HeaderFiles/model.h"
#include "HeaderFiles/camera.h"
#include "HeaderFiles/player.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);

	Window win;
	Core core;
	win.create("My Window", WINDOW_WIDTH, WINDOW_HEIGHT, 100, 0);
	core.init(win.hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);

	ConstantBuffer2 constBufferCPU2;
	constBufferCPU2.time = 0;

	GamesEngineeringBase::Timer timer;

	Shader_Manager sm;
	sm.init(&core);
	PSOManager psos;
	Texture_Manager tm;

	Object tree;
	tree.init(&core, &sm, &psos, &tm, "acacia");

	Object_Animation trex;
	trex.init(&core, &sm, &psos, &tm, "Trex");
	//Object_Animation fam;
	//fam.init(&core, &sm, &psos, &tm,"Farmer-male");

	Plane plane;
	plane.init(&core, &psos, &sm, &tm);
	Sphere sphere;
	sphere.init(&core, &psos, &sm, &tm);
	 
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

	float time = 0;
	//std::cout << "out" << std::endl;
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
			time = 0;
		}
			

		core.beginFrame();
		win.processMessages();

		plane.draw(&core, world, camera_.view_projection);
		sphere.draw(&core, world, camera_.view_projection);

		tree.update(world, camera_.view_projection);
		tree.draw(&core);

		trex.update(world, camera_.view_projection, dt, "attack");
		trex.draw(&core);

		farmer.update(&core, &win, dt);
		farmer.draw(&core);

		//fam.update(world, camera_.view_projection, dt, "idle basic 01");
		//fam.draw(&core);

		if (win.keys[VK_ESCAPE] == 1)
		{
			break;
		}
		core.finishFrame();
	}
	core.flushGraphicsQueue();

	FreeConsole();
}