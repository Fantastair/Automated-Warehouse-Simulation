#include <iostream>
#include <string>

#include "../../Core.h"
#include "../../SDL3.h"
#include "../../Display.h"

#define GRID_SIZE 60
#define INCOLOR GREEN
#define OUTCOLOR ORANGE

ButtonStyle NORMAL_BUTTON_STYLE;

RectUi *inner_track;
RectUi *outer_track;

float left_limit;
float right_limit;

ConnectorUi::ConnectorUi(int i) : RectUi(WIDTH, HEIGHT, 4, nullptr, rm.getColor_(DARKBLUE), 8), index(i)
{
    fill_rect = new RectUi(WIDTH - 4, 0, 0, nullptr, nullptr, radius - bd / 2);
    fill_rect->join(this);
    if ((i < 12 && i % 2 == 0) || i >= 15)
    {
        fill_rect->bg = rm.getColor_(INCOLOR);
    }
    else
    {
        fill_rect->bg = rm.getColor_(OUTCOLOR);
    }
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

    card = new ConnectorInfoCard(i);
    mousewidget = new ConnectorUiMouseWidget(this);
    mousewidget->activate();
}

ConnectorUi::~ConnectorUi(void)
{
    delete mousewidget;
}

void ConnectorUi::render(float left, float top)
{
    switch (ConnectorList[index].state)
    {
    case ConnectorState::ConnectorIdle:
        if (fill_rect->rect.h != 0)
        {
            fill_rect->set_rect_height(0);
        }
        break;
    case ConnectorState::WaitForCar:
        break;
    case ConnectorState::WorkWithCar:
        if (ConnectorList[index].type == ConnectorType::CarToStoration || ConnectorList[index].type == ConnectorType::CarToPerson)
        {
            fill_rect->set_rect_height((HEIGHT - bd) * ConnectorList[index].time_temp / LOADTIME);
        }
        else
        {
            fill_rect->set_rect_height((HEIGHT - bd) * (LOADTIME - ConnectorList[index].time_temp) / LOADTIME);
        }
        if (index < 12)
        {
            fill_rect->set_rect_bottomleft(bd / 2, HEIGHT - bd / 2);
        }
        else
        {
            fill_rect->set_rect_topleft(bd / 2, bd / 2);
        }
        break;
    case ConnectorState::WorkWithOther:
        if (ConnectorList[index].type == ConnectorType::CarToStoration || ConnectorList[index].type == ConnectorType::CarToPerson)
        {
            fill_rect->set_rect_height((HEIGHT - bd) * (ConnectorList[index].get_load_time() - ConnectorList[index].time_temp) / ConnectorList[index].get_load_time());
        }
        else
        {
            fill_rect->set_rect_height((HEIGHT - bd) * ConnectorList[index].time_temp / ConnectorList[index].get_load_time());
        }
        if (index < 12)
        {
            fill_rect->set_rect_topleft(bd / 2, bd / 2);
        }
        else
        {
            fill_rect->set_rect_bottomleft(bd / 2, HEIGHT - bd / 2);
        }
        break;
    case ConnectorState::CallingCar:
        break;
    default:
        break;
    }
    RectUi::render(left, top);
}

float ConnectorUi::WIDTH = CONNECTOR_WIDTH * ZOOM_FACTOR;
float ConnectorUi::HEIGHT = (CONNECTOR_WIDTH * 2) * ZOOM_FACTOR;
ConnectorUi *connectorui_list[18];

TextUi *system_runtime_text = nullptr;              // 系统运行时间文本
TextUi *simulation_time_text = nullptr;             // 仿真时间文本
TextUi *system_runtime_tip_text = nullptr;          // 系统运行时间提示文本
TextUi *simulation_time_tip_text = nullptr;         // 仿真时间提示文本
TextUi *simulation_speed_tip_text = nullptr;        // 仿真速度提示文本
TextUi *simulation_speed_text = nullptr;            // 仿真速度文本
SpeedDragBar *speed_dragbar = nullptr;              // 仿真速度拖动条
TextUi *switch_simulation_button_text = nullptr;    // 开始/暂停仿真按钮文本
Button *switch_simulation_button = nullptr;         // 开始/暂停仿真按钮
Button *reset_simulation_button = nullptr;          // 重置仿真按钮
Button *random_task_button = nullptr;               // 随机执行任务按钮
Button *file_task_button = nullptr;                 // 从文件导入任务按钮
Button *car_num_button = nullptr;                   // 设置穿梭车数量按钮
TextUi *car_num_button_text = nullptr;                     // 穿梭车数量文本

