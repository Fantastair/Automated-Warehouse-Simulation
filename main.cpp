#include <thread>
#include <string>
#include <iostream>
#include <filesystem>
#include "main.h"

std::string work_path;    // 工作目录
/**
 * @brief 主函数
 */
int main(int, char**)
{
    #ifdef MSVC_BUILD
        work_path = std::filesystem::current_path().string() + "\\..\\..";    // 初始化工作目录
    #else
        work_path = std::filesystem::current_path().string() + "/..";    // 初始化工作目录
    #endif
    // std::cout << "Current working directory: " << work_path << std::endl;
    RandomInit();                                            // 初始化随机数生成器
    
    // Launch_Init();    // 启动界面初始化

    DataClass_Init();                                     // 初始化底层数据类
    Display_Init(0, 0, SDL_WINDOW_HIGH_PIXEL_DENSITY);    // 初始化显示框架
    pre_load(rm);                                         // 预加载资源
    main_page_Init();                                     // 初始化主页面
    CarInfoInit();                                        // 初始化车辆信息模块
    std::thread sim_thread(simulate);                     // 创建仿真线程
    // Simulating = true;

    Display_Mainloop();        // 主循环

    thread_running = false;    // 停止仿真线程
    Simulating = false;        // 停止仿真
    sim_thread.join();         // 等待仿真线程结束
    delete root;               // 清除创建的显示元素
    rm.Clean();                // 清理资源
    Display_Quit();            // 退出显示框架
    return 0;
}
