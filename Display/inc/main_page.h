#ifndef __MAIN_PAGE_H__
#define __MAIN_PAGE_H__

#include "../../Display.h"

#define ZOOM_FACTOR 0.04f    // 缩放因子

extern float left_limit;
extern float right_limit;

class CarUi : public Ui
{
public:
    SDL_Texture *texture;    // 车辆纹理
    SDL_Surface *surface;    // 车辆图像表面
    int num;                 // 车辆编号
    double angle;            // 车辆角度，角度制
    RectUi *progress_bar;    // 进度条
    TextUi *id_text;         // 车辆编号文本
    static float WIDTH;      // 车辆宽度
    static float HEIGHT;     // 车辆高度

    CarUi(int n);
    ~CarUi(void);

    void init_id_text(void);
    void update_texture(void);
    void update_surface(void);
    void update_progress(void);
    void render(float left, float top) override;
};

extern CarUi *carui_list[7];
extern CarInfoBar *CIC_List[7];
extern std::list<std::string> message_list;
extern double last_speeds[7];
extern CarRunState last_run_states[7];

class ConnectorUi : public RectUi
{
public:
    int index;              // 接口索引
    static float WIDTH;     // 接口宽度
    static float HEIGHT;    // 接口高度
    RectUi *fill_rect;      // 填充矩形，用于显示接口状态

    ConnectorInfoCard *card;    // 接口卡片指针

    ConnectorUiMouseWidget *mousewidget;    // 鼠标组件

    ConnectorUi(int i);
    ~ConnectorUi(void);

    virtual void render(float left, float top);
};

class SpeedDragBar : public DragBar
{
public:
    SpeedDragBar(float width, float height, float bd_, DragBarStyle &style, Ui *drager);

    virtual void set_process(float p);
};

void main_page_Init(void);

void update_car_pos(void);

void update_car_pos_func(Uint64);
void update_time_func(Uint64);

void switch_simulation(void);
void reset_simulation(void);
void random_task(void);
void file_task(void);
void switch_car_num(void);

void set_simulation_speed_1x(void);
void set_simulation_speed_4x(void);
void set_simulation_speed_8x(void);
void set_simulation_speed_16x(void);
void set_simulation_speed_32x(void);

void check_task_over_func(Uint64);

void check_speed_func(Uint64);

#endif
