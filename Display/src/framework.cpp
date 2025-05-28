#include <list>
#include <string>
#include <iostream>

#include "../../Core.h"
#include "../../SDL3.h"
#include "../../Display.h"

SDL_Window *window = NULL;             // 窗口指针
SDL_Renderer *renderer = NULL;         // 渲染器指针
TTF_TextEngine *text_engine = NULL;    // 文本引擎指针

/**
 * @brief 初始化显示模块
 */
void Display_Init(int w, int h, SDL_WindowFlags flags)
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_CreateWindowAndRenderer(PROGRAM_NAME, w, h, flags, &window, &renderer);
    SDL_SetRenderLogicalPresentation(renderer, w, h, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    text_engine = TTF_CreateRendererTextEngine(renderer);

    SDL_SetWindowIcon(window, IMG_Load("res/icon.png"));
}

/**
 * @brief 退出显示模块
 */
void Display_Quit(void)
{
    TTF_DestroyRendererTextEngine(text_engine);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

bool update_flag = true;                      // 更新标志
Uint64 frame_time_ = (Uint64)(1e9 / FPS_);    // 每帧时间，单位纳秒
Uint64 clock_lime_ = 0;                        // 上一帧时间戳
Ui* root = nullptr;                           // 根元素指针
/**
 * @brief 主循环
 */
void Display_Mainloop(void)
{
    clock_lime_ = SDL_GetTicksNS();
    while (true)
    {
        auto dt = fps_clock();    // 控制帧率

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:                 // 用户关闭窗口
                    goto end;
                default:
                    break;
            }
        }

        update(dt);

        if (update_flag)    // 更新标志为真，表示需要更新界面
        {
            // 渲染元素
            root->render(0, 0);
            // 更新屏幕
            SDL_RenderPresent(renderer);
            update_flag = false;
        }
    }
    end:;
}

/**
 * @brief FPS时钟函数
 * @return 返回两次调用该函数的时间差，单位纳秒
 */
Uint64 fps_clock(void)
{
    auto dt = SDL_GetTicksNS() - clock_lime_;
    if (dt < frame_time_)
    {
        SDL_DelayNS(frame_time_ - dt);
        dt = frame_time_;
    }
    clock_lime_ += dt;
    // std::cout << "FPS: " << (double)1e9 / dt << std::endl;    // 输出FPS
    return dt;
}

/**
 * @brief 标记更新
 * @note 只有主动调用此函数，才会更新界面
 */
void mark_update(void)
{
    if (!update_flag)
    {
        update_flag = true;
    }
}

std::list<UpdateFunc> update_func_list;    // 更新函数列表
/**
 * @brief 更新函数
 * @param dt 时间差，单位纳秒
 */
void update(Uint64 dt)
{
    for (auto &func : update_func_list)
    {
        func(dt);    // 调用每个更新函数
    }
}

/**
 * @brief 初始化显示元素基类
 */
Ui::Ui(void)
{
    this->father = nullptr;
    this->kidgroup.clear();
    this->rect = {0, 0, 0, 0};
    this->handle_mbd_callback = nullptr;
    this->handle_mbu_callback = nullptr;
    this->handle_mmv_callback = nullptr;
}

/**
 * @brief 初始化显示元素基类
 * @param rect_ 矩形
 */
Ui::Ui(const SDL_FRect &rect_)
{
    this->father = nullptr;
    this->kidgroup.clear();
    this->rect = rect_;
    this->handle_mbd_callback = nullptr;
    this->handle_mbu_callback = nullptr;
    this->handle_mmv_callback = nullptr;
}

/**
 * @brief 初始化显示元素基类
 * @param left 左边距
 * @param top 上边距
 * @param width 宽度
 * @param height 高度
 */
Ui::Ui(float left, float top, float width, float height)
{
    this->father = nullptr;
    this->kidgroup.clear();
    this->rect = {left, top, width, height};
    this->handle_mbd_callback = nullptr;
    this->handle_mbu_callback = nullptr;
    this->handle_mmv_callback = nullptr;
}

/**
 * @brief 析构函数
 */
