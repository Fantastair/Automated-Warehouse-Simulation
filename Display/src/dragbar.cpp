#include <iostream>
#include "../../SDL3.h"
#include "../../Display.h"

/**
 * @brief 拖动条构造函数
 * @param width 拖动条宽度
 * @param height 拖动条高度
 * @param bd_ 边框粗细
 * @param style 样式
 * @param drager 拖动对象
 */
DragBar::DragBar(float width, float height, float bd_, DragBarStyle &style, Ui *drager) : RectUi(width, height, bd_, nullptr, style.fg), process(0.0)
{
    left_fill_bar = new RectUi(0, height - bd_, 0, style.bg1, nullptr);
    rigth_fill_bar = new RectUi(width - bd_, height - bd_, 0, style.bg2, nullptr);
    left_fill_bar->set_rect_midleft(bd_ / 2, height / 2);
    rigth_fill_bar->set_rect_midright(width - bd_ / 2, height / 2);
    Drager = drager;
    left_fill_bar->join(this);
    rigth_fill_bar->join(this);
    // Drager->join(this);
    Drager->handle_mbd_callback = std::bind(&DragBar::press_drager_callback, this, std::placeholders::_1);
    Drager->handle_mbu_callback = std::bind(&DragBar::release_drager_callback, this, std::placeholders::_1);
    Drager->handle_mmv_callback = std::bind(&DragBar::move_drager_callback, this, std::placeholders::_1);
}

void DragBar::press_drager_callback(SDL_MouseButtonEvent &event)
{
    std::cout << event.x << " " << event.y << std::endl;
}

void DragBar::release_drager_callback(SDL_MouseButtonEvent &event)
{
}

void DragBar::move_drager_callback(SDL_MouseMotionEvent &event)
{
}
