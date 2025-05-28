#include <random>
#include "../../SDL3.h"
#include "../../Core.h"
#include "../../Display.h"


/**
 * @brief 获取当前时间戳
 * @note 时间戳表示从程序初始化到调用此函数时的时间
 * @return 当前时间戳，单位纳秒
 */
Uint64 GetNS(void)
{
    return SDL_GetTicksNS();
}


/**
 * @brief 弧长转角度
 * @param arc 弧长，单位 mm
 * @param radius 圆的半径，单位 mm
 * @return 转换后的角度，弧度制
 */
double arc2angle(double arc, double radius)
{
    return arc / radius;
}

/**
 * @brief 弧度转角度
 * @param angle 弧度
 * @return 转换后的角度，单位度
 */
double angle2degree(double angle)
{
    return angle * 180.0 / SDL_PI_D;
}

/**
 * @brief 计算最小制动距离
 * @param current_speed 当前速度，单位 mm/ns
 * @param target_speed 目标速度，单位 mm/ns
 * @return 最小制动距离，单位 mm
 */
double min_break_distance(double current_speed, double target_speed)
{
    return (current_speed * current_speed - target_speed * target_speed) / (2 * ACCELERATION);
}

/**
 * @brief 初始化随机数生成器
 * @note 该函数在程序启动时调用一次
 */
void RandomInit(void)
{
    std::random_device rd;
    std::mt19937 generator(rd());
    std::srand(static_cast<unsigned int>(generator()));
}

/**
 * @brief 生成随机整数
 * @param a 最小值
 * @param b 最大值
 * @return 返回 [a, b] 范围内的随机整数
 */
int RandomInt(double a, double b)
{
    return static_cast<int>(std::rand() % static_cast<int>(b - a + 1) + a);
}

/**
 * @brief 生成随机任务
 * @param task 任务引用
 * @note 该函数会随机生成入库或出库任务
 */
void GenerateRandomTask(CarTask &task)
{
    // 生成随机任务
    task.task_type = RandomInt(0, 1);    // 随机任务类型，0 表示入库，1 表示出库
    if (task.task_type == 0)    // 入库任务
    {
        task.start_connector = RandomInt(15, 17);    // 随机入库接口
        task.end_connector = RandomInt(0, 5) * 2;    // 随机出库接口
    }
    else    // 出库任务
    {
        task.start_connector = RandomInt(0, 5) * 2 + 1;    // 随机入库接口
        task.end_connector = RandomInt(12, 14);    // 随机出库接口
    }
}
