#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "../../SDL3.h"
#include "../../Display.h"

typedef struct
{
    SDL_Color *normal_bg;
    SDL_Color *normal_fg;
    float normal_bd;
    float normal_radius;

    SDL_Color *hover_bg;
    SDL_Color *hover_fg;
    float hover_bd;
    float hover_radius;

    SDL_Color *press_bg;
    SDL_Color *press_fg;
    float press_bd;
    float press_radius;

    SDL_Color *disable_bg;
    SDL_Color *disable_fg;
    float disable_bd;
    float disable_radius;
} ButtonStyle;         // 按钮样式

enum ButtonState
{
    BUTTON_STATE_NORMAL = 0,   // 正常状态
    BUTTON_STATE_HOVER,         // 悬停状态
    BUTTON_STATE_PRESS,         // 按下状态
    BUTTON_STATE_DISABLE        // 禁用状态
};

class AnyButtonMouseWidget : public MouseBaseWidget
{
public:
    void (* func)(void);

    AnyButtonMouseWidget(Ui *ui_);
    AnyButtonMouseWidget(Ui &ui_);
    virtual ~AnyButtonMouseWidget(void);

    virtual void bind(void (*func_)(void));

    virtual void mouseclick(void);
};

class ButtonMouseWidget;

class Button : public RectUi
{
public:
    ButtonStyle &style;                // 按钮样式

    ButtonMouseWidget *mousewidget;    // 鼠标组件
    ButtonState state;                 // 按钮状态

    Button(float width, float height, ButtonStyle &style_);
    ~Button(void);

    virtual void disable(void);
    virtual void enable(void);
    virtual void bind(void (*func_)(void));
    virtual void set_state(ButtonState state_);
};

class ButtonMouseWidget : public AnyButtonMouseWidget
{
public:
    Button *button;                   // 按钮指针

    ButtonMouseWidget(Button *button_);
    ButtonMouseWidget(Button &button_);
    virtual ~ButtonMouseWidget(void);

    virtual void mousein(void);
    virtual void mouseout(void);
    virtual void mousepress(float x, float y, Uint8 button);
    virtual void mouserelease(float x, float y, Uint8 button);
};

#endif
