#pragma once
#include "HeaderFiles/window.h"
//#include "core.h"
//#include "renderer.h"
#include "HeaderFiles/GamesEngineeringBase.h"
//#include "mesh.h"
#include "HeaderFiles/model.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
	
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
	trex.init(&core, &sm, &psos, "Resource/Trex/Trex.gem");
	AnimationInstance animatedInstance;
	animatedInstance.init(&trex.animation, 0);

	while (1) {
		float dt = timer.dt();
		constBufferCPU2.time += dt;

		Vec3 from = Vec3(11 * cos(constBufferCPU2.time), 5, 11 * sinf(constBufferCPU2.time));
		Matrix v = Matrix::lookAt(from, Vec3(0, 1, 0), Vec3(0, 1, 0));
		Matrix p = Matrix::perspective(60, (float)(WINDOW_WIDTH) / (float)(WINDOW_HEIGHT), 0.1f, 80.f);
		Matrix world;
		world = Matrix::Scaling(Vec3(0.01f, 0.01f, 0.01f));
		Matrix vp = p.mul(v);

		for (int i = 0; i < 4; i++)
		{
			float angle = constBufferCPU2.time + (i * M_PI / 2.0f);
			constBufferCPU2.lights[i] = Vec4(WINDOW_WIDTH / 2.0f + (cosf(angle) * (WINDOW_WIDTH * 0.3f)),
				WINDOW_HEIGHT / 2.0f + (sinf(angle) * (WINDOW_HEIGHT * 0.3f)),
				0, 0);
		}

		core.beginFrame();
		win.processMessages();

		tree.update(world, vp);
		tree.draw(&core);

		animatedInstance.update("run", dt);
		if (animatedInstance.animationFinished() == true)
		{
			animatedInstance.resetAnimationTime();
		}
		trex.update(world, vp, &animatedInstance);
		trex.draw(&core);

		if (win.keys[VK_ESCAPE] == 1)
		{
			break;
		}
		core.finishFrame();
	}
	core.flushGraphicsQueue();

}