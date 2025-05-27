#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#include <list>
#include <string>

#define WINDOW_WIDTH 1920     // 窗口宽度
#define WINDOW_HEIGHT 1080    // 窗口高度
#define FPS_ 60               // 帧率

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_TextEngine *text_engine;

void Display_Init(int w, int h, SDL_WindowFlags flags);
void Display_Quit(void);

void Display_Mainloop(void);

Uint64 fps_clock(void);
void mark_update(void);
void update(Uint64 dt);

typedef void (*UpdateFunc)(Uint64 dt);            // 更新函数类型
extern std::list<UpdateFunc> update_func_list;    // 更新函数列表

/**
 * @brief 显示元素基类
 */
class Ui
{
public:
    Ui(void);
    Ui(const SDL_FRect &rect_);
    Ui(float left, float top, float width, float height);
    virtual ~Ui(void);

    Ui* father;                 // 父元素指针
    std::list<Ui*> kidgroup;    // 子元素链表

    SDL_FRect rect;             // 尺寸矩形

    void (*handle_mbd_callback)(SDL_MouseButtonEvent &event);    // 鼠标按下回调函数
    void (*handle_mbu_callback)(SDL_MouseButtonEvent &event);    // 鼠标抬起回调函数
    void (*handle_mmv_callback)(SDL_MouseMotionEvent &event);    // 鼠标移动回调函数

    virtual void join(Ui &father);                 // 加入父元素链表
    virtual void join(Ui *father);                 // 加入父元素链表
    virtual void leave(void);                      // 离开父元素链表

    virtual void render(float left, float top);    // 渲染函数
    virtual void HandleEvent(SDL_Event &event);    // 事件处理函数

    virtual void set_rect_left(float left);                   // 设置左边距
    virtual void set_rect_right(float right);                 // 设置右边距
    virtual void set_rect_bottom(float bottom);               // 设置下边距
    virtual void set_rect_top(float top);                     // 设置上边距
    virtual void set_rect_width(float width);                 // 设置宽度
    virtual void set_rect_height(float height);               // 设置高度
    virtual void set_rect_size(float width, float height);    // 设置尺寸
    virtual void set_rect_centerx(float x);                   // 设置中心点X坐标
    virtual void set_rect_centery(float y);                   // 设置中心点Y坐标
    virtual void set_rect_center(float x, float y);           // 设置中心点
    virtual void set_rect_topleft(float x, float y);          // 设置左上点
    virtual void set_rect_topright(float x, float y);         // 设置右上点
    virtual void set_rect_bottomleft(float x, float y);       // 设置左下点
    virtual void set_rect_bottomright(float x, float y);      // 设置右下点
    virtual void set_rect_midleft(float x, float y);          // 设置中左点
    virtual void set_rect_midright(float x, float y);         // 设置中右点
    virtual void set_rect_midtop(float x, float y);           // 设置中上点
    virtual void set_rect_midbottom(float x, float y);        // 设置中下点
};
extern Ui *root;    // 根元素指针


/**
 * @brief 背景元素类
 */
class BgUi : public Ui
{
public:
    SDL_Color bg;    // 背景颜色

    BgUi(void);
    BgUi(const SDL_Color &color);
    BgUi(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    virtual void render(float left, float top);               // 渲染函数
};


/**
 * @brief 图像元素类
 */
class ImageUi : public Ui
{
public:
    SDL_FRect cut_rect;      // 切割矩形
    SDL_Texture *texture;    // 纹理指针

    ImageUi(const std::string &path);
    ImageUi(const std::string &path, float left, float top);
    ~ImageUi(void);

    virtual void render(float left, float top);    // 渲染函数
};


class TextUi : public Ui
{
public:
    TTF_Text *text;    // 文本指针

    TextUi(const std::string &text_, TTF_Font *font_, const SDL_Color &color_);
    TextUi(const std::string &text_, TTF_Font *font_, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    TextUi(const std::string &text_, TTF_Font *font_, const SDL_Color &color_, float left, float centery);
    TextUi(const std::string &text_, TTF_Font *font_, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float left, float centery);
    ~TextUi(void);

    virtual void render(float left, float top);    // 渲染函数
};


class RectUi : public Ui
{
public:
    SDL_Color *bg;                // 背景颜色
    SDL_Color *fg;                // 边框颜色
    float bd;                     // 边框宽度
    float radius;                 // 圆角半径
    float top_left_radius;        // 左上角圆角半径
    float top_right_radius;       // 右上角圆角半径
    float bottom_left_radius;     // 左下角圆角半径
    float bottom_right_radius;    // 右下角圆角半径

    RectUi(float width, float height, float bd, SDL_Color *bg, SDL_Color *fg);
    RectUi(float width, float height, float bd, SDL_Color *bg, SDL_Color *fg, float radius);
    RectUi(float width, float height, float bd, SDL_Color *bg, SDL_Color *fg, float topleft_radius, float topright_radius, float bottomleft_radius, float bottomright_radius);

    virtual void render(float left, float top);    // 渲染函数
};


#endif