CarInfoBar *CIC_List[7];

/**
 * @brief 初始化主页面
 */
void main_page_Init(void)
{
    NORMAL_BUTTON_STYLE = {
        rm.getColor_(FAKEWHITE), rm.getColor_(DARKBLUE), 2, 16,
        rm.getColor_(DEEPWHITE), rm.getColor_(DARKBLUE), 4, 16,
        rm.getColor_(GRAY), rm.getColor_(DARKBLUE), 4, 16,
        rm.getColor_(GRAY), rm.getColor_(GRAY), 2, 16
    };

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
    text->set_rect_center(280, split_line->rect.h / 2);
    text->join(split_line);
    text = new TextUi("车辆列表", rm.getFont("shuhei.ttf", 32), rm.getColor(FAKEWHITE));
    text->set_rect_center(930, split_line->rect.h / 2);
    text->join(split_line);
    text = new TextUi("详细信息", rm.getFont("shuhei.ttf", 32), rm.getColor(FAKEWHITE));
    text->set_rect_center(1610, split_line->rect.h / 2);
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
    left_limit = outer_track->rect.x + outer_track->radius;
    right_limit = outer_track->rect.x + outer_track->rect.w - outer_track->radius;

    RectUi *goods_in_legend = new RectUi(30, 30, 4, rm.getColor_(GREEN), rm.getColor_(DARKBLUE), 4);
    goods_in_legend->set_rect_midtop(1715, 45);
    goods_in_legend->join(root);
    RectUi *goods_out_legend = new RectUi(30, 30, 4, rm.getColor_(ORANGE), rm.getColor_(DARKBLUE), 4);
    goods_out_legend->set_rect_midtop(1715, 90);
    goods_out_legend->join(root);
    TextUi *goods_in_text = new TextUi("入库货物", rm.getFont("deyi.ttf", 32), rm.getColor(DARKBLUE), goods_in_legend->rect.x + goods_in_legend->rect.w + 8, goods_in_legend->rect.y + goods_in_legend->rect.h / 2);
    goods_in_text->join(root);
    TextUi *goods_out_text = new TextUi("出库货物", rm.getFont("deyi.ttf", 32), rm.getColor(DARKBLUE), goods_out_legend->rect.x + goods_out_legend->rect.w + 8, goods_out_legend->rect.y + goods_out_legend->rect.h / 2);
    goods_out_text->join(root);

    for (int i = 0; i < 18; i++)
    {
        connectorui_list[i] = new ConnectorUi(i);
        connectorui_list[i]->join(root);
        ConnectorList[i].init();
    }
    for (int i = 0; i < 7; i++)
    {
        carui_list[i]->init_id_text();
        CarList[i].init();
    }

    update_car_pos();
    update_func_list.push_back(update_car_pos_func);

    system_runtime_tip_text = new TextUi("系统运行时间：", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE), 24, 688);
    system_runtime_tip_text->join(root);
    simulation_time_tip_text = new TextUi("仿真时间：", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    simulation_time_tip_text->set_rect_midright(system_runtime_tip_text->rect.x + system_runtime_tip_text->rect.w, system_runtime_tip_text->rect.y + system_runtime_tip_text->rect.h + 48);
    simulation_time_tip_text->join(root);
    system_runtime_text = new TextUi("00:00:00.0", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE), system_runtime_tip_text->rect.x + system_runtime_tip_text->rect.w + 8, system_runtime_tip_text->rect.y + system_runtime_tip_text->rect.h / 2);
    system_runtime_text->join(root);
    simulation_time_text = new TextUi("00:00:00.0", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE), simulation_time_tip_text->rect.x + simulation_time_tip_text->rect.w + 8, simulation_time_tip_text->rect.y + simulation_time_tip_text->rect.h / 2);
    simulation_time_text->join(root);

    update_func_list.push_back(update_time_func);

    simulation_speed_tip_text = new TextUi("仿真速度：", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    simulation_speed_tip_text->set_rect_midright(simulation_time_tip_text->rect.x + simulation_time_tip_text->rect.w, simulation_time_tip_text->rect.y + simulation_time_tip_text->rect.h + 48);
    simulation_speed_tip_text->join(root);
    simulation_speed_text = new TextUi(std::to_string(simulation_speed) + "x", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE), simulation_speed_tip_text->rect.x + simulation_speed_tip_text->rect.w + 8, simulation_speed_tip_text->rect.y + simulation_speed_tip_text->rect.h / 2);
    simulation_speed_text->join(root);

    DragBarStyle speed_dragbar_style = { rm.getColor_(THEMEBLUE), rm.getColor_(DEEPWHITE), rm.getColor_(DARKBLUE) };
    RectUi *drager = new RectUi(48, 48, 4, rm.getColor_(FAKEWHITE), rm.getColor_(DARKBLUE), 16);
    RectUi *drager_dot = new RectUi(24, 24, 0, rm.getColor_(THEMEBLUE), nullptr, 8);
    drager_dot->set_rect_center(drager->rect.w / 2, drager->rect.h / 2);
    drager_dot->join(drager);
    
    speed_dragbar = new SpeedDragBar(480, 20, 4, speed_dragbar_style, (Ui *)drager);
    speed_dragbar->set_rect_center(280, 894);
    speed_dragbar->set_process((simulation_speed - 0.5f) / (32 - 0.5f)); // 设置初始进度

    AnyButtonMouseWidget *fast_set_speed_button = nullptr;
    RectUi *scale_line = nullptr;

    TextUi *fast_set_speed_text_1x = new TextUi("1x", rm.getFont("deyi.ttf", 24), rm.getColor(DARKBLUE));
    fast_set_speed_text_1x->set_rect_center(speed_dragbar->rect.x + speed_dragbar->bd / 2 + (speed_dragbar->rect.w - speed_dragbar->bd) * (1 - 0.5f) / (32 - 0.5f), speed_dragbar->rect.y + speed_dragbar->rect.h + 32);
    fast_set_speed_text_1x->join(root);
    scale_line = new RectUi(2, 16, 0, rm.getColor_(DARKBLUE), nullptr);
    scale_line->set_rect_midtop(fast_set_speed_text_1x->rect.x + fast_set_speed_text_1x->rect.w / 2, speed_dragbar->rect.y + speed_dragbar->rect.h);
    scale_line->join(root);
    fast_set_speed_button = new AnyButtonMouseWidget(fast_set_speed_text_1x);
    fast_set_speed_button->bind(set_simulation_speed_1x);
    fast_set_speed_button->activate();

    TextUi *fast_set_speed_text_4x = new TextUi("4x", rm.getFont("deyi.ttf", 24), rm.getColor(DARKBLUE));
    fast_set_speed_text_4x->set_rect_center(speed_dragbar->rect.x + speed_dragbar->bd / 2 + (speed_dragbar->rect.w - speed_dragbar->bd) * (4 - 0.5f) / (32 - 0.5f), speed_dragbar->rect.y + speed_dragbar->rect.h + 32);
    fast_set_speed_text_4x->join(root);
    scale_line = new RectUi(2, 16, 0, rm.getColor_(DARKBLUE), nullptr);
    scale_line->set_rect_midtop(fast_set_speed_text_4x->rect.x + fast_set_speed_text_4x->rect.w / 2, speed_dragbar->rect.y + speed_dragbar->rect.h);
    scale_line->join(root);
    fast_set_speed_button = new AnyButtonMouseWidget(fast_set_speed_text_4x);
    fast_set_speed_button->bind(set_simulation_speed_4x);
    fast_set_speed_button->activate();

    TextUi *fast_set_speed_text_8x = new TextUi("8x", rm.getFont("deyi.ttf", 24), rm.getColor(DARKBLUE));
    fast_set_speed_text_8x->set_rect_center(speed_dragbar->rect.x + speed_dragbar->bd / 2 + (speed_dragbar->rect.w - speed_dragbar->bd) * (8 - 0.5f) / (32 - 0.5f), speed_dragbar->rect.y + speed_dragbar->rect.h + 32);
    fast_set_speed_text_8x->join(root);
    scale_line = new RectUi(2, 16, 0, rm.getColor_(DARKBLUE), nullptr);
    scale_line->set_rect_midtop(fast_set_speed_text_8x->rect.x + fast_set_speed_text_8x->rect.w / 2, speed_dragbar->rect.y + speed_dragbar->rect.h);
    scale_line->join(root);
    fast_set_speed_button = new AnyButtonMouseWidget(fast_set_speed_text_8x);
    fast_set_speed_button->bind(set_simulation_speed_8x);
    fast_set_speed_button->activate();

    TextUi *fast_set_speed_text_16x = new TextUi("16x", rm.getFont("deyi.ttf", 24), rm.getColor(DARKBLUE));
    fast_set_speed_text_16x->set_rect_center(speed_dragbar->rect.x + speed_dragbar->bd / 2 + (speed_dragbar->rect.w - speed_dragbar->bd) * (16 - 0.5f) / (32 - 0.5f), speed_dragbar->rect.y + speed_dragbar->rect.h + 32);
    fast_set_speed_text_16x->join(root);
    scale_line = new RectUi(2, 16, 0, rm.getColor_(DARKBLUE), nullptr);
    scale_line->set_rect_midtop(fast_set_speed_text_16x->rect.x + fast_set_speed_text_16x->rect.w / 2, speed_dragbar->rect.y + speed_dragbar->rect.h);
    scale_line->join(root);
    fast_set_speed_button = new AnyButtonMouseWidget(fast_set_speed_text_16x);
    fast_set_speed_button->bind(set_simulation_speed_16x);
    fast_set_speed_button->activate();

    TextUi *fast_set_speed_text_32x = new TextUi("32x", rm.getFont("deyi.ttf", 24), rm.getColor(DARKBLUE));
    fast_set_speed_text_32x->set_rect_center(speed_dragbar->rect.x + speed_dragbar->bd / 2 + (speed_dragbar->rect.w - speed_dragbar->bd) * (32 - 0.5f) / (32 - 0.5f), speed_dragbar->rect.y + speed_dragbar->rect.h + 32);
    fast_set_speed_text_32x->join(root);
    scale_line = new RectUi(2, 16, 0, rm.getColor_(DARKBLUE), nullptr);
    scale_line->set_rect_midtop(fast_set_speed_text_32x->rect.x + fast_set_speed_text_32x->rect.w / 2, speed_dragbar->rect.y + speed_dragbar->rect.h);
    scale_line->join(root);
    fast_set_speed_button = new AnyButtonMouseWidget(fast_set_speed_text_32x);
    fast_set_speed_button->bind(set_simulation_speed_32x);
    fast_set_speed_button->activate();

    speed_dragbar->join(root);

    switch_simulation_button = new Button(64, 64, NORMAL_BUTTON_STYLE);
    switch_simulation_button->set_rect_center(80, 1000);
    switch_simulation_button->join(root);
    switch_simulation_button_text = new TextUi(ICON_PLAY, rm.getFont("iconfont.ttf", 32), rm.getColor(DARKBLUE));
    switch_simulation_button_text->set_rect_center(switch_simulation_button->rect.w / 2, switch_simulation_button->rect.h / 2);
    switch_simulation_button_text->join(switch_simulation_button);
    switch_simulation_button->bind(switch_simulation);

    reset_simulation_button = new Button(64, 64, NORMAL_BUTTON_STYLE);
    reset_simulation_button->set_rect_center(switch_simulation_button->rect.x + switch_simulation_button->rect.w / 2 + 96, 1000);
    reset_simulation_button->join(root);
    RectUi *reset_simulation_button_text = new RectUi(32, 32, 0, rm.getColor_(DARKBLUE), nullptr, 8);
    reset_simulation_button_text->set_rect_center(reset_simulation_button->rect.w / 2, reset_simulation_button->rect.h / 2);
    reset_simulation_button_text->join(reset_simulation_button);
    reset_simulation_button->bind(reset_simulation);

    random_task_button = new Button(64, 64, NORMAL_BUTTON_STYLE);
    random_task_button->set_rect_center(reset_simulation_button->rect.x + reset_simulation_button->rect.w / 2 + 96, 1000);
    random_task_button->join(root);
    TextUi *random_task_button_text = new TextUi(ICON_RANDOM, rm.getFont("iconfont.ttf", 32), rm.getColor(DARKBLUE));
    random_task_button_text->set_rect_center(random_task_button->rect.w / 2, random_task_button->rect.h / 2);
    random_task_button_text->join(random_task_button);
    random_task_button->bind(random_task);

    file_task_button = new Button(64, 64, NORMAL_BUTTON_STYLE);
    file_task_button->set_rect_center(random_task_button->rect.x + random_task_button->rect.w / 2 + 96, 1000);
    file_task_button->join(root);
    TextUi *file_task_button_text = new TextUi(ICON_FILE, rm.getFont("iconfont.ttf", 32), rm.getColor(DARKBLUE));
    file_task_button_text->set_rect_center(file_task_button->rect.w / 2, file_task_button->rect.h / 2);
    file_task_button_text->join(file_task_button);
    file_task_button->bind(file_task);

    car_num_button = new Button(64, 64, NORMAL_BUTTON_STYLE);
    car_num_button->set_rect_center(file_task_button->rect.x + file_task_button->rect.w / 2 + 96, 1000);
    car_num_button->join(root);
    car_num_button_text = new TextUi("3", rm.getFont("shuhei.ttf", 32), rm.getColor(DARKBLUE));
    car_num_button_text->set_rect_center(car_num_button->rect.w / 2, car_num_button->rect.h / 2);
    car_num_button_text->join(car_num_button);
    car_num_button->bind(switch_car_num);

    float t = split_line->rect.y + split_line->rect.h;
    split_line = new RectUi(8, WINDOW_HEIGHT - t - 8, 0, rm.getColor_(DARKBLUE), nullptr);
    split_line->set_rect_midtop(560, t);
    split_line->join(root);

    update_func_list.push_back(check_task_over_func);

    for (int i = 0; i < 7; i++)
    {
        CIC_List[i] = new CarInfoBar(i);
        CIC_List[i]->set_rect_midleft(split_line->rect.x + split_line->rect.w + 16, t - 24 + (i + 1) * 420 / 7);
    }
    CIC_List[0]->join(root);
    CIC_List[1]->join(root);
    CIC_List[2]->join(root);

    split_line = new RectUi(8, WINDOW_HEIGHT - t - 8, 0, rm.getColor_(DARKBLUE), nullptr);
    split_line->set_rect_midtop(1300, t);
    split_line->join(root);

    // update_func_list.push_back(check_speed_func);
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
    id_text = new TextUi("C " + std::to_string(num + 1), rm.getFont("deyi.ttf", 25), rm.getColor(DARKBLUE));
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
    Draw_Rect_Surface(surface, {0, 0, WIDTH, HEIGHT}, 8, nullptr, rm.getColor_((CarList[num].state != CarState::CarIdle && CarList[num].state != CarState::CarInit) ? THEMEBLUE : GRAY), 8);

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
        progress_bar->set_rect_height(static_cast<float>((HEIGHT - 12) * static_cast<float>(CarList[num].time_temp) / LOADTIME));
        progress_bar->set_rect_top(7);
    }
    else if (CarList[num].state == CarState::CarPutting)
    {
        progress_bar->set_rect_height(static_cast<float>((HEIGHT - 12) * (1 - static_cast<float>(CarList[num].time_temp) / LOADTIME)));
        progress_bar->set_rect_top(7);
    }
    else if (CarList[num].state == CarState::CarIdle && progress_bar->rect.h != 0)
    {
        progress_bar->set_rect_height(0);
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

SpeedDragBar::SpeedDragBar(float width, float height, float bd_, DragBarStyle &style, Ui *drager) : DragBar(width, height, bd_, style, drager) {}

void SpeedDragBar::set_process(float p)
{
    DragBar::set_process(p);
    set_simulation_speed(0.5f + (32 - 0.5f) * p);
    SDL_Color c = rm.getColor(DARKBLUE);
    std::string speed_text = std::to_string(simulation_speed);
    if (speed_text.find('.') == std::string::npos)
    {
        speed_text += ".00"; // 如果没有小数点，添加两位小数
    }
    else
    {
        size_t dot_pos = speed_text.find('.');
        if (dot_pos + 3 == speed_text.size())
        {
            speed_text += "0"; // 如果小数点后只有一位，添加一位
        }
        else if (dot_pos + 4 < speed_text.size())
        {
            speed_text = speed_text.substr(0, dot_pos + 4); // 保留两位小数
        }
    }
    simulation_speed_text->set_text(speed_text + "x", rm.getFont("deyi.ttf", 48), c);
    simulation_speed_text->set_rect_midleft(simulation_speed_tip_text->rect.x + simulation_speed_tip_text->rect.w + 8, simulation_speed_tip_text->rect.y + simulation_speed_tip_text->rect.h / 2);
}

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
    mark_update();
}

