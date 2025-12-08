#pragma once
#include "HeaderFiles/window.h"
//#include "core.h"
//#include "renderer.h"
#include "HeaderFiles/GamesEngineeringBase.h"
//#include "mesh.h"
#include "HeaderFiles/model.h"
#include "HeaderFiles/camera.h"

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

	Object tree;
	tree.init(&core, &sm, &psos, "Resource/Tree/acacia_003.gem");

	Object_Animation trex;
	trex.init(&core, &sm, &psos, "Trex");
	Object_Animation AutomaticCarbine;
	AutomaticCarbine.init(&core, &sm, &psos, "AutomaticCarbine");
	AnimationInstance animatedInstance;
	animatedInstance.init(&trex.animation, 0);
	AnimationInstance animatedInstance2;
	animatedInstance2.init(&AutomaticCarbine.animation, 0);

	Camera camera(Vec3(10, 5, 10), Vec3(0, 1, 0), Vec3(0, 1, 0));
	camera.init((float)(WINDOW_WIDTH) / (float)(WINDOW_HEIGHT));
	Camera_ camera_;
	camera_.init_aspect((float)(WINDOW_WIDTH) , (float)(WINDOW_HEIGHT));
	camera_.init_pos(Vec3(0, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1));

	std::cout << "out" << std::endl;
	while (1) {
		float dt = timer.dt();
		constBufferCPU2.time += dt;

		//Vec3 from = Vec3(11 * cos(constBufferCPU2.time), 5, 11 * sinf(constBufferCPU2.time));
		//Matrix v = Matrix::lookAt(from, Vec3(0, 1, 0), Vec3(0, 1, 0));
		//Matrix p = Matrix::perspective(60, (float)(WINDOW_WIDTH) / (float)(WINDOW_HEIGHT), 0.1f, 80.f);

		Matrix world;
		world = Matrix::Scaling(Vec3(0.01f, 0.01f, 0.01f));
		//Matrix vp = p.mul(v);
		camera.update(&win, constBufferCPU2.time);
		camera_.update(&win, constBufferCPU2.time);

		for (int i = 0; i < 4; i++)
		{
			float angle = constBufferCPU2.time + (i * M_PI / 2.0f);
			constBufferCPU2.lights[i] = Vec4(WINDOW_WIDTH / 2.0f + (cosf(angle) * (WINDOW_WIDTH * 0.3f)),
				WINDOW_HEIGHT / 2.0f + (sinf(angle) * (WINDOW_HEIGHT * 0.3f)),
				0, 0);
		}

		core.beginFrame();
		win.processMessages();

		//tree.update(world, camera.vp);
		tree.update(world, camera_.view_projection);
		tree.draw(&core);

		animatedInstance.update("attack", dt);
		if (animatedInstance.animationFinished() == true)
		{
			animatedInstance.resetAnimationTime();
		}
		//trex.update(world, camera.vp, &animatedInstance);
		trex.update(world, camera_.view_projection, &animatedInstance);
		trex.draw(&core);

		if (win.keys[VK_ESCAPE] == 1)
		{
			break;
		}
		core.finishFrame();
	}
	core.flushGraphicsQueue();

	FreeConsole();
}