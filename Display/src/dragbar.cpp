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
    Drager->set_rect_center(height / 2, height / 2);
    left_fill_bar->join(this);
    rigth_fill_bar->join(this);
    Drager->join(this);

    mousewidget = new DragBarMouseWidget(this);
    mousewidget->activate();
}

/**
 * @brief 设置拖动条进度
 * @param p 进度值，范围0.0到1.0
 */
void DragBar::set_process(float p)
{
    left_fill_bar->set_rect_width((rect.w - bd) * p);
    left_fill_bar->set_rect_left(bd / 2);
    rigth_fill_bar->set_rect_width((rect.w - bd) * (1 - p));
    rigth_fill_bar->set_rect_right(rect.w - bd / 2);
    process = p;
    Drager->set_rect_centerx(bd / 2 + (rect.w - bd) * p);
}

/**
 * @brief 拖动条鼠标组件构造函数
 * @param dragbar_ 拖动条指针
 */
DragBarMouseWidget::DragBarMouseWidget(DragBar *dragbar_) : MouseBaseWidget(dragbar_) {}

/**
 * @brief 拖动条鼠标组件构造函数
 * @param dragbar_ 拖动条引用
 */
DragBarMouseWidget::DragBarMouseWidget(DragBar &dragbar_) : MouseBaseWidget(dragbar_) {}

/**
 * @brief 拖动条鼠标组件析构函数
 */
DragBarMouseWidget::~DragBarMouseWidget(void) {}

/**
 * @brief 鼠标按下事件处理
 * @param x 鼠标X坐标
 * @param y 鼠标Y坐标
 * @param button 鼠标按键
 */
void DragBarMouseWidget::mousepress(float x, [[maybe_unused]] float y, [[maybe_unused]] Uint8 button)
{
    if (mouseon)
    {
        ((DragBar *)ui)->set_process((x - ui->rect.x - ((DragBar *)ui)->bd / 2) / (ui->rect.w - ((DragBar *)ui)->bd));
    }
}

/**
 * @brief 鼠标移动事件处理
 * @param x 鼠标X坐标
 * @param y 鼠标Y坐标
 */
void DragBarMouseWidget::mousemove(float x, [[maybe_unused]] float y)
{
    if (mousedown == SDL_BUTTON_LEFT)
    {
        if (x < ui->rect.x + ((DragBar *)ui)->bd / 2)
        {
            x = ui->rect.x + ((DragBar *)ui)->bd / 2;
        }
        else if (x > ui->rect.x + ui->rect.w - ((DragBar *)ui)->bd / 2)
        {
            x = ui->rect.x + ui->rect.w - ((DragBar *)ui)->bd / 2;
        }
        float p = (x - ui->rect.x - ((DragBar *)ui)->bd / 2) / (ui->rect.w - ((DragBar *)ui)->bd);
        if (p != ((DragBar *)ui)->process)
        {
            ((DragBar *)ui)->set_process(p);
        }
    }
}