/**
 * @brief 开始/暂停仿真
 */
void switch_simulation(void)
{
    SDL_Color c = rm.getColor(DARKBLUE);
    if (Simulating)
    {
        Simulating = false;
        switch_simulation_button_text->set_text(ICON_PLAY, rm.getFont("iconfont.ttf", 32), c);
        random_task_button->enable();
        file_task_button->enable();
        car_num_button->enable();
    }
    else
    {
        Simulating = true;
        switch_simulation_button_text->set_text(ICON_PAUSE, rm.getFont("iconfont.ttf", 32), c);
        random_task_button->disable();
        file_task_button->disable();
        car_num_button->disable();
    }
}

std::list<std::string> message_list;

/**
 * @brief 重置仿真
 */
void reset_simulation(void)
{
    message_list.clear();
    SDL_Color c = rm.getColor(DARKBLUE);
    Simulating = false;
    simulation_time = 0;
    switch_simulation_button_text->set_text(ICON_PLAY, rm.getFont("iconfont.ttf", 32), c);
    for (int i = 0; i < 7; i++)
    {
        last_speeds[i] = 0;
        last_run_states[i] = CarRunState::CarUniform;
        CarList[i].state = CarState::CarIdle;
        CarList[i].speed = 0;
        CarList[i].position = CarList[i].initial_pos;
        CarList[i].task = { -1, -1, -1, -1 };
        CarList[i].time_temp = 0;
        CarList[i].maxspeed_curve = MAXSPEED_CURVE;
        CarList[i].maxspeed_stright = MAXSPEED_STRIGHT;

        CarList[i].CarIdleTime = 0;
        CarList[i].CarToGetTime = 0;
        CarList[i].CarGettingTime = 0;
        CarList[i].CarToPutTime = 0;
        CarList[i].CarWaitToPutTime = 0;
        CarList[i].CarPuttingTime = 0;
        CarList[i].CarInitTime = 0;
        CarList[i].CarWaitingTime = 0;
        CarList[i].CarStopCount = 0;
        CarList[i].run_state = CarRunState::CarUniform;
        CarList[i].in_task_count = 0;
        CarList[i].out_task_count = 0;
    }
    for (int i = 0; i < 18; i++)
    {
        ConnectorList[i].state = ConnectorState::ConnectorIdle;
        ConnectorList[i].time_temp = 0;
        ConnectorList[i].task_list.clear();

        ConnectorList[i].ConnectorIdleTime = 0;
        ConnectorList[i].ConnectorWaitForCarTime = 0;
        ConnectorList[i].ConnectorWorkWithCarTime = 0;
        ConnectorList[i].ConnectorWorkWithOtherTime = 0;
        ConnectorList[i].ConnectorCallingCarTime = 0;
    }
    update_car_pos();
    simulation_time_text->set_text(NS2String(simulation_time), rm.getFont("deyi.ttf", 48), c);
    simulation_time_text->set_rect_midleft(simulation_time_tip_text->rect.x + simulation_time_tip_text->rect.w + 8, simulation_time_tip_text->rect.y + simulation_time_tip_text->rect.h / 2);
    mark_update();
    task_over = false;
}

