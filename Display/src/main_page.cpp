#include <iostream>
#include <string>

#include "../../Core.h"
#include "../../SDL3.h"
#include "../../Display.h"

#define GRID_SIZE 60


RectUi *inner_track;
RectUi *outer_track;

class ConnectorUi : public RectUi
{
public:
    int index;              // 接口索引
    static float WIDTH;     // 接口宽度
    static float HEIGHT;    // 接口高度

    ConnectorUi(int i) : RectUi(WIDTH, HEIGHT, 4, nullptr, rm.getColor_(DARKBLUE), 8), index(i)
    {
        TextUi *arrow = nullptr;
        TextUi *index_text = new TextUi(std::to_string(i + 1), rm.getFont("deyi.ttf", 24), rm.getColor(DARKBLUE));
        if (i < 12)
        {
            index_text->set_rect_center(WIDTH / 2, HEIGHT / 4);
            if (i % 2 == 0)
            {
                set_rect_bottomleft(inner_track->rect.x + (CURVE_TRACKRADIUS + CS_1_LEFTPAD + (CONNECTOR_WIDTH * 2 + CS2SC_DISTANCE + SC2CS_DISTANCE) * (i/2)) * ZOOM_FACTOR, outer_track->rect.y - 6);
                arrow = new TextUi(ICON_UP_ARROW, rm.getFont("iconfont.ttf", 32), rm.getColor(DARKBLUE));
            }
            else
            {
                set_rect_bottomleft(inner_track->rect.x + (CURVE_TRACKRADIUS + CS_1_LEFTPAD + (CONNECTOR_WIDTH * 2 + CS2SC_DISTANCE + SC2CS_DISTANCE) * (i/2) + CONNECTOR_WIDTH + CS2SC_DISTANCE) * ZOOM_FACTOR, outer_track->rect.y - 6);
                arrow = new TextUi(ICON_DOWN_ARROW, rm.getFont("iconfont.ttf", 32), rm.getColor(DARKBLUE));
            }
        }
        else if (i < 15)
        {
            index_text->set_rect_center(WIDTH / 2, HEIGHT * 3 / 4); 
            set_rect_topleft(inner_track->rect.x + (CURVE_TRACKRADIUS + PC_18_LEFTPAD + (CONNECTOR_WIDTH + PC2PC_DISTANCE) * (14-i) + CONNECTOR_WIDTH * 3 + PC2PC_DISTANCE * 2 + PC2CP_DISTANCE) * ZOOM_FACTOR, outer_track->rect.y + outer_track->rect.h + 6);
            arrow = new TextUi(ICON_DOWN_ARROW, rm.getFont("iconfont.ttf", 32), rm.getColor(DARKBLUE));
        }
        else
        {
            index_text->set_rect_center(WIDTH / 2, HEIGHT * 3 / 4); 
            set_rect_topleft(inner_track->rect.x + (CURVE_TRACKRADIUS + PC_18_LEFTPAD + (CONNECTOR_WIDTH + PC2PC_DISTANCE) * (17-i)) * ZOOM_FACTOR, outer_track->rect.y + outer_track->rect.h + 6);
            arrow = new TextUi(ICON_UP_ARROW, rm.getFont("iconfont.ttf", 32), rm.getColor(DARKBLUE));
        }
        arrow->set_rect_center(rect.w / 2, rect.h / 2);
        arrow->join(this);
        index_text->join(this);
    }
};
float ConnectorUi::WIDTH = CONNECTOR_WIDTH * ZOOM_FACTOR;
float ConnectorUi::HEIGHT = (CONNECTOR_WIDTH * 2) * ZOOM_FACTOR;
ConnectorUi *connectorui_list[18];

TextUi *system_runtime_text = nullptr;          // 系统运行时间文本
TextUi *simulation_time_text = nullptr;         // 仿真时间文本
TextUi *system_runtime_tip_text = nullptr;      // 系统运行时间提示文本
TextUi *simulation_time_tip_text = nullptr;     // 仿真时间提示文本
TextUi *simulation_speed_tip_text = nullptr;    // 仿真速度提示文本
TextUi *simulation_speed_text = nullptr;        // 仿真速度文本
/**
 * @brief 初始化主页面
 */
