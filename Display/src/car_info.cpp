#include "../../SDL3.h"
#include "../../Core.h"
#include "../../Display.h"
#include <string>
#include <iostream>

#define INCOLOR GREEN
#define OUTCOLOR ORANGE

CarInfoCard * show_card = nullptr;
TextUi *car_pointer = nullptr;

float CarInfoBar::WIDTH = 700;    // 卡片宽度
float CarInfoBar::HEIGHT = 48;    // 卡片高度

/**
 * @brief 初始化模块
 */
void CarInfoInit(void)
{
    car_pointer = new TextUi(ICON_DOWN_ARROW, rm.getFont("iconfont.ttf", 32), rm.getColor(THEMEBLUE));
    car_pointer->set_rect_center(CarUi::WIDTH / 2, -CarUi::HEIGHT / 2);

    CIC_List[0]->mousewidget->mouseclick();
}


CarInfoBar::CarInfoBar(int num_) : RectUi(WIDTH, HEIGHT, 4, rm.getColor_(FAKEWHITE), rm.getColor_(DARKBLUE), 16), num(num_)
{
    mousewidget = new CarInfoBarWidget(this);
    mousewidget->activate();

    id_text = new TextUi("C " + std::to_string(num + 1), rm.getFont("deyi.ttf", 31), rm.getColor(DARKBLUE));
    id_text->set_rect_midleft(16, HEIGHT / 2);
    id_text->join(this);

    speed_text = new TextUi("0", rm.getFont("deyi.ttf", 31), rm.getColor(DARKBLUE));
    speed_text->set_rect_midleft(96, HEIGHT / 2);
    speed_text->join(this);

    speed_unit_text = new TextUi("m/min", rm.getFont("deyi.ttf", 31), rm.getColor(DARKBLUE));
    speed_unit_text->set_rect_midleft(168, HEIGHT / 2);
    speed_unit_text->join(this);

    task_text = new TextUi("无任务", rm.getFont("deyi.ttf", 31), rm.getColor(DARKBLUE));
    task_text->set_rect_midleft(300, HEIGHT / 2);
    task_text->join(this);

    goods_legend = new RectUi(32, 32, 0, nullptr, rm.getColor_(DARKBLUE), 8);
    goods_legend->set_rect_center(WIDTH - HEIGHT / 2, HEIGHT / 2);
    goods_legend->join(this);

    card = new CarInfoCard(num_);
}

CarInfoBar::~CarInfoBar(void)
{
    delete mousewidget;
}

void CarInfoBar::render(float left, float top)
{
    SDL_Color c = rm.getColor(DARKBLUE);
    std::string speed_str = std::to_string(CarList[num].speed * 1e6 * 60);
    // 保留一位小数
    if (speed_str.find('.') == std::string::npos)
    {
        speed_str += ".0"; // 如果没有小数点，添加一位小数
    }
    else
    {
        size_t dot_pos = speed_str.find('.');
        if (dot_pos + 2 == speed_str.size())
        {
            speed_str += "0"; // 如果小数点后只有一位，添加一位
        }
        else if (dot_pos + 3 < speed_str.size())
        {
            speed_str = speed_str.substr(0, dot_pos + 3); // 保留两位小数
        }
    }
    speed_text->set_text(speed_str, rm.getFont("deyi.ttf", 31), c);
    RectUi::render(left, top);

    if (CarList[num].task.start_connector != -1)
    {
        std::string task_str;
        goods_legend->bd = 4;
        task_str = "任务" + std::to_string(CarList[num].task.id) + "：TP";
        if (CarList[num].task.id < 10) { task_str += "00"; }
        else if (CarList[num].task.id < 100) { task_str += "0"; }
        task_str += std::to_string(CarList[num].task.id) + " ";
        if (CarList[num].task.task_type == 0)
        {
            task_str += "入库 " + std::to_string(CarList[num].task.start_connector + 1) + " → " + std::to_string(CarList[num].task.end_connector + 1);
            goods_legend->bg = rm.getColor_(INCOLOR);
        }
        else if (CarList[num].task.task_type == 1)
        {
            task_str += "出库 " + std::to_string(CarList[num].task.end_connector + 1) + " ← " + std::to_string(CarList[num].task.start_connector + 1);
            goods_legend->bg = rm.getColor_(OUTCOLOR);
        }
        if (strcmp(task_text->text->text, task_str.c_str()) != 0)
        {
            task_text->set_text(task_str, rm.getFont("deyi.ttf", 31), c);
        }
    }
    else if (strcmp(task_text->text->text, "无任务") != 0)
    {
        task_text->set_text("无任务", rm.getFont("deyi.ttf", 31), c);
        goods_legend->bg = nullptr;
        goods_legend->bd = 0;
    }
}
    

