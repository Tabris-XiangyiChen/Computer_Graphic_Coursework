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

/*class UI_Image
{
public:
    Mesh mesh;
    Shader_Manager* shader_manager;
    PSOManager* psos;
    Texture_Manager* textures;

    std::string vs_name = "VS_UI";
    std::string ps_name = "PS_UI";
    std::string pso_name = "UI_PSO";

    std::string texture_name;

    float x, y, w, h; // NDC

    void init(
        Core* core,
        Shader_Manager* sm,
        PSOManager* pso_mgr,
        Texture_Manager* tex_mgr,
        const std::string& tex,
        float _x, float _y, float _w, float _h
    )
    {
        shader_manager = sm;
        psos = pso_mgr;
        textures = tex_mgr;
        texture_name = tex;

        x = _x; y = _y; w = _w; h = _h;

        std::vector<UI_Vertex> vertices;
        std::vector<unsigned int> indices;
        build_ui_quad(vertices, indices, x, y, w, h);

        mesh.init_ui(core, vertices, indices);

        shader_manager->load(core, vs_name, Shader_Type::VERTEX);
        shader_manager->load(core, ps_name, Shader_Type::PIXEL);

        psos->createPSO_UI(
            core,
            pso_name,
            shader_manager->shaders[vs_name].shader,
            shader_manager->shaders[ps_name].shader,
            VertexLayoutCache::getUILayout() // UI layout
        );

        textures->load_onlyALB(core, tex, tex);
    }

    /*void set_position(float _x, float _y)
    {
        x = _x;
        y = _y;

        std::vector<UI_Vertex> vertices;
        std::vector<unsigned int> indices;
        build_ui_quad(vertices, indices, x, y, w, h);

        mesh.updateVertices(vertices); // ⭐ 关键：更新位置
    }

    void draw(Core* core)
    {
        core->beginRenderPass();

        psos->bind(core, pso_name);
        textures->updateTexturePS(core, texture_name);

        mesh.draw(core);
    }
};


class UI_Manager
{
public:
    std::unordered_map<std::string, UI_Image*> images;

    std::vector<UI_Image*> number_slots;

    float digit_width = 0.08f;
    float digit_height = 0.12f;

    void init(Core* core, Shader_Manager* _shader_manager, PSOManager* _psos, Texture_Manager* _textures, std::string filename)
    {
        UI_Image* ui = new UI_Image();
        std::string fileroot = "UI/" + filename + ".png";
        ui->init(core, _shader_manager, _psos, _textures, filename, -0.5, -0.5, 1, 1);
        add(filename, ui);
    }

    void add(std::string filename, UI_Image* img)
    {
        images.insert({ filename, img });
    }

    void draw(Core* core)
    {
        for (auto img : images)
            img.second->draw(core);
    }

    void init_digit_set(
        Core* core,
        Shader_Manager* sm,
        PSOManager* psos,
        Texture_Manager* textures,
        float digit_w,
        float digit_h
    )
    {
        for (int i = 0; i <= 9; ++i)
        {
            UI_Image* img = new UI_Image();
            std::string name = std::to_string(i);
            std::string path = "UI/" + name + ".png";

            // ⚠ 初始化时位置随便，真正 draw 时用多个实例
            img->init(core, sm, psos, textures, path, 0, 0, digit_w, digit_h);

            number_slots[i] = img;
        }
    }

    void set_number(
        int value,
        float start_x,
        float y,
        float spacing = 0.01f)
    {
        number_slots.clear();

        std::string text = std::to_string(value);

        float x = start_x;

        for (char c : text)
        {
            std::string key(1, c);

            auto it = images.find(key);
            if (it == images.end())
                continue;

            UI_Image* src = it->second;

            // ⭐ 克隆一个 UI_Image 实例
            UI_Image* digit = new UI_Image(*src);

            digit->set_position(x, y);

            number_slots.push_back(digit);

            x += digit_width + spacing;
        }
    }

    void draw_number(Core* core)
    {
        for (UI_Image* img : number_slots)
        {
            img->draw(core);
        }
    }


};*/

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
