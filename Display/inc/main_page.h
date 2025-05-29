#ifndef __MAIN_PAGE_H__
#define __MAIN_PAGE_H__

#define ZOOM_FACTOR 0.04f    // 缩放因子

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

void main_page_Init(void);

void update_car_pos(void);

void update_car_pos_func(Uint64);
void update_time_func(Uint64);

#endif