CarInfoBarWidget::CarInfoBarWidget(CarInfoBar *carinfo_) : MouseBaseWidget(carinfo_), carinfo(carinfo_) {}
CarInfoBarWidget::CarInfoBarWidget(CarInfoBar &carinfo_) : MouseBaseWidget(carinfo_), carinfo(&carinfo_) {}

CarInfoBarWidget::~CarInfoBarWidget(void) {}

/**
 * @brief 鼠标单击事件处理
 */
void CarInfoBarWidget::mouseclick(void)
{
    carinfo->bg = rm.getColor_(LIGHTWHITE);
    for (int i = 0; i < CarNum; i++)
    {
        if (CIC_List[i]->mousewidget->carinfo->card == show_card)
        {
            CIC_List[i]->bg = rm.getColor_(FAKEWHITE);
        }
    }
    if (show_card != nullptr)
    {
        show_card->leave();
    }
    show_card = carinfo->card;
    show_card->join(root);

    if (car_pointer->father != nullptr)
    {
        car_pointer->leave();
    }
    car_pointer->join(carui_list[carinfo->num]);
}

/**
 * @brief 鼠标移入事件处理
 */
void CarInfoBarWidget::mousein(void)
{
    if (show_card != carinfo->card)
    {
        carinfo->bg = rm.getColor_(LIGHTWHITE);
        carinfo->card->join(root);
    }
}

/**
 * @brief 鼠标移出事件处理
 */
void CarInfoBarWidget::mouseout(void)
{
    if (show_card != carinfo->card)
    {
        carinfo->bg = rm.getColor_(FAKEWHITE);
        carinfo->card->leave();
    }
}

float CarInfoCard::WIDTH = 570;    // 卡片宽度
float CarInfoCard::HEIGHT = 400;    // 卡片高度