void main_page_Init(void)
{
    root = new BgUi(rm.getColor(FAKEWHITE));

    RectUi *grid = nullptr;
    for (int i = 1; i < WINDOW_WIDTH / GRID_SIZE; i++)
    {
        grid = new RectUi(2, 600, 0, rm.getColor_(DEEPWHITE), nullptr);
        grid->set_rect_midtop((float)(i * GRID_SIZE), 0);
        grid->join(root);
    }
    for (int i = 1; i < 600 / GRID_SIZE; i++)
    {
        grid = new RectUi(WINDOW_WIDTH, 2, 0, rm.getColor_(DEEPWHITE), nullptr);
        grid->set_rect_midleft(0, (float)(i * GRID_SIZE));
        grid->join(root);
    }

    RectUi *split_line = new RectUi(WINDOW_WIDTH, 48, 0, rm.getColor_(DARKBLUE), nullptr);
    split_line->set_rect_top(600 - 4);
    split_line->join(root);
    TextUi *text = new TextUi("控制面板", rm.getFont("shuhei.ttf", 32), rm.getColor(FAKEWHITE));
    text->set_rect_center(split_line->rect.w / 2, split_line->rect.h / 2);
    text->join(split_line);
    RectUi *border = new RectUi(WINDOW_WIDTH - 8, WINDOW_HEIGHT - 8, 8, nullptr, rm.getColor_(DARKBLUE));
    border->set_rect_topleft(4, 4);
    border->join(root);

    RectUi *ruler = new RectUi(1000 * ZOOM_FACTOR, 4, 0, rm.getColor_(DARKBLUE), nullptr, 2);
    ruler->set_rect_center(120, 60);
    ruler->join(root);
    RectUi *ruler_lp = new RectUi(4, 12, 0, rm.getColor_(DARKBLUE), nullptr, 2);
    ruler_lp->set_rect_bottomleft(ruler->rect.x, ruler->rect.y + ruler->rect.h);
    ruler_lp->join(root);
    RectUi *ruler_rp = new RectUi(4, 12, 0, rm.getColor_(DARKBLUE), nullptr, 2);
    ruler_rp->set_rect_bottomright(ruler->rect.x + ruler->rect.w, ruler->rect.y + ruler->rect.h);
    ruler_rp->join(root);
    TextUi *ruler_text = new TextUi("1:25", rm.getFont("deyi.ttf", 24), rm.getColor(DARKBLUE));
    ruler_text->set_rect_midtop(ruler->rect.x + ruler->rect.w / 2, ruler->rect.y + ruler->rect.h);
    ruler_text->join(root);
    ruler_text = new TextUi("1m", rm.getFont("deyi.ttf", 24), rm.getColor(DARKBLUE));
    ruler_text->set_rect_midbottom(ruler->rect.x + ruler->rect.w / 2, ruler_rp->rect.y);
    ruler_text->join(root);

    inner_track = new RectUi(
        (STRIGHT_TRACKLENGTH + CURVE_TRACKRADIUS * 2) * ZOOM_FACTOR,
        (CURVE_TRACKRADIUS * 2) * ZOOM_FACTOR,
        4,
        nullptr,
        rm.getColor_(DARKBLUE),
        (CURVE_TRACKRADIUS) * ZOOM_FACTOR
    );
    inner_track->set_rect_center(WINDOW_WIDTH / 2, 600 / 2);
    inner_track->join(root);
    outer_track = new RectUi(
        (STRIGHT_TRACKLENGTH + CURVE_TRACKRADIUS * 2 + TRACKWIDTH * 2) * ZOOM_FACTOR,
        (CURVE_TRACKRADIUS * 2 + TRACKWIDTH * 2) * ZOOM_FACTOR,
        4,
        nullptr,
        rm.getColor_(DARKBLUE),
        (CURVE_TRACKRADIUS + TRACKWIDTH) * ZOOM_FACTOR
    );
    outer_track->set_rect_center(WINDOW_WIDTH / 2, 600 / 2);
    outer_track->join(root);

    RectUi *goods_in_legend = new RectUi(30, 30, 4, rm.getColor_(GREEN), rm.getColor_(DARKBLUE), 4);
    goods_in_legend->set_rect_midtop(120, 480);
    goods_in_legend->join(root);
    RectUi *goods_out_legend = new RectUi(30, 30, 4, rm.getColor_(ORANGE), rm.getColor_(DARKBLUE), 4);
    goods_out_legend->set_rect_midtop(120, 540);
    goods_out_legend->join(root);
    TextUi *goods_in_text = new TextUi("入库货物", rm.getFont("deyi.ttf", 32), rm.getColor(DARKBLUE), goods_in_legend->rect.x + goods_in_legend->rect.w + 8, goods_in_legend->rect.y + goods_in_legend->rect.h / 2);
    goods_in_text->join(root);
    TextUi *goods_out_text = new TextUi("出库货物", rm.getFont("deyi.ttf", 32), rm.getColor(DARKBLUE), goods_out_legend->rect.x + goods_out_legend->rect.w + 8, goods_out_legend->rect.y + goods_out_legend->rect.h / 2);
    goods_out_text->join(root);

    for (int i = 0; i < 18; i++)
    {
        connectorui_list[i] = new ConnectorUi(i);
        connectorui_list[i]->join(root);
    }
    for (int i = 0; i < 7; i++)
    {
        carui_list[i]->init_id_text();
        CarList[i].init();
    }

    for (int i = 0; i < CarNum; i++)
    {
        GenerateRandomTask(CarList[i].task);
    }

    update_car_pos();
    update_func_list.push_back(update_car_pos_func);

    system_runtime_tip_text = new TextUi("系统运行时间：", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE), 24, 688);
    system_runtime_tip_text->join(root);
    simulation_time_tip_text = new TextUi("仿真时间：", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    simulation_time_tip_text->set_rect_midright(system_runtime_tip_text->rect.x + system_runtime_tip_text->rect.w, system_runtime_tip_text->rect.y + system_runtime_tip_text->rect.h + 48);
    simulation_time_tip_text->join(root);
    system_runtime_text = new TextUi("00:00:00.00", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE), system_runtime_tip_text->rect.x + system_runtime_tip_text->rect.w + 8, system_runtime_tip_text->rect.y + system_runtime_tip_text->rect.h / 2);
    system_runtime_text->join(root);
    simulation_time_text = new TextUi("00:00:00.00", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE), simulation_time_tip_text->rect.x + simulation_time_tip_text->rect.w + 8, simulation_time_tip_text->rect.y + simulation_time_tip_text->rect.h / 2);
    simulation_time_text->join(root);

    update_func_list.push_back(update_time_func);

    simulation_speed_tip_text = new TextUi("仿真速度：", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    simulation_speed_tip_text->set_rect_midright(simulation_time_tip_text->rect.x + simulation_time_tip_text->rect.w, simulation_time_tip_text->rect.y + simulation_time_tip_text->rect.h + 48);
    simulation_speed_tip_text->join(root);
    simulation_speed_text = new TextUi(std::to_string(simulation_speed) + "x", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE), simulation_speed_tip_text->rect.x + simulation_speed_tip_text->rect.w + 8, simulation_speed_tip_text->rect.y + simulation_speed_tip_text->rect.h / 2);
    simulation_speed_text->join(root);

    DragBarStyle speed_dragbar_style = {
        rm.getColor_(THEMEBLUE),
        rm.getColor_(DEEPWHITE),
        rm.getColor_(DARKBLUE)
    };
    RectUi *drager = new RectUi(48, 48, 4, rm.getColor_(FAKEWHITE), rm.getColor_(DARKBLUE), 8);
    DragBar *speed_dragbar = new DragBar(320, 20, 4, speed_dragbar_style, (Ui *)&drager);
    speed_dragbar->set_rect_center(400, 800);
    speed_dragbar->join(root);
}


