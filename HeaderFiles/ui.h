#pragma once
#include "model.h"


void build_ui_quad(
    std::vector<UI_Vertex>& vertices,
    std::vector<unsigned int>& indices,
    float x, float y,        // NDC 坐标
    float w, float h         // NDC 尺寸
)
{
    vertices =
    {
        {{x,     y    }, {0, 0}},
        {{x + w, y    }, {1, 0}},
        {{x,     y - h}, {0, 1}},
        {{x + w, y - h}, {1, 1}},
    };

    indices = { 0, 1, 2, 1, 3, 2 };
}

class UI_Image
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
    }*/

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
    std::vector<UI_Image*> images;

    void init(Core* core, Shader_Manager* _shader_manager, PSOManager* _psos, Texture_Manager* _textures, std::string filename)
    {
        UI_Image* ui = new UI_Image();
        ui->init(core, _shader_manager, _psos, _textures, filename, -0.5, -0.5, 1, 1);
        add(ui);
    }

    void add(UI_Image* img)
    {
        images.push_back(img);
    }

    void draw(Core* core)
    {
        for (auto img : images)
            img->draw(core);
    }
};
