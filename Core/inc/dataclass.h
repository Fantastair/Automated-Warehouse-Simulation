#ifndef __DATACLASS_H__
#define __DATACLASS_H__

#include <list>
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
#define MINDISTANCE_CURVE   400                   // 弯道上前后车最小间距，考虑车身长度，mm
#define LOADTIME            ((Uint64)7.5e9)       // 穿梭车与接口设备交接货物时间，纳秒
// 接口参数
#define SC_LOAD_TIME    ((Uint64)50e9)    // 出库接口设备出货时间，ns
#define CS_LOAD_TIME    ((Uint64)25e9)    // 入库接口设备收货时间，ns
#define PC_LOAD_TIME    ((Uint64)30e9)    // 入库口装货时间，ns
#define CP_LOAD_TIME    ((Uint64)30e9)    // 出库口卸货时间，ns

extern bool task_over;

enum class CarState    // 车辆状态
{
    CarIdle = 0,     // 空闲
    CarToGet,        // 前往取货点
    CarGetting,      // 正在获取货物
    CarToPut,        // 前往收货点
    CarWaitToPut,    // 等待放置货物
    CarPutting,      // 正在放置货物
    CarInit,         // 回到初始位置
    CarWaiting,      // 等待前车运动
};

enum class CarRunState    // 车辆运行状态
{
    CarUniform = 0,    // 匀速运行
    CarAccelerate,     // 加速运行
    CarDecelerate,     // 减速运行
};

enum class ConnectorState     // 接口设备状态
{
    ConnectorIdle = 0,    // 空闲
    WaitForCar,           // 有货，等待穿梭车到达
    WorkWithCar,          // 与穿梭车交接货物
    WorkWithOther,        // 与其他对象（库存/操作人员）交接货物
    CallingCar,           // 呼叫穿梭车
};

enum class ConnectorType    // 接口设备类型
{
    CarToStoration = 0,    // 入库接口设备
    StorationToCar,        // *出库接口设备
    CarToPerson,           // 出库口
    PersonToCar,           // *入库口
};

typedef struct 
{
    int start_connector;    // 起始接口索引，-1 表示无任务
    int end_connector;      // 结束接口索引，-1 表示无任务
    int task_type;          // 任务类型，0 表示入库，1 表示出库，-1 表示无任务
    int id;            // 任务 ID，-1 表示无任务
} CarTask;    // 车辆任务



extern double TrackSplit[5];
extern double ConnectorPos[18];

void DataClass_Init(void);
void Set_CarNum(int num);
int GetTrackIndex(double pos);
void Reset_CarPos(void);
int GetTrackIndex(double pos);
bool isAllConnectorIdle(void);
bool isAllCarIdle(void);
void call_car(void);

/**
 * @brief 穿梭车
 */
class ShuttleCar
{
public:
    int id;                     // 车辆编号
    CarState state;             // 车辆状态
    CarState state_temp;        // 车辆临时状态，用于状态切换
    double speed;               // 车辆速度，mm/ns
    double position;            // 车辆位置，mm
    double initial_pos;         // 车辆初始位置，mm
    CarTask task;               // 车辆任务
    ShuttleCar *front;          // 前一辆车的指针
    ShuttleCar *back;           // 后一辆车的指针
    Uint64 time_temp;           // 临时时间戳，用于统计时间
    double maxspeed_stright;    // 最大直轨速度，mm/ns
    double maxspeed_curve;      // 最大弯轨速度，mm/ns

    Uint64 CarIdleTime;         // 空闲时间，纳秒
    Uint64 CarToGetTime;        // 前往取货点时间，纳秒
    Uint64 CarGettingTime;      // 获取货物时间，纳秒
    Uint64 CarToPutTime;        // 前往放置点时间，纳秒
    Uint64 CarWaitToPutTime;    // 等待放置货物时间，纳秒
    Uint64 CarPuttingTime;      // 放置货物时间，纳秒
    Uint64 CarInitTime;         // 回到初始位置时间，纳秒
    Uint64 CarWaitingTime;      // 等待前车运动时间，纳秒
    int CarStopCount;           // 停车次数
    CarRunState run_state;      // 车辆运行状态
    int in_task_count;          // 入库任务计数
    int out_task_count;         // 出库任务计数

    ShuttleCar(int id_);

    void init(void);
    void update(Uint64 dt);
    double get_free_distance(void);
    void move_to_pos_dt(double pos, Uint64 dt);

    Uint64 get_CarIdleTime(void);
    Uint64 get_CarWorkTime(void);
};
extern ShuttleCar CarList[7];
extern int CarNum;
ShuttleCar* GetFreeCar(void);
ShuttleCar* GetFreeCar(int id);

/**
 * @brief 接口设备
 */
class Connector
{
public:
    ConnectorState state;            // 接口状态
    ConnectorType type;              // 接口类型
    int id;                          // 接口编号
    Uint64 time_temp;                // 工作时间，从交接开始记录时长
    double position;                 // 接口位置，mm
    std::list<CarTask> task_list;    // 任务列表，存储等待的任务

    Uint64 ConnectorIdleTime;             // 接口空闲时间，纳秒
    Uint64 ConnectorWaitForCarTime;       // 接口等待穿梭车时间，纳秒
    Uint64 ConnectorWorkWithCarTime;      // 接口与穿梭车交接货物时间，纳秒
    Uint64 ConnectorWorkWithOtherTime;    // 接口与其他对象交接货物时间，纳秒
    Uint64 ConnectorCallingCarTime;       // 接口呼叫穿梭车时间，纳秒

    Connector(int num);
    void init();
    void update(Uint64 dt);
    Uint64 get_load_time(void);
    void start_work_with_car(void);

    Uint64 get_work_time(void);
    Uint64 get_idle_time(void);
};
extern Connector ConnectorList[18];

#endif
