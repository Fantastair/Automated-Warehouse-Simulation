#ifndef __DRAGBAR_H__
#define __DRAGBAR_H__

#include "../../Display.h"

typedef struct
{
    SDL_Color *bg1;    // 填充色1
    SDL_Color *bg2;    // 填充色2
    SDL_Color *fg;     // 前景色
} DragBarStyle;        // 拖动条样式

class DragBarMouseWidget;    // 前向声明拖动条鼠标组件类

class DragBar : public RectUi
{
public:
    RectUi *left_fill_bar;     // 左填充条
    RectUi *rigth_fill_bar;    // 右填充条
    Ui *Drager;                // 拖动对象
    double process;            // 进度

    DragBarMouseWidget *mousewidget;    // 鼠标组件

    DragBar(float width, float height, float bd_, DragBarStyle &style, Ui *drager);

    virtual void set_process(float p);
};

class DragBarMouseWidget : public MouseBaseWidget
{
public:
    DragBarMouseWidget(DragBar *dragbar_);
    DragBarMouseWidget(DragBar &dragbar_);
    virtual ~DragBarMouseWidget(void) override;

    virtual void mousepress(float x, float y, Uint8 button) override;
    virtual void mousemove(float x, float y) override;
};


#endif