/**
 * @brief 车辆UI类构造函数
 */
CarUi::CarUi(int n) : Ui(0, 0, WIDTH, HEIGHT), num(n), angle(0)
{
    surface = SDL_CreateSurface((int)WIDTH, (int)HEIGHT, SDL_PIXELFORMAT_RGBA32);
    progress_bar = new RectUi(WIDTH - 16, 0, 0, nullptr, nullptr);
    progress_bar->set_rect_center(WIDTH / 2, HEIGHT / 2);
}

/**
 * @brief 车辆UI类析构函数
 */
CarUi::~CarUi(void)
{
    if (texture) SDL_DestroyTexture(texture);
}

/**
 * @brief 初始化车辆编号文本
 */
void CarUi::init_id_text(void)
{
    id_text = new TextUi(std::to_string(num + 1), rm.getFont("deyi.ttf", 24), rm.getColor(DARKBLUE));
    id_text->set_rect_center(WIDTH / 2, HEIGHT / 2);
    id_text->join(this);
}

/**
 * @brief 更新纹理
 */
void CarUi::update_texture(void)
{
    if (texture) SDL_DestroyTexture(texture);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
}

/**
 * @brief 更新表面
 */
void CarUi::update_surface(void)
{
    SDL_UnlockSurface(surface);

    SDL_FillSurfaceRect(surface, nullptr, SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), nullptr, 0, 0, 0, 0));

    // 绘制车辆
    Draw_Rect_Surface(surface, progress_bar->rect, 0, progress_bar->bg, nullptr);
    Draw_Rect_Surface(surface, {0, 0, WIDTH, HEIGHT}, 8, nullptr, rm.getColor_(THEMEBLUE), 8);

    SDL_LockSurface(surface);
}

