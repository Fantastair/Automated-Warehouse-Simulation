#ifndef __DATACLASS_H__
#define __DATACLASS_H__

#include "../../SDL3.h"

// 轨道尺寸
#define TRACKWIDTH             1200     // 轨道宽度，mm
#define STRIGHT_TRACKLENGTH    40000    // 直轨长度，mm
#define CURVE_TRACKRADIUS      2500     // 弯轨内径，mm
// 接口尺寸
#define CONNECTOR_WIDTH        1100     // 接口宽度，mm
#define CS_1_LEFTPAD           3250     // 1 号入库接口设备左端距离上直轨左端点长度，mm
#define CS2SC_DISTANCE         1300     // 入库接口设备右端距离出库接口设备左端距离，mm
#define SC2CS_DISTANCE         2500     // 出库接口设备右端距离入库接口设备左端距离，mm
#define PC_18_LEFTPAD          7450     // 18 号入库口左端距离下直轨左端点长度，mm
#define PC2PC_DISTANCE         1900     // 入库口右端距离入库口左端距离，mm
#define PC2CP_DISTANCE         10900    // 入库口右端距离出库口左端距离，mm
// 穿梭车参数
#define MAXSPEED_STRIGHT    ((double)(8e-6/3))    // 最大直轨速度，mm/ns
#define MAXSPEED_CURVE      ((double)(2e-6/3))    // 最大弯轨速度，mm/ns
#define ACCELERATION        5e-16                 // 最大加（减）速度，mm/ns^2
#define CARLENGTH           2000                  // 车辆长度，mm
#define MINDISTANCE         200                   // 前后车最小间距，考虑车身长度，mm
#define MINDISTANCE_CURVE   600                   // 弯道上前后车最小间距，考虑车身长度，mm
#define LOADTIME            7.5e9                 // 穿梭车与接口设备交接货物时间，纳秒
// 接口参数
#define SC_LOAD_TIME    50e9    // 出库接口设备出货时间，ns
#define CS_LOAD_TIME    25e9    // 入库接口设备收货时间，ns
#define PC_LOAD_TIME    30e9    // 入库口装货时间，ns
#define CP_LOAD_TIME    30e9    // 出库口卸货时间，ns


enum class CarState    // 车辆状态
{
    CarIdle = 0,    // 空闲
    CarGetting,     // 正在获取货物
    CarPutting,     // 正在放置货物
    CarToGet,       // 前往取货点
    CarToPut,       // 前往收货点
};

enum class ConnectorState     // 接口设备状态
{
    ConnectorIdle = 0,    // 空闲
    WaitForCar,           // 有货，等待穿梭车到达
    WorkWithCar,          // 与穿梭车交接货物
    WorkWithOther,        // 与其他对象（库存/操作人员）交接货物
};

typedef struct 
{
    int start_connector;    // 起始接口索引，-1 表示无任务
    int end_connector;      // 结束接口索引，-1 表示无任务
    int task_type;          // 任务类型，0 表示入库，1 表示出库，-1 表示无任务
} CarTask;    // 车辆任务



extern double TrackSplit[5];
extern double ConnectorPos[18];

void DataClass_Init(void);
void Set_CarNum(int num);
int GetTrackIndex(double pos);
void Reset_CarPos(void);
int GetTrackIndex(double pos);

/**
 * @brief 穿梭车
 */
class ShuttleCar
{
public:
    int id;                    // 车辆编号
    CarState state;            // 车辆状态
    double speed;              // 车辆速度，mm/ns
    double position;           // 车辆位置，mm
    double initial_pos;        // 车辆初始位置，mm
    CarTask task;              // 车辆任务
    ShuttleCar *front;         // 前一辆车的指针
    ShuttleCar *back;          // 后一辆车的指针
    Uint64 time_temp;          // 临时时间戳，用于统计时间

    ShuttleCar(int id_);

    void init(void);
    void update(Uint64 dt);
    double get_free_distance(void);
    void move_to_pos_dt(double pos, Uint64 dt);
};
extern ShuttleCar CarList[7];
extern int CarNum;
ShuttleCar* GetFreeCar(void);

/**
 * @brief 接口设备
 */
class Connector
{
public:
    ConnectorState state;    // 接口状态
    Uint64 worktime;         // 工作时间，从交接开始记录时长

    Connector() : state(ConnectorState::ConnectorIdle), worktime(0) {}
};
extern Connector ConnectorList[18];

#endif
