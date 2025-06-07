#include <iostream>

#include "../../Display.h"

AnyButtonMouseWidget::AnyButtonMouseWidget(Ui *ui_) : MouseBaseWidget(ui_), func(nullptr) {}
AnyButtonMouseWidget::AnyButtonMouseWidget(Ui &ui_) : MouseBaseWidget(ui_), func(nullptr) {}
AnyButtonMouseWidget::~AnyButtonMouseWidget(void) {}

/**
 * @brief 绑定鼠标点击事件处理函数
 * @param func_ 处理函数指针
 */
 void AnyButtonMouseWidget::bind(void (*func_)(void)) { func = func_; }

/**
 * @brief 鼠标点击事件处理
 */
void AnyButtonMouseWidget::mouseclick(void)
{
    if (func != nullptr) { func(); }
}


/**
 * @brief 按钮构造函数
 * @param width 按钮宽度
 * @param height 按钮高度
 * @param style_ 按钮样式
 */
Button::Button(float width, float height, ButtonStyle &style_) : RectUi(width, height, style_.normal_bd, style_.normal_bg, style_.normal_fg, style_.normal_radius), style(style_), state(BUTTON_STATE_NORMAL)
{
    mousewidget = new ButtonMouseWidget(this);
    mousewidget->activate();
}

/**
 * @brief 按钮析构函数
 */
Button::~Button(void)
{
    if (mousewidget != nullptr)
    {
        mousewidget->deactivate();
        delete mousewidget;
        mousewidget = nullptr;
    }
}

/**
 * @brief 禁用按钮
 */
void Button::disable(void)
{
    mousewidget->deactivate();
    this->set_state(BUTTON_STATE_DISABLE);
}

/**
 * @brief 启用按钮
 */
void Button::enable(void)
{
    mousewidget->activate();
    this->set_state(BUTTON_STATE_NORMAL);
}

/**
 * @brief 绑定鼠标点击事件处理函数
 * @param func_ 处理函数指针
 */
void Button::bind(void (*func_)(void))
{
    if (mousewidget != nullptr) { mousewidget->bind(func_); }
}

/**
 * @brief 设置按钮状态
 * @param state 状态字符串
 */
void Button::set_state(ButtonState state_)
{
    if (state_ == state) { return; }
    switch (state_)
    {
    case BUTTON_STATE_NORMAL:
        this->bg = style.normal_bg;
        this->fg = style.normal_fg;
        this->bd = style.normal_bd;
        this->radius = style.normal_radius;
        break;
    case BUTTON_STATE_HOVER:
        this->bg = style.hover_bg;
        this->fg = style.hover_fg;
        this->bd = style.hover_bd;
        this->radius = style.hover_radius;
        break;
    case BUTTON_STATE_PRESS:
        this->bg = style.press_bg;    
        this->fg = style.press_fg;
        this->bd = style.press_bd;
        this->radius = style.press_radius;
        break;
    case BUTTON_STATE_DISABLE:
        this->bg = style.disable_bg;
        this->fg = style.disable_fg;
        this->bd = style.disable_bd;
        this->radius = style.disable_radius;
        break;
    default:
        break;
    }
    state = state_;
}

ButtonMouseWidget::ButtonMouseWidget(Button *button_) : AnyButtonMouseWidget(button_), button(button_) {}
ButtonMouseWidget::ButtonMouseWidget(Button &button_) : AnyButtonMouseWidget(button_), button(&button_) {}
ButtonMouseWidget::~ButtonMouseWidget(void) {}

/**
 * @brief 鼠标移入事件处理
 */
void ButtonMouseWidget::mousein(void)
{
    if (this->mousedown == SDL_BUTTON_LEFT)
    {
        button->set_state(BUTTON_STATE_PRESS);
    }
    else
    {
        button->set_state(BUTTON_STATE_HOVER);
    }
}

/**
 * @brief 鼠标移出事件处理
 */
void ButtonMouseWidget::mouseout(void)
{
    button->set_state(BUTTON_STATE_NORMAL);
}

/**
 * @brief 鼠标按下事件处理
 */
void ButtonMouseWidget::mousepress(float, float, Uint8)
{
    if (this->mousedown == SDL_BUTTON_LEFT)
    {
        this->button->set_state(BUTTON_STATE_PRESS);
    }
}

/**
 * @brief 鼠标释放事件处理
 */
void ButtonMouseWidget::mouserelease(float, float, Uint8)
{
    if (this->mouseon)
    {
        this->button->set_state(BUTTON_STATE_HOVER);
    }
}
