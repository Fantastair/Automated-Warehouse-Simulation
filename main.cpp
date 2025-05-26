#include <iostream>
#include "main.h"

/**
 * @brief 主函数
 */
int main(int, char**)
{
    DataClass_Init();
    Display_Init();
    pre_load(rm);

    root = new BgUi(0, 0, 0, 255);
    mark_update();

    ImageUi *img = new ImageUi("icon.png", 100, 100);
    img->join(root);

    TextUi *text = new TextUi("Hello, World!", rm.getFont("deyi.ttf", 32), 255, 0, 0, 255, 700, 200);
    text->join(root);

    SDL_Color bg = {255, 255, 0, 255};
    SDL_Color fg = {255, 0, 0, 255};
    RectUi *rect = new RectUi(200, 80, 4, &bg, &fg);
    rect->set_rect_center(800, 300);
    rect->join(root);

    Display_Mainloop();

    delete root;
    rm.Quit();
    Display_Quit();
    return 0;
}