/**
 * @brief 更新进度条
 */
void CarUi::update_progress(void)
{
    if (CarList[num].task.task_type == 0 && progress_bar->bg != rm.getColor_(GREEN))
    {
        progress_bar->bg = rm.getColor_(GREEN);
    }
    else if (CarList[num].task.task_type == 1 && progress_bar->bg != rm.getColor_(ORANGE))
    {
        progress_bar->bg = rm.getColor_(ORANGE);
    }
    if (CarList[num].state == CarState::CarGetting)
    {
        progress_bar->set_rect_height(static_cast<float>((HEIGHT - 14) * static_cast<double>(CarList[num].time_temp) / LOADTIME));
        progress_bar->set_rect_top(7);
    }
    else if (CarList[num].state == CarState::CarPutting)
    {
        progress_bar->set_rect_height(static_cast<float>((HEIGHT - 14) * (1 - static_cast<double>(CarList[num].time_temp) / LOADTIME)));
        progress_bar->set_rect_top(7);
    }
}

/**
 * @brief 渲染函数
 * @param left 左边距
 * @param top 上边距
 */
void CarUi::render(float left, float top)
{
    rect.x += left;
    rect.y += top;
    update_progress();
    update_surface();
    update_texture();
    SDL_RenderTextureRotated(renderer, texture, nullptr, &rect, angle, NULL, SDL_FLIP_NONE);
    rect.x -= left;
    rect.y -= top;
    Ui::render(left, top);
}
float CarUi::WIDTH = CARLENGTH * ZOOM_FACTOR;
float CarUi::HEIGHT = TRACKWIDTH * ZOOM_FACTOR;


CarUi *carui_list[7] = {
    new CarUi(0),
    new CarUi(1),
    new CarUi(2),
    new CarUi(3),
    new CarUi(4),
    new CarUi(5),
    new CarUi(6)
};
/**
 * @brief 更新穿梭车位置
 */