CarInfoCard::CarInfoCard(int num_) : RectUi(WIDTH, HEIGHT, 4, rm.getColor_(FAKEWHITE), rm.getColor_(DARKBLUE), 32), num(num_)
{
    this->set_rect_center(1610, 858);

    id_text = new TextUi("C " + std::to_string(num + 1), rm.getFont("deyi.ttf", 64), rm.getColor(DARKBLUE));
    id_text->set_rect_midleft(24, 64);
    id_text->join(this);

    TextUi *state_tip_text = new TextUi("状态：", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    state_tip_text->set_rect_midleft(160, id_text->rect.y + id_text->rect.h / 2);
    state_tip_text->join(this);
    state_text = new TextUi("空闲", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    state_text->set_rect_midleft(state_tip_text->rect.x + state_tip_text->rect.w + 24, state_tip_text->rect.y + state_tip_text->rect.h / 2);
    state_text->join(this);

    TextUi *work_time_tip_text = new TextUi("工作时间", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    work_time_tip_text->set_rect_midleft(24, 160);
    work_time_tip_text->join(this);

    work_time_text = new TextUi("00:00:00.0", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    work_time_text->set_rect_midleft(work_time_tip_text->rect.x + work_time_tip_text->rect.w + 24, work_time_tip_text->rect.y + work_time_tip_text->rect.h / 2);
    work_time_text->join(this);

    TextUi *idle_time_tip_text = new TextUi("空闲时间", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    idle_time_tip_text->set_rect_midleft(24, work_time_text->rect.y + work_time_text->rect.h + 32);
    idle_time_tip_text->join(this);

    idle_time_text = new TextUi("00:00:00.0", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    idle_time_text->set_rect_midleft(idle_time_tip_text->rect.x + idle_time_tip_text->rect.w + 24, idle_time_tip_text->rect.y + idle_time_tip_text->rect.h / 2);
    idle_time_text->join(this);

    TextUi *stop_count_tip_text = new TextUi("停车次数", rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    stop_count_tip_text->set_rect_midleft(24, idle_time_text->rect.y + idle_time_text->rect.h + 32);
    stop_count_tip_text->join(this);

    stop_count_text = new TextUi(std::to_string(CarList[num].CarStopCount), rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    stop_count_text->set_rect_midleft(stop_count_tip_text->rect.x + stop_count_tip_text->rect.w + 24, stop_count_tip_text->rect.y + stop_count_tip_text->rect.h / 2);
    stop_count_text->join(this);

    TextUi *in_task_count_tip_text = new TextUi("入库任务次数", rm.getFont("deyi.ttf", 41), rm.getColor(DARKBLUE));
    in_task_count_tip_text->set_rect_midleft(24, stop_count_text->rect.y + stop_count_text->rect.h + 32);
    in_task_count_tip_text->join(this);
    
    in_task_count_text = new TextUi(std::to_string(CarList[num].in_task_count), rm.getFont("deyi.ttf", 41), rm.getColor(DARKBLUE));
    in_task_count_text->set_rect_midleft(in_task_count_tip_text->rect.x + in_task_count_tip_text->rect.w + 24, in_task_count_tip_text->rect.y + in_task_count_tip_text->rect.h / 2);
    in_task_count_text->join(this);

    TextUi *out_task_count_tip_text = new TextUi("出库任务次数", rm.getFont("deyi.ttf", 41), rm.getColor(DARKBLUE));
    out_task_count_tip_text->set_rect_midleft(WIDTH / 2, in_task_count_tip_text->rect.y + in_task_count_tip_text->rect.h / 2);
    out_task_count_tip_text->join(this);

    out_task_count_text = new TextUi(std::to_string(CarList[num].out_task_count), rm.getFont("deyi.ttf", 41), rm.getColor(DARKBLUE));
    out_task_count_text->set_rect_midleft(out_task_count_tip_text->rect.x + out_task_count_tip_text->rect.w + 24, out_task_count_tip_text->rect.y + out_task_count_tip_text->rect.h / 2);
    out_task_count_text->join(this);
}

CarInfoCard::~CarInfoCard(void) {}

void CarInfoCard::render(float left, float top)
{
    SDL_Color c = rm.getColor(DARKBLUE);
    Uint64 t = CarList[num].get_CarWorkTime();
    work_time_text->set_text(NS2String(t), rm.getFont("deyi.ttf", 48), c);
    t = CarList[num].get_CarIdleTime();
    idle_time_text->set_text(NS2String(t), rm.getFont("deyi.ttf", 48), c);
    if (strcmp(stop_count_text->text->text, std::to_string(CarList[num].CarStopCount).c_str()) != 0)
    {
        stop_count_text->set_text(std::to_string(CarList[num].CarStopCount), rm.getFont("deyi.ttf", 48), c);
    }
    if (strcmp(in_task_count_text->text->text, std::to_string(CarList[num].in_task_count).c_str()) != 0)
    {
        in_task_count_text->set_text(std::to_string(CarList[num].in_task_count), rm.getFont("deyi.ttf", 41), c);
    }
    if (strcmp(out_task_count_text->text->text, std::to_string(CarList[num].out_task_count).c_str()) != 0)
    {
        out_task_count_text->set_text(std::to_string(CarList[num].out_task_count), rm.getFont("deyi.ttf", 41), c);
    }
    std::string state_str;
    switch (CarList[num].state)
    {
    case CarState::CarIdle:
        state_str = "空闲";
        break;
    case CarState::CarToGet:
        state_str = "前往取货点";
        break;
    case CarState::CarGetting:
        state_str = "装货";
        break;
    case CarState::CarToPut:
        state_str = "前往送货点";
        break;
    case CarState::CarWaitToPut:
        state_str = "等待卸货";
        break;
    case CarState::CarPutting:
        state_str = "卸货";
        break;
    case CarState::CarInit:
        state_str = "归位";
        break;
    case CarState::CarWaiting:
        state_str = "等待前车";
        break;
    }
    if (strcmp(state_text->text->text, state_str.c_str()) != 0)
    {
        state_text->set_text(state_str, rm.getFont("deyi.ttf", 48), c);
    }
    RectUi::render(left, top);
}

float ConnectorInfoCard::WIDTH = 310;    // 卡片宽度
float ConnectorInfoCard::HEIGHT = 340;    // 卡片高度

ConnectorInfoCard::ConnectorInfoCard(int num_) : RectUi(WIDTH, HEIGHT, 4, rm.getColor_(LIGHTWHITE), rm.getColor_(DARKBLUE), 32), num(num_)
{
    id_text = new TextUi(std::to_string(num_ + 1), rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    id_text->set_rect_midleft(16, 48);
    id_text->join(this);

    std::string type_str;
    switch (ConnectorList[num_].type)
    {
    case ConnectorType::CarToStoration:
        type_str = "入库接口设备";
        break;
    case ConnectorType::StorationToCar:
        type_str = "出库接口设备";
        break;
    case ConnectorType::CarToPerson:
        type_str = "出库口";
        break;
    case ConnectorType::PersonToCar:
        type_str = "入库口";
        break;
    default:
        break;
    }
    type_text = new TextUi(type_str, rm.getFont("deyi.ttf", 48), rm.getColor(DARKBLUE));
    type_text->set_rect_midleft(64, id_text->rect.y + id_text->rect.h / 2);
    type_text->join(this);

    TextUi *state_tip_text = new TextUi("状态：", rm.getFont("deyi.ttf", 31), rm.getColor(DARKBLUE));
    state_tip_text->set_rect_midleft(16, type_text->rect.y + type_text->rect.h + 80);
    state_tip_text->join(this);
    state_text = new TextUi("空闲", rm.getFont("deyi.ttf", 31), rm.getColor(DARKBLUE));
    state_text->set_rect_midleft(state_tip_text->rect.x + state_tip_text->rect.w + 8, state_tip_text->rect.y + state_tip_text->rect.h / 2);
    state_text->join(this);

    TextUi *load_time_tip_text = new TextUi("工作时间：", rm.getFont("deyi.ttf", 31), rm.getColor(DARKBLUE));
    load_time_tip_text->set_rect_midleft(16, state_text->rect.y + state_text->rect.h + 48);
    load_time_tip_text->join(this);
    load_time_text = new TextUi("00:00:00.0", rm.getFont("deyi.ttf", 31), rm.getColor(DARKBLUE));
    load_time_text->set_rect_midleft(load_time_tip_text->rect.x + load_time_tip_text->rect.w + 8, load_time_tip_text->rect.y + load_time_tip_text->rect.h / 2);
    load_time_text->join(this);

    TextUi *idle_time_tip_text = new TextUi("空闲时间：", rm.getFont("deyi.ttf", 31), rm.getColor(DARKBLUE));
    idle_time_tip_text->set_rect_midleft(16, load_time_text->rect.y + load_time_text->rect.h + 48);
    idle_time_tip_text->join(this);
    idle_time_text = new TextUi("00:00:00.0", rm.getFont("deyi.ttf", 31), rm.getColor(DARKBLUE));
    idle_time_text->set_rect_midleft(idle_time_tip_text->rect.x + idle_time_tip_text->rect.w + 8, idle_time_tip_text->rect.y + idle_time_tip_text->rect.h / 2);
    idle_time_text->join(this);
}

ConnectorInfoCard::~ConnectorInfoCard(void) {}

void ConnectorInfoCard::render(float left, float top)
{
    SDL_Color c = rm.getColor(DARKBLUE);
    std::string state_str;
    switch (ConnectorList[num].state)
    {
    case ConnectorState::ConnectorIdle:
        state_str = "空闲";
        break;
    case ConnectorState::WaitForCar:
        state_str = "等待穿梭车";
        break;
    case ConnectorState::WorkWithCar:
        if (ConnectorList[num].type == ConnectorType::CarToStoration || ConnectorList[num].type == ConnectorType::CarToPerson)
        {
            state_str = "装货";
        }
        else
        {
            state_str = "卸货";
        }
        break;
    case ConnectorState::WorkWithOther:
        if (ConnectorList[num].type == ConnectorType::CarToStoration || ConnectorList[num].type == ConnectorType::CarToPerson)
        {
            state_str = "卸货";
        }
        else
        {
            state_str = "装货";
        }
        break;
    case ConnectorState::CallingCar:
        state_str = "呼叫穿梭车";
        break;
    }
    if (strcmp(state_text->text->text, state_str.c_str()) != 0)
    {
        state_text->set_text(state_str, rm.getFont("deyi.ttf", 31), c);
    }
    load_time_text->set_text(NS2String(ConnectorList[num].get_work_time()), rm.getFont("deyi.ttf", 31), c);
    idle_time_text->set_text(NS2String(ConnectorList[num].get_idle_time()), rm.getFont("deyi.ttf", 31), c);
    RectUi::render(left, top);
}

void ConnectorInfoCard::follow_mouse(void)
{
    float x, y;
    SDL_GetMouseState(&x, &y);
    if (x < WIDTH) { x = WIDTH; }
    if (y + HEIGHT > WINDOW_HEIGHT) { y = WINDOW_HEIGHT - HEIGHT; }
    this->set_rect_topright(x, y);
}

ConnectorUiMouseWidget::ConnectorUiMouseWidget(ConnectorUi *connectorui_) : MouseBaseWidget(connectorui_), connectorui(connectorui_) {}
ConnectorUiMouseWidget::ConnectorUiMouseWidget(ConnectorUi &connectorui_) : MouseBaseWidget(connectorui_), connectorui(&connectorui_) {}
ConnectorUiMouseWidget::~ConnectorUiMouseWidget(void) {}

/**
 * @brief 鼠标移入事件处理
 */
void ConnectorUiMouseWidget::mousein(void)
{
    connectorui->card->join(root);
    connectorui->card->follow_mouse();
}

/**
 * @brief 鼠标移出事件处理
 */
void ConnectorUiMouseWidget::mouseout(void)
{
    connectorui->card->leave();
}

/**
 * @brief 鼠标按下事件处理
 */
void ConnectorUiMouseWidget::mousepress(float, float, Uint8 button)
{
    if (button != SDL_BUTTON_LEFT) { return; }
    if (12 <= connectorui->index && !Simulating)
    {
        if (mousedown == button && mouseon)
        {
            connectorui->card->leave();
        }
    }
}

/**
 * @brief 鼠标释放事件处理
 */
void ConnectorUiMouseWidget::mouserelease(float, float, Uint8 button)
{
    if (button != SDL_BUTTON_LEFT) { return; }
    if (12 <= connectorui->index && !Simulating)
    {
        if (mousedown == button && mouseon)
        {
            connectorui->card->join(root);
            connectorui->card->follow_mouse();
        }
    }
}

/**
 * @brief 鼠标移动事件处理
 */
void ConnectorUiMouseWidget::mousemove(float, float)
{
    connectorui->card->follow_mouse();
}
