#include "../../SDL3.h"
#include "../../Core.h"
#include "../../Display.h"
#include <iostream>

/**
 * @brief 启动界面初始化函数
 */
void Launch_Init(void)
{
    Display_Init(728, 500, SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    pre_load(rm);

    root = new BgUi(rm.getColor(FAKEWHITE));
    ImageUi *bg = new ImageUi("launch.png", 4, 4);
    bg->join(root);
    ImageUi *icon = new ImageUi("icon.png");
    icon->set_rect_size(128, 128);
    icon->set_rect_midbottom(68, 496);
    icon->join(root);
    TextUi *title = new TextUi(PROGRAM_NAME, rm.getFont("shuhei.ttf", 32), rm.getColor(DARKBLUE));
    title->set_rect_midleft(150, icon->rect.y + 32);
    title->join(root);
    RectUi *line = new RectUi(550, 8, 0, rm.getColor_(DARKBLUE), nullptr, 4);
    line->set_rect_midleft(150, icon->rect.y + 60);
    line->join(root);
    TextUi *subtitle1 = new TextUi("山东大学 控制学院第七届学以致用", rm.getFont("shuhei.ttf", 24), rm.getColor(DARKBLUE));
    TextUi *subtitle2 = new TextUi("2025 编程能力大赛作品", rm.getFont("shuhei.ttf", 24), rm.getColor(DARKBLUE));
    subtitle1->set_rect_midleft(150, icon->rect.y + 84);
    subtitle1->join(root);
    subtitle2->set_rect_midleft(150, icon->rect.y + 108);
    subtitle2->join(root);

    root->render(0, 0);
    SDL_RenderPresent(renderer);

    SDL_Delay(2500);
    delete root;
    root = nullptr;
    rm.Clean();
    Display_Quit();
}
