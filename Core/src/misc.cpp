#include "../../SDL3.h"
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
