#include "../../SDL3.h"

/**
 * @brief 检测点是否在矩形内
 * @param rect 矩形
 * @param x 点的X坐标
 * 
 */
bool collide_point(SDL_FRect &rect, float x, float y)
{
    return (x >= rect.x && (x < rect.x + rect.w) && y >= rect.y && (y < rect.y + rect.h));
}
    