Ui::~Ui(void)
{
    // 离开父元素链表
    if (this->father != nullptr)
    {
        this->leave();
    }
    // 清空子元素链表
    kidgroup.clear();
}

/**
 * @brief 加入父元素链表
 * @param father_ 父元素引用
 */
void Ui::join(Ui &father_)
{
    if (this->father != nullptr) { this->leave(); }
    this->father = &father_;
    father_.kidgroup.push_back(this);
}

/**
 * @brief 加入父元素链表
 * @param father_ 父元素指针
 */
void Ui::join(Ui *father_)
{
    if (this->father != nullptr) { this->leave(); }
    this->father = father_;
    father_->kidgroup.push_back(this);
}

/**
 * @brief 加入父元素链表到指定位置
 * @param father_ 父元素引用
 * @param index 索引位置
 */
void Ui::join(Ui &father_, int index)
{
    if (this->father != nullptr) { this->leave(); }
    this->father = &father_;
    auto it = father_.kidgroup.begin();
    std::advance(it, index);
    father_.kidgroup.insert(it, this);
}

/**
 * @brief 加入父元素链表到指定位置
 * @param father_ 父元素指针
 * @param index 索引位置
 */
void Ui::join(Ui *father_, int index)
{
    if (this->father != nullptr) { this->leave(); }
    this->father = father_;
    auto it = father_->kidgroup.begin();
    std::advance(it, index);
    father_->kidgroup.insert(it, this);
}

/**
 * @brief 离开父元素链表
 */
void Ui::leave(void)
{
    if (this->father == nullptr) { return; }
    this->father->kidgroup.remove(this);
    this->father = nullptr;
}

/**
 * @brief 渲染函数
 * @param left 左边距
 * @param top 上边距
 */
void Ui::render(float left, float top)
{
    left += rect.x;
    top += rect.y;
    for (auto &ui : kidgroup)
    {
        ui->render(left, top);
    }
}

/**
 * @brief 事件处理函数
 * @param event 事件
 */
void Ui::HandleEvent(SDL_Event &event)
{
    // 调用事件处理回调函数
    switch (event.type)
    {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (handle_mbd_callback != nullptr) handle_mbd_callback(event.button);
        event.button.x += rect.x;
        event.button.y += rect.y;
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (handle_mbu_callback != nullptr) handle_mbu_callback(event.button);
        event.button.x += rect.x;
        event.button.y += rect.y;
        break;
    case SDL_EVENT_MOUSE_MOTION:
        if (handle_mmv_callback != nullptr) handle_mmv_callback(event.motion);
        event.motion.x += rect.x;
        event.motion.y += rect.y;
        break;
    default:
        break;
    }
    // 将事件传递给子元素处理
    for (auto &ui : kidgroup)
    {
        ui->HandleEvent(event);
    }
    // 还原事件坐标
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP || event.type == SDL_EVENT_MOUSE_MOTION)
    {
        event.button.x -= rect.x;
        event.button.y -= rect.y;
    }
}

void Ui::set_rect_left(float left)                   { this->rect.x = left; }
void Ui::set_rect_right(float right)                 { this->rect.x = right - this->rect.w; }
void Ui::set_rect_bottom(float bottom)               { this->rect.y = bottom - this->rect.h; }
void Ui::set_rect_top(float top)                     { this->rect.y = top; }
void Ui::set_rect_width(float width)                 { this->rect.w = width; }
void Ui::set_rect_height(float height)               { this->rect.h = height; }
void Ui::set_rect_size(float width, float height)    { this->rect.w = width; this->rect.h = height; }
void Ui::set_rect_centerx(float x)                   { this->rect.x = x - this->rect.w / 2; }
void Ui::set_rect_centery(float y)                   { this->rect.y = y - this->rect.h / 2; }
void Ui::set_rect_center(float x, float y)           { this->rect.x = x - this->rect.w / 2; this->rect.y = y - this->rect.h / 2; }
void Ui::set_rect_topleft(float x, float y)          { this->rect.x = x; this->rect.y = y; }
void Ui::set_rect_topright(float x, float y)         { this->rect.x = x - this->rect.w; this->rect.y = y; }
void Ui::set_rect_bottomleft(float x, float y)       { this->rect.x = x; this->rect.y = y - this->rect.h; }
void Ui::set_rect_bottomright(float x, float y)      { this->rect.x = x - this->rect.w; this->rect.y = y - this->rect.h; }
void Ui::set_rect_midleft(float x, float y)          { this->rect.x = x; this->rect.y = y - this->rect.h / 2; }
void Ui::set_rect_midright(float x, float y)         { this->rect.x = x - this->rect.w; this->rect.y = y - this->rect.h / 2; }
void Ui::set_rect_midtop(float x, float y)           { this->rect.x = x - this->rect.w / 2; this->rect.y = y; }
void Ui::set_rect_midbottom(float x, float y)        { this->rect.x = x - this->rect.w / 2; this->rect.y = y - this->rect.h; }


