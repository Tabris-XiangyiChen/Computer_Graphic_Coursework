#pragma once
#include "vectors.h"
#include "window.h"

class Camera
{
public:
	Vec3 from;
	Vec3 to;
	Vec3 up;
	Matrix lookat;
	Matrix perspective;
	Matrix vp;
	float fov;
	float near_view;
	float far_view;
	float aspect;


	Camera()
	{
		from = Vec3(0.f, 0.f, 0.f);
		to = Vec3(0.f, 1.f, 0.f);
		up = Vec3(0.f, 0.f, 1.f);
		fov = M_PI / 3;
		near_view = 0.1f;
		far_view = 10.f;
	}

	Camera(Vec3 _from, Vec3 _to, Vec3 _up, float _fov = 60, float _near = 0.1f, float _far = 80.f)
	{
		from = _from;
		to = _to;
		up = _up;
		fov = _fov;
		near_view = _near;
		far_view = _far;
	}

	void init(float _aspect)
	{
		aspect = _aspect;
		lookat = Matrix::lookAt(from, to, up);
		perspective = Matrix::perspective(fov, aspect, near_view, far_view);
		vp = perspective.mul(lookat);
	}

	void update(Window* window, float dt)
	{
		//from = Vec3(11 * cos(dt), 5, 11 * sinf(dt));
		float speed = 5.0f;  // 每秒移动速度

		Vec3 forward = (to - from).Normalize();
		Vec3 right = forward.Cross(up).Normalize();

		if (window->keys['W'])
			from = from + forward * speed * dt;

		if (window->keys['S'])
			from = from - forward * speed * dt;

		if (window->keys['A'])
			from = from - right * speed * dt;

		if (window->keys['D'])
			from = from + right * speed * dt;

		// 更新矩阵
		lookat = Matrix::lookAt(from, to, up);
		perspective = Matrix::perspective(fov, aspect, near_view, far_view);
		vp = perspective.mul(lookat);
	}
};

class Camera_
{
public:
    Vec3 position;
    Vec3 target; 
    Vec3 up;
    Vec3 forward;
    Vec3 right; 

    float fov;
    float aspect;
    float near_plane;
    float far_plane;

    Matrix view;
    Matrix projection;
    Matrix view_projection;

    float speed;
    float mouse_sensitivity;
    float yaw;
    float pitch;

public:

    Camera_() :
        position(10, 5, 10),
        target(0, 1, 0),
        up(0, 0, 1),
        forward(0, 0, -1),
        fov(60.0f),
        aspect(16.0f / 9.0f),
        near_plane(0.01f),
        far_plane(100.0f),
        speed(0.01f),
        mouse_sensitivity(0.1f),
        yaw(0.0f),
        pitch(0.0f)
    {
        update_vectors();
        update_matrices();
    }

    void init_pos(Vec3 _pos, Vec3 _to, Vec3 _up)
    {
        position = _pos;
        target = _to;
        up = _up;

        forward = (target - position).Normalize();
        right = forward.Cross(up).Normalize();

        pitch = asinf(forward.z) * 180.0f / M_PI;
        yaw = atan2f(forward.y, forward.x) * 180.0f / M_PI;

        update_vectors();
        update_matrices();
    }

    void init_aspect(float width, float height)
    {
        aspect = width / height;
        update_matrices();
    }

    //update the camera's view scale
    void init_camview(float _fov, float width, float height, float _near_plane, float _far_plane)
    {
        fov = _fov;
        aspect = width / height;
        near_plane = _near_plane;
        far_plane = _far_plane;
        update_matrices();
    }


    void update(Window* window, float dt)
    {

        ProcessKeyboard(window, dt);

        ProcessMouse_notcenter(window);

        update_matrices();
    }


    void SetPerspective(float _fov, float _aspect, float _near, float _far)
    {
        fov = _fov;
        aspect = _aspect;
        near_plane = _near;
        far_plane = _far;
        projection = Matrix::perspective(fov, aspect, near_plane, far_plane);
        view_projection = projection * view;
    }

private:

    void ProcessKeyboard(Window* window, float dt)
    {
        float s = speed * dt;

        if (window->keys['W'])
            position = position + forward * s;

        if (window->keys['S'])
            position = position - forward * s;

        if (window->keys['A'])
            position = position - right * s;

        if (window->keys['D'])
            position = position + right * s;

        if (window->keys['Q'])
            position = position - up * s;

        if (window->keys['E'])
            position = position + up * s;

        target = position + forward;
    }


    void ProcessMouse(Window* window)
    {
        static float last_x;
        static float last_y;
        //if (!window->mouseButtons[0]) return;
        if (window->mousex == last_x && window->mousey == last_y)
            return;

        float centerX = window->width * 0.5f;
        float centerY = window->height * 0.5f;

        float xoffset = window->mousex - centerX;
        float yoffset = centerY - window->mousey;  // 注意 Y 轴反向

        xoffset *= mouse_sensitivity;
        yoffset *= mouse_sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        update_vectors();

        // move the mouse to screen centre
        last_x = window->mousex;
        last_y = window->mousey;
        window->SetCursorPos(centerX, centerY);
    }

    void ProcessMouse_notcenter(Window* window)
    {
        static float last_x = 0, last_y = 0;
        static bool firstMouse = true;

        if (window->mousex != last_x && window->mousey != last_y)  // 鼠标右键
        {
            if (firstMouse)
            {
                last_x = window->mousex;
                last_y = window->mousey;
                firstMouse = false;
            }

            float xoffset = last_x - window->mousex;
            float yoffset =  window->mousey - last_y;

            xoffset *= mouse_sensitivity;
            yoffset *= mouse_sensitivity;

            yaw += xoffset;
            pitch += yoffset;

            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;

            update_vectors();

            float centerX = window->width * 0.5f;
            float centerY = window->height * 0.5f;
            //window->SetCursorPos(centerX, centerY);
        }
        else
        {
            firstMouse = true;
        }

        last_x = window->mousex;
        last_y = window->mousey;
    }

    void update_vectors()
    {
        Vec3 newForward;
        newForward.x = cos(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);
        newForward.y = sin(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);
        newForward.z = sin(pitch * M_PI / 180.0f);

        forward = newForward.Normalize();

        right = forward.Cross(Vec3(0, 0, 1)).Normalize();
        up = right.Cross(forward).Normalize();

        target = position + forward;
    }

    // update matrices
    void update_matrices()
    {
        view = Matrix::lookAt(position, target, up);
        projection = Matrix::perspective(fov, aspect, near_plane, far_plane);
        view_projection = projection.mul(view);
    }
};
