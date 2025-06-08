#ifndef __CAR_INFO_H__
#define __CAR_INFO_H__

#include "../../SDL3.h"
#include "../../Core.h"
#include "../../Display.h"

void CarInfoInit(void);

class CarInfoBarWidget;    // 前向声明组件类
class CarInfoCard;         // 前向声明卡片类

extern CarInfoCard *show_card;

class CarInfoBar : public RectUi
{
public:
    static float WIDTH;
    static float HEIGHT;

    int num;    // 车辆编号

    CarInfoBarWidget *mousewidget;    // 鼠标组件
    CarInfoCard *card;                // 车辆卡片指针

    TextUi *id_text;            // 车辆编号文本
    TextUi *speed_text;         // 车辆速度文本
    TextUi *speed_unit_text;    // 车辆速度单位文本
    TextUi *task_text;          // 车辆任务文本
    RectUi *goods_legend;       // 货物图例矩形

    CarInfoBar(int num_);
    ~CarInfoBar(void);

    virtual void render(float left, float top);
};

class CarInfoBarWidget : public MouseBaseWidget
{
public:
    CarInfoBar *carinfo;    // 车辆信息卡片指针

    CarInfoBarWidget(CarInfoBar *carinfo_);
    CarInfoBarWidget(CarInfoBar &carinfo_);
    virtual ~CarInfoBarWidget(void);

    virtual void mouseclick(void);
    virtual void mousein(void);
    virtual void mouseout(void);
};

class CarInfoCard : public RectUi
{
public:
    static float WIDTH;    // 卡片宽度
    static float HEIGHT;   // 卡片高度

    int num;    // 车辆编号

    TextUi *id_text;            // 车辆编号文本
    TextUi *state_text;         // 车辆状态文本
    TextUi *work_time_text;     // 车辆工作时间文本
    TextUi *idle_time_text;     // 车辆空闲时间文本
    TextUi *stop_count_text;    // 车辆停车次数文本
    TextUi *in_task_count_text;    // 车辆进行入库任务次数文本
    TextUi *out_task_count_text;   // 车辆进行出库任务次数文本

    CarInfoCard(int num_);
    ~CarInfoCard(void);

    virtual void render(float left, float top);
};

class ConnectorInfoCard : public RectUi
{
public:
    static float WIDTH;    // 卡片宽度
    static float HEIGHT;   // 卡片高度

    int num;    // 连接器编号

    TextUi *id_text;            // 连接器编号文本
    TextUi *type_text;          // 连接器类型文本
    TextUi *state_text;         // 连接器状态文本
    TextUi *load_time_text;     // 连接器装载时间文本
    TextUi *idle_time_text;     // 连接器空闲时间文本
    TextUi *task_count_text;    // 连接器剩余任务计数文本

    ConnectorInfoCard(int num_);
    ~ConnectorInfoCard(void);

    virtual void render(float left, float top);
    void follow_mouse(void);
};

class ConnectorUi;    // 前向声明连接器UI类

class ConnectorUiMouseWidget : public MouseBaseWidget
{
public:
    ConnectorUi *connectorui;    // 连接器UI指针
    float press_x;               // 鼠标按下位置
    float press_left;            // 鼠标按下左边距

    ConnectorUiMouseWidget(ConnectorUi *connectorui_);
    ConnectorUiMouseWidget(ConnectorUi &connectorui_);
    virtual ~ConnectorUiMouseWidget(void);

    virtual void mousein(void);
    virtual void mouseout(void);
    virtual void mousepress(float x, float, Uint8 button);
    virtual void mouserelease(float, float, Uint8 button);
    virtual void mousemove(float x, float);
};

#endif
