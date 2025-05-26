#include "../../SDL3.h"
#include "../../Core.h"

Uint64 last_NS;             // 上次仿真时间戳
bool Simulating = false;    // 是否正在仿真

/**
 * @brief 运行仿真
 * @note 该函数在仿真线程中运行
 */
void simulate(void)
{
    last_NS = GetNS();
    Simulating = true;
    while (Simulating)
    {
        auto t = GetNS() - last_NS;    // 计算时间差
        last_NS += t;                  // 更新时间戳
        update(t);                 // 更新状态和数据
    }
}

/**
 * @brief 更新状态和数据
 * @param t 时间差，单位纳秒
 * @note 该函数在仿真线程中运行
 */
void update(Uint64 t)
{
    // 在该函数里实现仿真逻辑
    t;
}
