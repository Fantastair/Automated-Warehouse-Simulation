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