/**
 * @brief 随机执行任务
 */
void random_task(void)
{
    for (int i = 0; i < 18; i++)
    {
        ConnectorList[i].task_list.clear();
        ConnectorList[i].state = ConnectorState::ConnectorIdle;
        ConnectorList[i].time_temp = 0;
    }
    CarTask task;
    GenerateRandomTask(task);
    task.id = 1;
    ConnectorList[task.start_connector].task_list.push_back(task);
    int start_connector1 = task.start_connector;
    do
    {
        GenerateRandomTask(task);
        task.id = 2;
    } while (task.start_connector == start_connector1);
    ConnectorList[task.start_connector].task_list.push_back(task);
    int start_connector2 = task.start_connector;
    do
    {
        GenerateRandomTask(task);
        task.id = 3;
    } while (task.start_connector == start_connector1 || task.start_connector == start_connector2);
    ConnectorList[task.start_connector].task_list.push_back(task);
    for (int i = 0; i < 18; i++)
    {
        if (!ConnectorList[i].task_list.empty())
        {
            ConnectorList[i].state = ConnectorState::WorkWithOther;
            ConnectorList[i].time_temp = ConnectorList[i].get_load_time();
        }
    }
    task_over = false;
    for (int i = 0; i < CarNum; i++)
    {
        CarList[i].maxspeed_stright = RandomDouble(MAXSPEED_STRIGHT / 2, MAXSPEED_STRIGHT);
        CarList[i].maxspeed_curve = RandomDouble(MAXSPEED_CURVE / 2, MAXSPEED_CURVE);
    }
}

