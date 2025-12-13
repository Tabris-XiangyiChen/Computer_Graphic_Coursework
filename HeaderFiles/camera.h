#pragma once
#include "vectors.h"
#include "window.h"

class Camera
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

    Camera() :
        position(Vec3(0, 0, 0)),
        target(Vec3(0, 0, 1)),
        up(Vec3(0, 1, 0)),
        forward(Vec3(0, 0, 1)),
        fov(60.0f),
        aspect(16.0f / 9.0f),
        near_plane(0.01f),
        far_plane(3000.0f),
        speed(20.f),
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
        update_vectors();

        update_matrices();
    }

    void update_free(Window* window, float dt)
    {
        ProcessKeyboard(window, dt);

        //ProcessMouse(window);

        update_vectors();

        update_matrices();
    }


    // update matrices
    void update_matrices()
    {
        view = Matrix::lookAt(position, target, up);
        projection = Matrix::perspective(fov, aspect, near_plane, far_plane);
        view_projection = projection.mul(view);

    }
    void update_vectors()
    {
        Vec3 newForward;
        newForward.x = cos(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);
        newForward.y = sin(yaw * M_PI / 180.0f) * cos(pitch * M_PI / 180.0f);
        newForward.z = sin(pitch * M_PI / 180.0f);

        forward = newForward.Normalize();

        Vec3 world_up = Vec3(0, 1, 0);
        right = forward.Cross(world_up).Normalize();
        up = right.Cross(forward).Normalize();

        target = position + forward;
    }

    // 获取相机在地面平面上的方向（忽略垂直分量）
    Vec3 get_forward_flat() const
    {
        Vec3 flat_forward = forward;
        flat_forward.y = 0;  // 如果Y是垂直轴，忽略Y分量
        if (flat_forward.length() > 0.001f)
            return flat_forward.Normalize();
        return Vec3(0, 0, 1);  // 默认前向
    }

    Vec3 get_right_flat() const
    {
        Vec3 flat_right = right;
        flat_right.y = 0;  // 如果Y是垂直轴，忽略Y分量
        if (flat_right.length() > 0.001f)
            return flat_right.Normalize();
        return Vec3(1, 0, 0);  // 默认右向
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
        //window->SetCursorPos(centerX, centerY);
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

};

class ThirdPersonCameraController
{
public:
    Camera* camera;
    Vec3* target_pos;    // charactor positino

    float distance = 200.0f;       // distance to the charactor
    float min_distance = 10.0f;
    float max_distance = 50.0f;

    float yaw = 0.0f;
    float pitch = 20.0f;

    Vec3 target_offset = Vec3(0, 100.0f, 0);

    float mouse_sensitivity = 0.1f;

    bool mouse_free = false;
    bool mouse_lock_initialized = false;

    float last_mousex = 0.0f;
    float last_mousey = 0.0f;
public:
    void init(Camera* cam, Vec3* target)
    {
        camera = cam;
        target_pos = target;

        //yaw = camera->yaw;
        //pitch = camera->pitch;
        update_camera_transform();
    }

    void update(Window* window, float dt)
    {
        update_mouse_mode(window);

        update_mouse(window);
        
        //update_zoom(window);

        update_camera_transform();
    }


private:

    void update_mouse_mode(Window* window)
    {
        static bool key_1_last = false;

        bool key_1_now = window->keys['1'];

        if (key_1_now && !key_1_last)
        {
            mouse_free = !mouse_free;
            mouse_lock_initialized = false; // 强制重新初始化
        }

        key_1_last = key_1_now;
    }

    void update_mouse(Window* window)
    {
        if (mouse_free)
        {
            ShowCursor(TRUE);
            ClipCursor(NULL);        // 释放鼠标限制
            mouse_lock_initialized = false;
            return;
        }
        float centerX = window->width * 0.5f;
        float centerY = window->height * 0.5f;

        ShowCursor(FALSE);
        if (!mouse_lock_initialized)
        {
            RECT rect;
            GetClientRect(window->hwnd, &rect);

            POINT ul = { rect.left, rect.top };
            POINT lr = { rect.right, rect.bottom };
            ClientToScreen(window->hwnd, &ul);
            ClientToScreen(window->hwnd, &lr);

            RECT clip_rect = { ul.x, ul.y, lr.x, lr.y };
            ClipCursor(&clip_rect);

            POINT p{ (LONG)centerX, (LONG)centerY };
            ClientToScreen(window->hwnd, &p);
            SetCursorPos(p.x, p.y);

            last_mousex = centerX;
            last_mousey = centerY;

            mouse_lock_initialized = true;
            return;
        }

        float dx = window->mousex - centerX;
        float dy = window->mousey - centerY;

        dx *= mouse_sensitivity;
        dy *= mouse_sensitivity;

        yaw -= dx;
        pitch += dy;

        pitch = clamp(pitch, -40.0f, 80.0f);

        // 用完立即把鼠标拉回中心（关键）
        POINT p{ (LONG)centerX, (LONG)centerY };
        ClientToScreen(window->hwnd, &p);
        SetCursorPos(p.x, p.y);
    
    }

    //void update_mouse(Window* window)
    //{
    //    if (!mouse_look_enabled)
    //        return;
    //    RECT r;
    //    GetClientRect(window->hwnd, &r);
    //    int center_x = (r.right - r.left) / 2;
    //    int center_y = (r.bottom - r.top) / 2;
    //    // 当前鼠标位置（已经是相对窗口的）
    //    float dx = window->mousex - center_x;
    //    float dy = window->mousey - center_y;
    //    dx *= mouse_sensitivity;
    //    dy *= mouse_sensitivity;
    //    yaw -= dx;
    //    pitch += dy;
    //    pitch = clamp(pitch, -40.0f, 80.0f);
    //    SetCursorPos(window->width / 2, window->height / 2);
    //}

    //void update_zoom(Window* window)
    //{
    //    distance -= window->mouse_scroll * 0.5f;
    //    distance = clamp(distance, min_distance, max_distance);
    //}

    void update_camera_transform()
    {
        Vec3 orbit_target = *target_pos + target_offset;

        float radYaw = yaw * M_PI / 180.0f;
        float radPitch = pitch * M_PI / 180.0f;

        // 球坐标 → 偏移
        Vec3 offset;
        offset.x = distance * cos(radPitch) * cos(radYaw);
        offset.z = distance * cos(radPitch) * sin(radYaw);  // Z轴作为水平面
        offset.y = distance * sin(radPitch);                // Y轴作为垂直轴

        // 3. 设置相机位置
        camera->position = orbit_target + offset;

        // 4. 设置相机看向目标点
        camera->target = orbit_target;

        // 5. 重新计算相机方向向量
        camera->forward = (camera->target - camera->position).Normalize();

        Vec3 world_up = Vec3(0, 1, 0);  // Y轴向上
        camera->right = camera->forward.Cross(world_up).Normalize();
        camera->up = camera->right.Cross(camera->forward).Normalize();

        // 6. 更新相机的yaw和pitch（保持同步）
        camera->pitch = pitch;
        camera->yaw = yaw;

        // 7. 更新相机矩阵
        camera->update_matrices();
    }
};