/**
 * @brief 初始化背景元素
 */
BgUi::BgUi(void) : Ui(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)
{
    Ui({0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});
}

/**
 * @brief 初始化背景元素
 * @param color 颜色
 */
BgUi::BgUi(const SDL_Color &color) : Ui(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)
{
    bg = color;
}

/**
 * @brief 初始化背景元素
 * @param r 红色分量
 * @param g 绿色分量
 * @param b 蓝色分量
 * @param a 透明度分量
 */
BgUi::BgUi(Uint8 r, Uint8 g, Uint8 b, Uint8 a) : Ui(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)
{
    bg = {r, g, b, a};
}

/**
 * @brief 渲染背景元素
 * @param left 左边距
 * @param top 上边距
 */
void BgUi::render(float left, float top)
{
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(renderer, &rect);
    Ui::render(left, top);
}


/**
 * @brief 初始化图像元素
 * @param path 路径
 */
ImageUi::ImageUi(const std::string &path) : Ui(0, 0, 0, 0)
{
    texture = rm.getImage(path);
    cut_rect = {0, 0, (float)texture->w, (float)texture->h};
    set_rect_size((float)texture->w, (float)texture->h);
}

/**
 * @brief 初始化图像元素
 * @param path 路径
 * @param left 左边距
 * @param top 上边距
 */
ImageUi::ImageUi(const std::string &path, float left, float top) : Ui(left, top, 0, 0)
{
    texture = rm.getImage(path);
    cut_rect = {0, 0, (float)texture->w, (float)texture->h};
    set_rect_size((float)texture->w, (float)texture->h);
}

/**
 * @brief 析构函数
 */
ImageUi::~ImageUi(void)
{
    SDL_DestroyTexture(texture);
}

/**
 * @brief 渲染图像元素
 * @param left 左边距
 * @param top 上边距
 */
void ImageUi::render(float left, float top)
{
    rect.x += left;
    rect.y += top;
    SDL_RenderTexture(renderer, texture, &cut_rect, &rect);
    rect.x -= left;
    rect.y -= top;
    Ui::render(left, top);
}


/**
 * @brief 初始化文本元素
 * @param text_ 文本
 * @param font_ 字体
 * @param color_ 颜色
 */
TextUi::TextUi(const std::string &text_, TTF_Font *font_, const SDL_Color &color_) : Ui(0, 0, 0, 0)
{
    text = TTF_CreateText(text_engine, font_, text_.c_str(), 0);
    TTF_SetTextColor(text, color_.r, color_.g, color_.b, color_.a);
    int w, h;
    TTF_GetTextSize(text, &w, &h);
    set_rect_size((float)w, (float)h);
}

/**
 * @brief 初始化文本元素
 * @param text_ 文本
 * @param font_ 字体
 * @param r 红色分量
 * @param g 绿色分量
 * @param b 蓝色分量
 * @param a 透明度分量
 */
TextUi::TextUi(const std::string &text_, TTF_Font *font_, Uint8 r, Uint8 g, Uint8 b, Uint8 a) : Ui(0, 0, 0, 0)
{
    text = TTF_CreateText(text_engine, font_, text_.c_str(), 0);
    TTF_SetTextColor(text, r, g, b, a);
    int w, h;
    TTF_GetTextSize(text, &w, &h);
    set_rect_size((float)w, (float)h);
}

