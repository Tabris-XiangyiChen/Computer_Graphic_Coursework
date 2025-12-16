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