/**
 * @brief 从文件导入任务回调函数
 */
void SDLCALL file_task_callback(void*, const char * const *file_list, int)
{
    if (file_list == NULL || file_list[0] == NULL)    // 检查文件列表是否为空
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "未选择文件", NULL);
        return;
    }
    std::list<CarTask> tasks;
    load_task_csv(tasks, file_list[0]);
    for (const auto &task : tasks)
    {
        ConnectorList[task.start_connector].task_list.push_back(task);
    }
    for (int i = 0; i < 18; i++)
    {
        if (!ConnectorList[i].task_list.empty())
        {
            ConnectorList[i].state = ConnectorState::WorkWithOther;
            ConnectorList[i].time_temp = ConnectorList[i].get_load_time();
        }
    }
}
extern std::string work_path;
/**
 * @brief 从文件导入任务
 */
void file_task(void)
{
    for (int i = 0; i < 18; i++)
    {
        ConnectorList[i].task_list.clear();
        ConnectorList[i].state = ConnectorState::ConnectorIdle;
        ConnectorList[i].time_temp = 0;
    }
    SDL_DialogFileFilter filter = { "任务文件", "csv" };
    SDL_ShowOpenFileDialog(file_task_callback, NULL, NULL, &filter, 1, (work_path + "\\").c_str(), false);
    task_over = false;
}

