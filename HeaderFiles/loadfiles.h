#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include "vectors.h"
#include "mesh.h"
#include <random>

#define FILE_NAME_FLOWER_MATRIX "Save/flower_matrix.txt"
#define FILE_NAME_FENCE_MATRIX "Save/fence_matrix.txt"


inline float random_float(float min_v, float max_v)
{
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<float> dist(min_v, max_v);
    return dist(rng);
}

Vec3 random_dir_xz()
{
    float a = random_float(0.0f, 2.0f * M_PI);
    return Vec3(cosf(a), 0.0f, sinf(a));
}



void save_instance_matrices(
    const std::string filename,
    const std::vector<INSTANCE>& matrices)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    file << "# matrices\n";

    for (const INSTANCE& m : matrices)
    {
        for (int r = 0; r < 4; ++r)
        {
            for (int c = 0; c < 4; ++c)
            {
                file << m.w.a[r][c] << " ";
            }
        }
        file << "\n";
    }

    file.close();
}

bool load_instance_matrices( const std::string filename, std::vector<INSTANCE>& out_instances)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open instance file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream ss(line);

        Matrix m;
        for (int r = 0; r < 4; ++r)
        {
            for (int c = 0; c < 4; ++c)
            {
                if (!(ss >> m.a[r][c]))
                {
                    std::cerr << "Invalid matrix format in file\n";
                    return false;
                }
            }
        }

        INSTANCE inst;
        inst.w = m;
        out_instances.push_back(inst);
    }

    return true;
}

//create instance matrixs
Matrix generate_random_grass_matrix(
    const Vec3& center,
    float range_x,
    float range_z,
    float min_scale = 0.8f,
    float max_scale = 1.2f)
{

    float rx = random_float(-range_x, range_x);
    float rz = random_float(-range_z, range_z);

    Vec3 pos = {
        center.x + rx,
        center.y,
        center.z + rz
    };

    float angle = random_float(0.0f, 2.0f * M_PI);
    Matrix R = Matrix::rotateY(angle);

    float s = random_float(min_scale, max_scale);
    Matrix S = Matrix::Scaling(s);

    Matrix T = Matrix::Translate(pos);

    return T.mul(S).mul(R);
}

void create_matixes(std::vector<INSTANCE>& matrixs, const Vec3& center, float range_x, float range_z,
                float min_scale = 0.8f, float max_scale = 1.2f)
{
    for (unsigned int i = 0; i < 5; i++)
    {
        INSTANCE matrix;
        matrix.w = generate_random_grass_matrix(center, range_x, range_z, min_scale, max_scale);
        matrixs.push_back(matrix);
    }
}

std::vector<INSTANCE> generateFenceRectangle(float width, float depth, float segmentLength,
         Vec3 scale, Matrix adjust_matrix = Matrix(),float y = 0.0f)
{
    std::vector<INSTANCE> instances;

    int countX = static_cast<int>(width / segmentLength);
    int countZ = static_cast<int>(depth / segmentLength);

    float halfW = width * 0.5f;
    float halfD = depth * 0.5f;

    auto addInstance = [&](float x, float z, float rotY)
        {
            Matrix T = Matrix::Translate(Vec3( x, y, z ));
            Matrix R = Matrix::rotateY(rotY);
            Matrix S = Matrix::Scaling(scale);

            Matrix W = T.mul(S).mul(R).mul(adjust_matrix);
            instances.push_back({ W });
        };

    // +z
    for (int i = 0; i < countX; ++i)
    {
        float x = -halfW + i * segmentLength + segmentLength * 0.5f;
        float z = halfD;
        addInstance(x, z, 0.0f);
    }

    // -z
    for (int i = 0; i < countX; ++i)
    {
        float x = -halfW + i * segmentLength + segmentLength * 0.5f;
        float z = -halfD;
        addInstance(x, z, M_PI);
    }

    // +x
    for (int i = 0; i < countZ; ++i)
    {
        float x = halfW;
        float z = -halfD + i * segmentLength + segmentLength * 0.5f;
        addInstance(x, z, M_PI * 0.5f);
    }

    // -x
    for (int i = 0; i < countZ; ++i)
    {
        float x = -halfW;
        float z = -halfD + i * segmentLength + segmentLength * 0.5f;
        addInstance(x, z, -M_PI * 0.5f);
    }

    return instances;
}

