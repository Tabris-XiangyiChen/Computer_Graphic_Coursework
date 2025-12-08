#pragma once
#include <string>
#include <windows.h>
#include <fstream>
#include <iostream>

class debug
{
public:
    enum class log_target
    {
        OutputWindow,
        Console,
        File
    };

private:
    static bool console_initialized;
    static std::ofstream log_file;
    static log_target current_target;

public:

    // 设置输出模式
    static void set_target(log_target t)
    {
        current_target = t;
    }

    // 初始化控制台
    static void init_console()
    {
        if (!console_initialized)
        {
            AllocConsole();
            FILE* fp;
            freopen_s(&fp, "CONOUT$", "w", stdout);
            freopen_s(&fp, "CONOUT$", "w", stderr);
            console_initialized = true;
        }
    }

    // 初始化文件 log.txt
    static void init_file(const std::string& filename = "log.txt")
    {
        if (!log_file.is_open())
        {
            log_file.open(filename, std::ios::out | std::ios::app);
        }
    }

    // 主 log 输出
    static void log(const std::string& msg)
    {
        switch (current_target)
        {
        case log_target::OutputWindow:
            OutputDebugStringA((msg + "\n").c_str());
            break;

        case log_target::Console:
            init_console();
            std::cout << msg << std::endl;
            break;

        case log_target::File:
            init_file();
            log_file << msg << std::endl;
            break;
        }
    }

    // vector3 输出
    static void log_vec3(const std::string& name, float x, float y, float z)
    {
        log(name + ": (" + std::to_string(x) + ", "
            + std::to_string(y) + ", "
            + std::to_string(z) + ")");
    }

    // 数字输出
    static void log_int(const std::string& name, int v)
    {
        log(name + ": " + std::to_string(v));
    }

    static void log_float(const std::string& name, float v)
    {
        log(name + ": " + std::to_string(v));
    }
};
