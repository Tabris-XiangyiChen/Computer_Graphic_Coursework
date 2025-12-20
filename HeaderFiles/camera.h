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
        position(Vec3(0, 0, 10)),
        target(Vec3(0, 0, 1)),
        up(Vec3(0, 1, 0)),
        forward(Vec3(0, 0, 1)),
        fov(60.0f),
        aspect(16.0f / 9.0f),
        near_plane(0.01f),
        far_plane(30000.0f),
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

    Vec3 get_forward_flat() const
    {
        Vec3 flat_forward = forward;
        flat_forward.y = 0; 
        if (flat_forward.length() > 0.001f)
            return flat_forward.Normalize();
        return Vec3(0, 0, 1);
    }

    Vec3 get_right_flat() const
    {
        Vec3 flat_right = right;
        flat_right.y = 0; 
        if (flat_right.length() > 0.001f)
            return flat_right.Normalize();
        return Vec3(1, 0, 0); 
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
        float yoffset = centerY - window->mousey;

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

        if (window->mousex != last_x && window->mousey != last_y) 
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
            mouse_lock_initialized = false;
        }

        key_1_last = key_1_now;
    }

    void update_mouse(Window* window)
    {
        if (mouse_free)
        {
            ShowCursor(TRUE);
            ClipCursor(NULL);
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

        pitch = clamp(pitch, -30.0f, 80.0f);

        // drag the mouse back to the center 
        POINT p{ (LONG)centerX, (LONG)centerY };
        ClientToScreen(window->hwnd, &p);
        SetCursorPos(p.x, p.y);
    
    }

    void update_camera_transform()
    {
        Vec3 target = *target_pos + target_offset;

        float radiusYaw = yaw * M_PI / 180.0f;
        float radiusPitch = pitch * M_PI / 180.0f;

        Vec3 offset;
        offset.x = distance * cos(radiusPitch) * cos(radiusYaw);
        offset.z = distance * cos(radiusPitch) * sin(radiusYaw);
        offset.y = distance * sin(radiusPitch);

        camera->position = target + offset;

        // Set the camera to look at the target point.
        camera->target = target;

        camera->forward = (camera->target - camera->position).Normalize();

        Vec3 world_up = Vec3(0, 1, 0);
        camera->right = camera->forward.Cross(world_up).Normalize();
        camera->up = camera->right.Cross(camera->forward).Normalize();

        camera->pitch = pitch;
        camera->yaw = yaw;
        //update vp matirxs
        camera->update_matrices();
    }
};
