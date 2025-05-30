#ifndef __DRAGBAR_H__
#define __DRAGBAR_H__

#include "../../Display.h"

typedef struct
{
    SDL_Color *bg1;    // 填充色1
    SDL_Color *bg2;    // 填充色2
    SDL_Color *fg;     // 前景色
} DragBarStyle;        // 拖动条样式

class DragBar : public RectUi
{
public:
    RectUi *left_fill_bar;     // 左填充条
    RectUi *rigth_fill_bar;    // 右填充条
    Ui *Drager;                // 拖动对象
    double process;            // 进度

    DragBar(float width, float height, float bd_, DragBarStyle &style, Ui *drager);

    void press_drager_callback(SDL_MouseButtonEvent &event);    // 鼠标按下拖动对象回调函数
    void release_drager_callback(SDL_MouseButtonEvent &event);  // 鼠标释放拖动对象回调函数
    void move_drager_callback(SDL_MouseMotionEvent &event);     // 鼠标移动拖动对象回调函数
};


#endif