/**
 * @brief 切换穿梭车数量
 */
void switch_car_num(void)
{
    SDL_Color c = rm.getColor(DARKBLUE);
    int num = CarNum;
    if (num == 3)
    {
        num = 5;
        CIC_List[3]->join(root);
        CIC_List[4]->join(root);
    }
    else if (num == 5)
    {
        num = 7;
        CIC_List[5]->join(root);
        CIC_List[6]->join(root);
    }
    else if (num == 7)
    {
        num = 3;
        CIC_List[3]->leave();
        CIC_List[4]->leave();
        CIC_List[5]->leave();
        CIC_List[6]->leave();
    }
    Set_CarNum(num);
    Reset_CarPos();
    update_car_pos();
    car_num_button_text->set_text(std::to_string(CarNum), rm.getFont("shuhei.ttf", 32), c);
}

/**
 * @brief 快速设置仿真速度为1x
 */
void set_simulation_speed_1x(void)
{
    set_simulation_speed(1);
    speed_dragbar->set_process((1 - 0.5f) / (32 - 0.5f));
}

/**
 * @brief 快速设置仿真速度为4x
 */
void set_simulation_speed_4x(void)
{
    set_simulation_speed(4);
    speed_dragbar->set_process((4 - 0.5f) / (32 - 0.5f));
}

