#include <iostream>
#include "main.h"

/**
 * @brief 主函数
 */
int main(int, char**)
{
    // Launch_Init();    // 启动界面初始化

    DataClass_Init();                                // 初始化底层数据类
    Display_Init(WINDOW_WIDTH, WINDOW_HEIGHT, 0);    // 初始化显示框架
    pre_load(rm);                                    // 预加载资源
    main_page_Init();                                // 初始化主页面

    Display_Mainloop();    // 主循环

    delete root;           // 清除创建的显示元素
    rm.Clean();            // 清理资源
    Display_Quit();        // 退出显示框架
    return 0;
}
