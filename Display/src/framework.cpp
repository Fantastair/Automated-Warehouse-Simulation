#include <list>
#include <string>
#include <iostream>

#include "../../Core.h"
#include "../../SDL3.h"
#include "../../Display.h"

SDL_Window *window = NULL;             // 窗口指针
SDL_Renderer *renderer = NULL;         // 渲染器指针
TTF_TextEngine *text_engine = NULL;    // 文本引擎指针
bool scaled = false;                   // 是否缩放
SDL_Rect window_rect;                  // 窗口矩形，用于获取显示器边界

/**
 * @brief 初始化显示模块
 */
void Display_Init(int w, int h, SDL_WindowFlags flags)
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_GetDisplayBounds(1, &window_rect);    // 获取显示器边界
    // std::cout << "Display bounds: " << window_rect.x << ", " << window_rect.y << ", " << window_rect.w << ", " << window_rect.h << std::endl;
    int w_, h_;
    if (w != 0 && h != 0)
    {
        w_ = w;
        h_ = h;
    }
    else if (window_rect.w >= 1920 && window_rect.h >= 1080)
    {
        w_ = 1920;
        h_ = 1080;
    }
    else
    {
        w_ = 1280;
        h_ = 720;
        scaled = true;
    }
    SDL_CreateWindowAndRenderer(PROGRAM_NAME, w_, h_, flags, &window, &renderer);
    // std::cout << "Window created with size: " << w_ << "x" << h_ << std::endl;
    if (w != 0 && h != 0)
    {
        SDL_SetRenderLogicalPresentation(renderer, w, h, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    }
    else
    {
        SDL_SetRenderLogicalPresentation(renderer, 1920, 1080, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    }
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
        auto dt = fps_clock();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:                 // 用户关闭窗口
                    goto end;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                case SDL_EVENT_MOUSE_MOTION:
                    if (scaled)
                    {
                        event.button.x *= 1.5;
                        event.button.y *= 1.5;
                    }
                default:
                    root->handle(event);    
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
 * @brief 事件处理函数
 * @param event 事件
 */
void Ui::handle(SDL_Event &event)
{
    if (!kidgroup.empty())
    {
        if (father != nullptr && (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP || event.type == SDL_EVENT_MOUSE_MOTION))
        {
            event.button.x += rect.x;    // 调整事件坐标
            event.button.y += rect.y;    // 调整事件坐标
            for (auto &ui : kidgroup)
            {
                ui->handle(event);    // 处理事件
            }
            event.button.x -= rect.x;    // 恢复事件坐标
            event.button.y -= rect.y;    // 恢复事件坐标
        }
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP || event.type == SDL_EVENT_MOUSE_MOTION)
        {
            for (auto &ui : kidgroup)
            {
                ui->handle(event);    // 处理事件
            }
        }
    }
    if (!widgetgroup.empty())
    {
        for (auto &widget : widgetgroup)
        {
            widget->handle(event);    // 处理组件事件
        }
    }
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
BgUi::BgUi(void) : Ui(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT) {}

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
 * @brief 设置文本内容
 * @param text_ 文本内容
 * @param font_ 字体
 * @param color 文本颜色
 */
void TextUi::set_text(const std::string &text_, TTF_Font *font_, SDL_Color &color)
{
    TTF_DestroyText(text);
    text = TTF_CreateText(text_engine, font_, text_.c_str(), 0);
    TTF_SetTextColor(text, color.r, color.g, color.b, color.a);
    int w, h;
    TTF_GetTextSize(text, &w, &h);
    set_rect_size((float)w, (float)h);
}


/**
 * @brief 初始化矩形元素
 * @param width 宽度
 * @param height 高度
 * @param bd 边框宽度
 * @param bg 背景颜色
 * @param fg 边框颜色
 */
RectUi::RectUi(float width, float height, float bd_, SDL_Color *bg_, SDL_Color *fg_) : Ui(0, 0, width, height), bg(bg_), fg(fg_), bd(bd_), radius(0), top_left_radius(0), top_right_radius(0), bottom_left_radius(0), bottom_right_radius(0) {}

/**
 * @brief 初始化矩形元素
 * @param width 宽度
 * @param height 高度
 * @param bd 边框宽度
 * @param bg 背景颜色
 * @param fg 边框颜色
 * @param radius 圆角半径
 */
RectUi::RectUi(float width, float height, float bd_, SDL_Color *bg_, SDL_Color *fg_, float radius_) : Ui(0, 0, width, height), bg(bg_), fg(fg_), bd(bd_), radius(radius_), top_left_radius(0), top_right_radius(0), bottom_left_radius(0), bottom_right_radius(0) {}

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
RectUi::RectUi(float width, float height, float bd_, SDL_Color *bg_, SDL_Color *fg_, float topleft_radius_, float topright_radius_, float bottomleft_radius_, float bottomright_radius_) : Ui(0, 0, width, height), bg(bg_), fg(fg_), bd(bd_), radius(0), top_left_radius(topleft_radius_), top_right_radius(topright_radius_), bottom_left_radius(bottomleft_radius_), bottom_right_radius(bottomright_radius_) {}

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


/**
 * @brief 组件基类构造函数
 */
Widget::Widget(Ui *ui_) : ui(ui_) {}

/**
 * @brief 组件基类构造函数
 * @param ui_ 显示元素引用
 */
Widget::Widget(Ui &ui_) : ui(&ui_) {}

/**
 * @brief 组件基类析构函数
 */
Widget::~Widget(void) {}

/**
 * @brief 组件激活函数
 * @note 组件必须激活才能接收到事件
 */
void Widget::activate(void)
{
    ui->widgetgroup.push_back(this) ;
}

/**
 * @brief 组件停用函数
 * @note 组件停用后将不再接收到事件
 */
void Widget::deactivate(void)
{
    ui->widgetgroup.remove(this);
}


/**
 * @brief 鼠标组件基类构造函数
 * @param ui_ 显示元素指针
 */
MouseBaseWidget::MouseBaseWidget(Ui *ui_) : Widget(ui_), mousedown(0), mouseon(false) {}

/**
 * @brief 鼠标组件基类构造函数
 * @param ui_ 显示元素引用
 */
MouseBaseWidget::MouseBaseWidget(Ui &ui_) : Widget(ui_), mousedown(0), mouseon(false) {}

/**
 * @brief 鼠标组件基类析构函数
 */
MouseBaseWidget::~MouseBaseWidget(void) {}

/**
 * @brief 鼠标事件处理函数
 * @param event 事件
 */
void MouseBaseWidget::handle(SDL_Event &event)
{
    switch (event.type)
    {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (collide_point(ui->rect, event.button.x, event.button.y) && mousedown == 0) { mousedown = event.button.button; }
        this->mousepress(event.button.x, event.button.y, event.button.button);
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        this->mouserelease(event.button.x, event.button.y, event.button.button);
        if (collide_point(ui->rect, event.button.x, event.button.y) && mousedown != 0) { this->mouseclick(); }
        mousedown = 0;
        break;
    case SDL_EVENT_MOUSE_MOTION:
        if (mouseon)
        {
            if (!collide_point(ui->rect, event.motion.x, event.motion.y))
            {
                mouseon = false;
                this->mouseout();
            }
        }
        else if (collide_point(ui->rect, event.motion.x, event.motion.y))
        {
            mouseon = true;
            this->mousein();
        }
        this->mousemove(event.motion.x, event.motion.y);
        break;
    default:
        break;
    }
}

/**
 * @brief 鼠标按下事件处理函数
 * @param x X坐标
 * @param y Y坐标
 * @param button 鼠标按钮
 */
void MouseBaseWidget::mousepress([[maybe_unused]] float x, [[maybe_unused]] float y, [[maybe_unused]] Uint8 button) {}

/**
 * @brief 鼠标释放事件处理函数
 * @param x X坐标
 * @param y Y坐标
 * @param button 鼠标按钮
 */
void MouseBaseWidget::mouserelease([[maybe_unused]] float x, [[maybe_unused]] float y, [[maybe_unused]] Uint8 button) {}

/**
 * @brief 鼠标单击事件处理函数
 */
void MouseBaseWidget::mouseclick(void) {}

/**
 * @brief 鼠标移出组件事件处理函数
 */
void MouseBaseWidget::mouseout(void) {}

/**
 * @brief 鼠标移入组件事件处理函数
 */
void MouseBaseWidget::mousein(void) {}

/**
 * @brief 鼠标移动事件处理函数
 * @param x X坐标
 * @param y Y坐标
 */
void MouseBaseWidget::mousemove([[maybe_unused]] float x, [[maybe_unused]] float y) {}