/**
 * @brief 快速设置仿真速度为8x
 */
void set_simulation_speed_8x(void)
{
    set_simulation_speed(8);
    speed_dragbar->set_process((8 - 0.5f) / (32 - 0.5f));
}

/**
 * @brief 快速设置仿真速度为16x
 */
void set_simulation_speed_16x(void)
{
    set_simulation_speed(16);
    speed_dragbar->set_process((16 - 0.5f) / (32 - 0.5f));
}

/**
 * @brief 快速设置仿真速度为32x
 */
void set_simulation_speed_32x(void)
{
    set_simulation_speed(32.0);
    speed_dragbar->set_process((32.0 - 0.5) / (32 - 0.5));
}

/**
 * @brief 检查任务是否完成
 */
void check_task_over_func(Uint64)
{
    if (task_over)
    {
        task_over = false;
        // std::cout << "所有任务已完成！" << std::endl;
        for (auto &message : message_list)
        {
            std::cout << message << std::endl;
        }
        message_list.clear();
    }
}

double last_speeds[7] = { 0, 0, 0, 0, 0, 0, 0 };
CarRunState last_run_states[7] = {
    CarRunState::CarUniform,
    CarRunState::CarUniform,
    CarRunState::CarUniform,
    CarRunState::CarUniform,
    CarRunState::CarUniform,
    CarRunState::CarUniform,
    CarRunState::CarUniform
};

