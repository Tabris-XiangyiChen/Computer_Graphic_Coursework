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
    Vec3 position;      // 摄像机位置
    Vec3 target;        // 观察目标
    Vec3 up;            // 上方向
    Vec3 forward;       // 前方向
    Vec3 right;         // 右方向

    float fov;          // 视野角度（度）
    float aspect;       // 宽高比
    float near_plane;    // 近平面
    float far_plane;     // 远平面

    // 变换矩阵
    Matrix view;
    Matrix projection;
    Matrix viewProjection;

    // 控制参数
    float moveSpeed;
    float mouseSensitivity;
    float yaw;          // 水平旋转
    float pitch;        // 垂直旋转

public:
    // 构造函数
    Camera_() :
        position(10, 5, 10),
        target(0, 1, 0),
        up(0, 0, 1),
        forward(0, 0, -1),
        fov(60.0f),
        aspect(16.0f / 9.0f),
        near_plane(0.01f),
        far_plane(100.0f),
        moveSpeed(0.01f),
        mouseSensitivity(0.1f),
        yaw(-90.0f),
        pitch(0.0f)
    {
        UpdateVectors();
        UpdateMatrices();
    }

    // 初始化
    void init(float width, float height)
    {
        aspect = width / height;
        UpdateMatrices();
    }

    // 更新摄像机
    void update(Window* window, float dt)
    {
        // 处理键盘输入
        ProcessKeyboard(window, dt);

        // 处理鼠标输入
        ProcessMouse(window);

        // 更新矩阵
        UpdateMatrices();
    }

    // 设置位置和朝向
    void LookAt(const Vec3& pos, const Vec3& targ, const Vec3& upVec = Vec3(0, 0, 1))
    {
        position = pos;
        target = targ;
        up = upVec;

        forward = (target - position).Normalize();
        right = forward.Cross(up).Normalize();

        UpdateMatrices();
    }

    // 设置投影参数
    void SetPerspective(float _fov, float _aspect, float _near, float _far)
    {
        fov = _fov;
        aspect = _aspect;
        near_plane = _near;
        far_plane = _far;
        projection = Matrix::perspective(fov, aspect, near_plane, far_plane);
        viewProjection = projection * view;
    }

private:
    // 处理键盘输入
    void ProcessKeyboard(Window* window, float dt)
    {
        float speed = moveSpeed * dt;

        if (window->keys['W'])
            position = position + forward * speed;

        if (window->keys['S'])
            position = position - forward * speed;

        if (window->keys['A'])
            position = position - right * speed;

        if (window->keys['D'])
            position = position + right * speed;

        if (window->keys['Q'])
            position = position - up * speed;

        if (window->keys['E'])
            position = position + up * speed;

        // 更新目标点
        target = position + forward;
    }

    // 处理鼠标输入
    void ProcessMouse(Window* window)
    {
        static float lastX = 0, lastY = 0;
        static bool firstMouse = true;

        if (window->mouseButtons[0]) // 左键按下
        {
            if (firstMouse)
            {
                lastX = window->mousex;
                lastY = window->mousey;
                firstMouse = false;
            }

            float xoffset = window->mousex - lastX;
            float yoffset = lastY - window->mousey; // 反转Y轴

            lastX = window->mousex;
            lastY = window->mousey;

            xoffset *= mouseSensitivity;
            yoffset *= mouseSensitivity;

            yaw += xoffset;
            pitch += yoffset;

            // 限制俯仰角
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;

            UpdateVectors();
        }
        else
        {
            firstMouse = true;
        }

        //// 滚轮缩放
        //if (window->mouseWheelDelta != 0)
        //{
        //    fov -= window->mouseWheelDelta * 2.0f;
        //    if (fov < 1.0f) fov = 1.0f;
        //    if (fov > 120.0f) fov = 120.0f;

        //    projection = Matrix::perspective(fov * M_PI / 180.0f, aspect, nearPlane, farPlane);
        //    viewProjection = projection * view;

        //    window->mouseWheelDelta = 0;
        //}
    }

    // 更新方向向量
    void UpdateVectors()
    {
        // 根据yaw和pitch计算前方向
        Vec3 newForward;
        newForward.x = cos(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);
        newForward.y = sin(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);
        newForward.z = sin(pitch * M_PI / 180.0f);

        forward = newForward.Normalize();

        // 重新计算右方向和上方向
        right = forward.Cross(Vec3(0, 0, 1)).Normalize();
        up = right.Cross(forward).Normalize();

        // 更新目标点
        target = position + forward;
    }

    // 更新变换矩阵
    void UpdateMatrices()
    {
        view = Matrix::lookAt(position, target, up);
        projection = Matrix::perspective(fov, aspect, near_plane, far_plane);
        viewProjection = projection.mul(view);
    }
};