void update_car_pos(void)
{
    for (int i = 0; i < 7; i++)
    {
        if (i < CarNum)
        {
            if (carui_list[i]->father == nullptr)
            {
                carui_list[i]->join(root);
            }
            int track_index = GetTrackIndex(CarList[i].position);
            switch (track_index)
            {
                float angle;
            case 0:
                if (carui_list[i]->angle != 0) { carui_list[i]->angle = 0; }
                carui_list[i]->set_rect_center(
                    (static_cast<float>(CarList[i].position) + CURVE_TRACKRADIUS) * ZOOM_FACTOR + inner_track->rect.x,
                    (inner_track->rect.y + outer_track->rect.y) / 2
                );
                break;
            case 1:
                angle = static_cast<float>(arc2angle(CarList[i].position - TrackSplit[1], CURVE_TRACKRADIUS + TRACKWIDTH / 2));
                carui_list[i]->angle = 180 + angle2degree(angle);
                carui_list[i]->set_rect_center(
                    inner_track->rect.x + inner_track->rect.w - CURVE_TRACKRADIUS * ZOOM_FACTOR + SDL_sinf(angle) * ((CURVE_TRACKRADIUS + TRACKWIDTH / 2) * ZOOM_FACTOR - 0),
                    inner_track->rect.y + inner_track->rect.h / 2 - ((CURVE_TRACKRADIUS + TRACKWIDTH / 2) * ZOOM_FACTOR - 0) * SDL_cosf(angle)
                );
                break;
            case 2:
                if (carui_list[i]->angle != 180) { carui_list[i]->angle = 180; }
                carui_list[i]->set_rect_center(
                    inner_track->rect.x + inner_track->rect.w - (static_cast<float>(CarList[i].position - TrackSplit[2]) + CURVE_TRACKRADIUS) * ZOOM_FACTOR,
                    (inner_track->rect.y + inner_track->rect.h + outer_track->rect.y + outer_track->rect.h) / 2
                );
                break;
            case 3:
                angle = static_cast<float>(arc2angle(CarList[i].position - TrackSplit[3], CURVE_TRACKRADIUS + TRACKWIDTH / 2));
                carui_list[i]->angle = angle2degree(angle);
                carui_list[i]->set_rect_center(
                    inner_track->rect.x + CURVE_TRACKRADIUS * ZOOM_FACTOR - SDL_sinf(angle) * ((CURVE_TRACKRADIUS + TRACKWIDTH / 2) * ZOOM_FACTOR - 0),
                    inner_track->rect.y + inner_track->rect.h / 2 + ((CURVE_TRACKRADIUS + TRACKWIDTH / 2) * ZOOM_FACTOR - 0) * SDL_cosf(angle)
                );
                break;
            default:
                break;
            }
        }
        else
        {
            if (carui_list[i]->father != nullptr)
            {
                carui_list[i]->leave();
            }
            continue;
        }
    }
}

/**
 * @brief 更新穿梭车位置
 * @param dt 时间差，单位纳秒
 */
void update_car_pos_func(Uint64)
{
    if (Simulating)
    {
        update_car_pos();
        mark_update();
    }
}

/**
 * @brief 更新时间文本
 */
void update_time_func(Uint64)
{
    SDL_Color c = rm.getColor(DARKBLUE);
    system_runtime_text->set_text(NS2String(system_runtime), rm.getFont("deyi.ttf", 48), c);
    system_runtime_text->set_rect_midleft(system_runtime_tip_text->rect.x + system_runtime_tip_text->rect.w + 8, system_runtime_tip_text->rect.y + system_runtime_tip_text->rect.h / 2);
    if (Simulating)
    {
        simulation_time_text->set_text(NS2String(simulation_time), rm.getFont("deyi.ttf", 48), c);
        simulation_time_text->set_rect_midleft(simulation_time_tip_text->rect.x + simulation_time_tip_text->rect.w + 8, simulation_time_tip_text->rect.y + simulation_time_tip_text->rect.h / 2);
    }
}
