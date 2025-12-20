#pragma once
#include "model.h"


static void build_ui_quad(
    std::vector<UI_Vertex>& vertices,
    std::vector<unsigned int>& indices)
{
    vertices =
    {
        {{-0.5f, -0.5f}, {0, 1}},
        {{ 0.5f, -0.5f}, {1, 1}},
        {{-0.5f,  0.5f}, {0, 0}},
        {{ 0.5f,  0.5f}, {1, 0}},
    };

    indices = { 0, 1, 2, 1, 3, 2 };
}

struct UI_CB
{
    float pos[2];   // NDC center position
    float size[2];  // width / height (NDC)
};



class UI_Image
{
public:
    Mesh mesh;
    Shader_Manager* shader_manager = nullptr;
    PSOManager* psos = nullptr;
    Texture_Manager* textures = nullptr;

    std::string vs_name = "VS_UI";
    std::string ps_name = "PS_UI";
    std::string pso_name = "UI_PSO";

    std::string texture_name;

    UI_CB ui_cb;

    void init(Core* core,Shader_Manager* sm,PSOManager* pso_mgr,Texture_Manager* tex_mgr,const std::string& tex,
        float x, float y, float w, float h)
    {
        shader_manager = sm;
        psos = pso_mgr;
        textures = tex_mgr;
        texture_name = tex;

        ui_cb.pos[0] = x;
        ui_cb.pos[1] = y;
        ui_cb.size[0] = w;
        ui_cb.size[1] = h;

        std::vector<UI_Vertex> vertices;
        std::vector<unsigned int> indices;
        build_ui_quad(vertices, indices);
        mesh.init_ui(core, vertices, indices);

        shader_manager->load(core, vs_name, Shader_Type::VERTEX);
        shader_manager->load(core, ps_name, Shader_Type::PIXEL);

        psos->createPSO_UI( core, pso_name,shader_manager->shaders[vs_name].shader,shader_manager->shaders[ps_name].shader, VertexLayoutCache::getUILayout());

        std::string path = "UI/" + tex + ".png";
        textures->load_onlyALB(core, tex, path);
    }

    void update_cb()
    {
        shader_manager->update( vs_name, "UI_Buffer","ui_pos", ui_cb.pos);

        shader_manager->update( vs_name,"UI_Buffer", "ui_size", ui_cb.size);
    }

    void draw(Core* core)
    {
        core->beginRenderPass();

        update_cb();

        psos->bind(core, pso_name);
        textures->updateTexturePS(core, texture_name);

        // VS CB bind
        for (auto& cb : shader_manager->shaders[vs_name].constantBuffers)
        {
            core->getCommandList()->SetGraphicsRootConstantBufferView(5, cb.second.getGPUAddress());
            cb.second.next();
        }

        mesh.draw(core);
    }
};

class UI_Manager
{
public:
    std::unordered_map<std::string, UI_Image*> images;

    void add(Core* core, Shader_Manager* sm, PSOManager* psos, Texture_Manager* textures, const std::string& name,
        float x, float y, float w, float h)
    {
        UI_Image* img = new UI_Image();
        img->init(core, sm, psos, textures, name, x, y, w, h);

        images[name] = img;
    }

    void draw_one(Core* core, const std::string& name)
    {
        auto it = images.find(name);
        if (it != images.end())
        {
            it->second->draw(core);
        }
    }

    void draw_all(Core* core)
    {
        for (auto& img : images)
        {
            img.second->draw(core);
        }
    }
};

class UI_Number
{
public:
    UI_Manager* ui_mgr = nullptr;

    float start_x = -0.1f;
    float start_y = 0.0f;
    float digit_w = 0.1f;
    float digit_h = 0.2f;
    float spacing = 0.01f;

    UI_Image* digits[10] = {};

    void init_digits(Core* core,Shader_Manager* sm, PSOManager* psos,Texture_Manager* textures,UI_Manager* mgr,
        float x, float y, float w, float h,float _spacing = 0.01f)
    {
        ui_mgr = mgr;
        start_x = x;
        start_y = y;
        digit_w = w;
        digit_h = h;
        spacing = _spacing;

        for (int i = 0; i < 10; ++i)
        {
            std::string name = std::to_string(i);

            UI_Image* img = new UI_Image();
            img->init(core, sm, psos, textures, name, x, y, w, h);

            digits[i] = img;
            ui_mgr->images.insert({std::to_string(i), img });
        }
    }

    void draw_number(Core* core, int value)
    {
        if (value < 0) value = 0;

        std::string text = std::to_string(value);

        float x = start_x;

        for (char c : text)
        {
            int digit = c - '0';
            if (digit < 0 || digit > 9)
                continue;

            UI_Image* img = digits[digit];

            // update cb
            img->ui_cb.pos[0] = x;
            img->ui_cb.pos[1] = start_y;

            img->ui_cb.size[0] = digit_w;
            img->ui_cb.size[1] = digit_h;

            img->draw(core);

            x += digit_w + spacing;
        }
    }
};
