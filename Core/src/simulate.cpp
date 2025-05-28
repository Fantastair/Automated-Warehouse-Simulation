#include <thread>
#include <iostream>

#include "../../SDL3.h"
#include "../../Core.h"

Uint64 last_NS;             // 上次仿真时间戳
bool thread_running = true;  // 仿真线程是否正在运行
bool Simulating = false;    // 是否正在仿真
int simulation_speed = 8; // 仿真速度，单位倍数
/**
 * @brief 运行仿真
 * @note 该函数在仿真线程中运行
 */
void simulate(void)
{
    while (thread_running)
    {
        while (!Simulating)
        {
            if (!thread_running) return;  // 如果线程已停止，退出
        }
        last_NS = GetNS();
        while (Simulating)
        {
            Uint64 t = GetNS() - last_NS;    // 计算时间差
            last_NS += t;                    // 更新时间戳
            t *= simulation_speed;           // 根据仿真速度调整时间差
            update_simulation(t);            // 更新状态和数据
        }
    }
}

/**
 * @brief 设置仿真速度
 * @param speed 仿真速度倍数
 */
void set_simulation_speed(int speed)
{
    simulation_speed = speed;
}

/**
 * @brief 获取仿真速度
 * @return 返回当前仿真速度倍数
 */
int get_simulation_speed(void)
{
    return simulation_speed;
}

/**
 * @brief 更新状态和数据
 * @param t 时间差，单位纳秒
 * @note 该函数在仿真线程中运行
 */
void update_simulation(Uint64 t)
{
    // 在该函数里实现仿真逻辑
    int j = GetFreeCar()->id;
    for (int i = j; i < CarNum + j; i++)
    {
        CarList[i % CarNum].update(t);  // 更新车辆状态
    }
}

/**
 * @brief 启动仿真线程
 * @note 该函数在主线程中调用
 */
void start_simulation(void)
{
    std::thread sim_thread(simulate);  // 创建仿真线程
}