#define TOLERANCE 1e-9

void check_speed_func(Uint64)
{
    for (int i = 0; i < CarNum; i++)
    {
        CarRunState state = last_run_states[i];
        double last_speed = last_speeds[i];
        std::string last_time = NS2String(GetNS());

        // 计算速度变化量
        double speed_change = CarList[i].speed - last_speed;
        
        // 根据速度变化和容差确定当前动作类型
        CarRunState current_action;
        if (speed_change > TOLERANCE)
        {
            current_action = CarRunState::CarAccelerate;
        }
        else if (speed_change < -TOLERANCE)
        {
            current_action = CarRunState::CarDecelerate;
        }
        else
        {
            current_action = CarRunState::CarUniform;
        }
        
        /* 状态转换处理 */
        // 检测到新的加速开始（当前加速且之前不是加速）
        if (current_action == CarRunState::CarAccelerate && state != CarRunState::CarAccelerate)
        {
            // std::cout << "CSC00" << i + 1 << " " << last_time << " " << "开始加速，速度：" << speed_mmns_to_mmin(last_speed) << std::endl;
            message_list.push_back("CSC00" + std::to_string(i + 1) + " " + last_time + " 开始加速，速度：" + speed_mmns_to_mmin(last_speed));
        } 
        // 检测到新的减速开始（当前减速且之前不是减速）
        else if (current_action == CarRunState::CarDecelerate && state != CarRunState::CarDecelerate)
        {
            // std::cout << "CSC00" << i + 1 << " " << last_time << " " << "开始减速，速度：" << speed_mmns_to_mmin(last_speed) << std::endl;
            message_list.push_back("CSC00" + std::to_string(i + 1) + " " + last_time + " 开始减速，速度：" + speed_mmns_to_mmin(last_speed));
        }
        // 检测到速度保持（当前速度保持且之前不是速度保持）
        else if (current_action == CarRunState::CarUniform && state != CarRunState::CarUniform)
        {
            // std::cout << "CSC00" << i + 1 << " " << last_time << " " << "匀速行驶，速度：" << speed_mmns_to_mmin(CarList[i].speed) << std::endl;
            message_list.push_back("CSC00" + std::to_string(i + 1) + " " + last_time + " 匀速行驶，速度：" + speed_mmns_to_mmin(CarList[i].speed));
        }
        
        // 更新状态和记录值
        last_speeds[i] = CarList[i].speed;
        last_run_states[i] = current_action;  // 注意：这里更新为当前动作状态
    }
}