/**
 * @brief 初始化文本元素
 * @param text_ 文本
 * @param font_ 字体
 * @param color_ 颜色
 * @param left 左边距
 * @param centery 中心Y坐标
 */
TextUi::TextUi(const std::string &text_, TTF_Font *font_, const SDL_Color &color_, float left, float centery) : TextUi(text_, font_, color_)
{
    set_rect_midleft(left, centery);
}

/**
 * @brief 初始化文本元素
 * @param text_ 文本
 * @param font_ 字体
 * @param r 红色分量
 * @param g 绿色分量
 * @param b 蓝色分量
 * @param a 透明度分量
 * @param left 左边距
 * @param centery 中心Y坐标
 */
TextUi::TextUi(const std::string &text_, TTF_Font *font_, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float left, float centery) : TextUi(text_, font_, r, g, b, a)
{
    set_rect_midleft(left, centery);
}

/**
 * @brief 析构函数
 */
TextUi::~TextUi(void)
{
    TTF_DestroyText(text);
}

/**
 * @brief 渲染文本元素
 * @param left 左边距
 * @param top 上边距
 */
void TextUi::render(float left, float top)
{
    TTF_DrawRendererText(text, left + rect.x, top + rect.y);
    Ui::render(left, top);
}


/**
 * @brief 初始化矩形元素
 * @param width 宽度
 * @param height 高度
 * @param bd 边框宽度
 * @param bg 背景颜色
 * @param fg 边框颜色
 */
RectUi::RectUi(float width, float height, float bd, SDL_Color *bg, SDL_Color *fg) : Ui(0, 0, width, height), bd(bd), bg(bg), fg(fg), radius(0), top_left_radius(0), top_right_radius(0), bottom_left_radius(0), bottom_right_radius(0) {}

/**
 * @brief 初始化矩形元素
 * @param width 宽度
 * @param height 高度
 * @param bd 边框宽度
 * @param bg 背景颜色
 * @param fg 边框颜色
 * @param radius 圆角半径
 */
RectUi::RectUi(float width, float height, float bd, SDL_Color *bg, SDL_Color *fg, float radius) : Ui(0, 0, width, height), bd(bd), bg(bg), fg(fg), radius(radius), top_left_radius(0), top_right_radius(0), bottom_left_radius(0), bottom_right_radius(0) {}

/**
 * @brief 初始化矩形元素
 * @param width 宽度
 * @param height 高度
 * @param bd 边框宽度
 * @param bg 背景颜色
 * @param fg 边框颜色
 * @param topleft_radius 左上角圆角半径
 * @param topright_radius 右上角圆角半径
 * @param bottomleft_radius 左下角圆角半径
 * @param bottomright_radius 右下角圆角半径
 */
RectUi::RectUi(float width, float height, float bd, SDL_Color *bg, SDL_Color *fg, float topleft_radius, float topright_radius, float bottomleft_radius, float bottomright_radius) : Ui(0, 0, width, height), bd(bd), bg(bg), fg(fg), radius(0), top_left_radius(topleft_radius), top_right_radius(topright_radius), bottom_left_radius(bottomleft_radius), bottom_right_radius(bottomright_radius) {}

/**
 * @brief 渲染矩形元素
 * @param left 左边距
 * @param top 上边距
 */
void RectUi::render(float left, float top)
{
    rect.x += left;
    rect.y += top;
    if (radius == 0)
    {
        if (top_left_radius == 0 && top_right_radius == 0 && bottom_left_radius == 0 && bottom_right_radius == 0)
        {
            Draw_Rect(rect, bd, bg, fg);
        }
        else
        {
            Draw_Rect(rect, bd, bg, fg, top_left_radius, top_right_radius, bottom_left_radius, bottom_right_radius);
        }
    }
    else if (radius != 0)
    {
        Draw_Rect(rect, bd, bg, fg, radius);
    }
    rect.x -= left;
    rect.y -= top;
    Ui::render(left, top);
}
