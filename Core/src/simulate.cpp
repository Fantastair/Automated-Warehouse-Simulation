#include <thread>
#include <iostream>

#include "../../SDL3.h"
#include "../../Core.h"

bool thread_running = true;    // 仿真线程是否正在运行
bool Simulating = false;       // 是否正在仿真
float simulation_speed = 8;      // 仿真速度，单位倍数
Uint64 system_runtime = 0;     // 系统运行时间，单位纳秒
Uint64 simulation_time = 0;    // 仿真时间，单位纳秒

/**
 * @brief 运行仿真
 * @note 该函数在仿真线程中运行
 */
void simulate(void)
{
    system_runtime = GetNS();
    while (thread_running)
    {
        Uint64 t = GetNS() - system_runtime;    // 计算时间差
        system_runtime += t;                    // 更新时间戳
        if (Simulating)
        {
            t = static_cast<Uint64>(t * simulation_speed);           // 根据仿真速度调整时间差
            simulation_time += t;            // 累加仿真时间
            update_simulation(t);            // 更新状态和数据
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds(200000));
        }
    }
}

/**
 * @brief 设置仿真速度
 * @param speed 仿真速度倍数
 */
void set_simulation_speed(float speed)
{
    simulation_speed = speed;
}

/**
 * @brief 获取仿真速度
 * @return 返回当前仿真速度倍数
 */
float get_simulation_speed(void)
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
    for (int i = 17; i >= 0; i--)
    {
        ConnectorList[i].update(t);  // 更新接口设备状态
    }
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
