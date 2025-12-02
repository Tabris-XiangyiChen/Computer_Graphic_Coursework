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
	sm.init();
	Object_Manager cube;
	cube.init(&core, &sm);

	while (1) {
		float dt = timer.dt();
		constBufferCPU2.time += dt;

		Vec3 from = Vec3(11 * cos(constBufferCPU2.time), 5, 11 * sinf(constBufferCPU2.time));
		Matrix v = Matrix::lookAtMatrix(from, Vec3(0, 1, 0), Vec3(0, 1, 0));
		Matrix p = Matrix::projectionMatrix(M_PI / 3, (float)(WINDOW_WIDTH) / (float)(WINDOW_HEIGHT), 0.1f, 80.f);
		//Matrix v = Matrix::LookAt_matrix(from, Vec3(0, 1, 0), Vec3(0, 1, 0));
		//Matrix p = Matrix::Perspective_projection_matrix(M_PI / 3, (float)(WINDOW_WIDTH) / (float)(WINDOW_HEIGHT), 0.1f, 80.f);
		//Matrix v = Matrix::LookAtLH(from, Vec3(0, 1, 0), Vec3(0, 1, 0));
		//Matrix p = Matrix::PerspectiveLH(2 * M_PI / 3, (float)(WINDOW_WIDTH) / (float)(WINDOW_HEIGHT), 0.1f, 80.f);
		Matrix w;

		for (int i = 0; i < 4; i++)
		{
			float angle = constBufferCPU2.time + (i * M_PI / 2.0f);
			constBufferCPU2.lights[i] = Vec4(WINDOW_WIDTH / 2.0f + (cosf(angle) * (WINDOW_WIDTH * 0.3f)),
				WINDOW_HEIGHT / 2.0f + (sinf(angle) * (WINDOW_HEIGHT * 0.3f)),
				0, 0);
		}

		core.beginFrame();
		win.processMessages();
		cube.update(v, p);
		cube.draw(&core);

		if (win.keys[VK_ESCAPE] == 1)
		{
			break;
		}
		core.finishFrame();
	}
	core.flushGraphicsQueue();

}