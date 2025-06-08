#include <list>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "../../SDL3.h"
#include "../../Core.h"
#include "../../Display.h"

bool task_over = false;

/**
 * @brief 轨道分割点
 * @note 轨道坐标系：将环形轨道坐标简化至一维，顺时针方向为正，单位为 mm
 *       轨道分为 4 段，上直轨 -> 右弯轨 -> 下直轨 -> 左弯轨
 *       TrackSplit[4] 也是轨道最大坐标，溢出需要取余处理
 * @example 条件 (TrackSplit[0] <= x && x < TrackSplit[1]) 表示 坐标x 位于轨道第一段，也就是上直轨
 */
double TrackSplit[5];

/**
 * @brief 接口设备列表
 * @note 接口设备一共有 4 种
 *       第 1，3，5，7，9，11 个为入库接口设备，接收穿梭车(C)货物，向库存(S)转移
 *       第 2，4，6，8，10，12 个为出库接口设备，接收库存(S)货物，向穿梭车(C)转移
 *       第 13，14，15 个为出库口，接收穿梭车(C)货物，向操作人员(P)转移
 *       第 16，17，18 个为入库口，接收操作人员(P)货物，向穿梭车(C)转移
 * @details 与穿梭机交接货物时间为穿梭机参数 LOADTIME
 *          与其他对象（库存/操作人员）交接货物时间为接口参数
 *              SC_LOAD_TIME 接收库存(S)货物时间
 *              CS_LOAD_TIME 向库存(S)转移货物时间
 *              PC_LOAD_TIME 接收操作人员(P)货物时间
 *              CP_LOAD_TIME 向操作人员(P)转移货物时间
 */
Connector ConnectorList[18] = {
    Connector(0), Connector(1), Connector(2), Connector(3), Connector(4),
    Connector(5), Connector(6), Connector(7), Connector(8), Connector(9),
    Connector(10), Connector(11), Connector(12), Connector(13), Connector(14),
    Connector(15), Connector(16), Connector(17)
};                           // 接口设备列表
double ConnectorPos[18];     // 接口位置

ShuttleCar CarList[7] = {    // 穿梭车列表
    ShuttleCar(0),
    ShuttleCar(1),
    ShuttleCar(2),
    ShuttleCar(3),
    ShuttleCar(4),
    ShuttleCar(5),
    ShuttleCar(6)
};

/**
 * @brief 初始化数据类
 */
void DataClass_Init(void)
{
    // 初始化轨道数据
    TrackSplit[0] = 0;                                         // 上直轨起点
    TrackSplit[1] = TrackSplit[0] + 40000;                     // 上直轨终点，右弯轨起点
    TrackSplit[2] = TrackSplit[1] + (2500+1200/2)*SDL_PI_D;    // 右弯轨终点，下直轨起点
    TrackSplit[3] = TrackSplit[2] + 40000;                     // 下直轨终点，左弯轨起点
    TrackSplit[4] = TrackSplit[3] + (2500+1200/2)*SDL_PI_D;    // 左弯轨终点

    // 初始化接口位置
    ConnectorPos[0] = CS_1_LEFTPAD + CONNECTOR_WIDTH / 2;
    ConnectorPos[1] = ConnectorPos[0] + CS2SC_DISTANCE + CONNECTOR_WIDTH;
    ConnectorPos[2] = ConnectorPos[1] + SC2CS_DISTANCE + CONNECTOR_WIDTH;
    ConnectorPos[3] = ConnectorPos[2] + CS2SC_DISTANCE + CONNECTOR_WIDTH;
    ConnectorPos[4] = ConnectorPos[3] + SC2CS_DISTANCE + CONNECTOR_WIDTH;
    ConnectorPos[5] = ConnectorPos[4] + CS2SC_DISTANCE + CONNECTOR_WIDTH;
    ConnectorPos[6] = ConnectorPos[5] + SC2CS_DISTANCE + CONNECTOR_WIDTH;
    ConnectorPos[7] = ConnectorPos[6] + CS2SC_DISTANCE + CONNECTOR_WIDTH;
    ConnectorPos[8] = ConnectorPos[7] + SC2CS_DISTANCE + CONNECTOR_WIDTH;
    ConnectorPos[9] = ConnectorPos[8] + CS2SC_DISTANCE + CONNECTOR_WIDTH;
    ConnectorPos[10] = ConnectorPos[9] + SC2CS_DISTANCE + CONNECTOR_WIDTH;
    ConnectorPos[11] = ConnectorPos[10] + CS2SC_DISTANCE + CONNECTOR_WIDTH;
    ConnectorPos[17] = TrackSplit[3] - PC_18_LEFTPAD - CONNECTOR_WIDTH / 2;
    ConnectorPos[16] = ConnectorPos[17] - PC2PC_DISTANCE - CONNECTOR_WIDTH;
    ConnectorPos[15] = ConnectorPos[16] - PC2PC_DISTANCE - CONNECTOR_WIDTH;
    ConnectorPos[14] = ConnectorPos[15] - PC2CP_DISTANCE - CONNECTOR_WIDTH;
    ConnectorPos[13] = ConnectorPos[14] - PC2PC_DISTANCE - CONNECTOR_WIDTH;
    ConnectorPos[12] = ConnectorPos[13] - PC2PC_DISTANCE - CONNECTOR_WIDTH;

    // 初始使用 3 辆车
    Set_CarNum(3);
    Reset_CarPos();
}

int CarNum;

/**
 * @brief 设置穿梭车数量
 * @param num 穿梭车数量
 */
void Set_CarNum(int num)
{
    CarNum = num;
    for (int i = 0; i < num; i++)
    {
        CarList[i].back = CarList + (i+1) % num;
        CarList[i].front = CarList + (i-1+num) % num;
    }
}

/**
 * @brief 重置车辆位置
 */
void Reset_CarPos(void)
{
    if (CarList[0].initial_pos == 0)
    {
        CarList[0].position = ConnectorPos[14];
    }
    else
    {
        CarList[0].position = CarList[0].initial_pos;
    }
    for (int i = 1; i < CarNum; i++)
    {
        CarList[i].position = CarList[i-1].position - CARLENGTH - MINDISTANCE;
    }
}

/**
 * @brief 获取指定位置所在轨道索引
 * @param pos 轨道位置坐标
 * @return 0 ~ 3，分别表示 4 段轨道
 */
int GetTrackIndex(double pos)
{
    if (pos >= TrackSplit[4])
    {
        pos = fmod(pos, TrackSplit[4]);
    }
    for (int i = 0; i < 4; i ++)
    {
        if (TrackSplit[i] <= pos && pos < TrackSplit[i + 1])
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 穿梭车构造函数
 */
ShuttleCar::ShuttleCar(int id_) : id(id_), state(CarState::CarIdle), state_temp(CarState::CarIdle), speed(0), position(0), initial_pos(0), task({-1, -1, -1, -1}), next_task({-1, -1, -1, -1}), front(nullptr), back(nullptr), time_temp(0), maxspeed_stright(MAXSPEED_STRIGHT), maxspeed_curve(MAXSPEED_CURVE),
    CarIdleTime(0), CarToGetTime(0), CarWaitToGetTime(0), CarGettingTime(0), CarToPutTime(0), CarWaitToPutTime(0), CarPuttingTime(0), CarInitTime(0), CarWaitingTime(0),
    CarStopCount(0), run_state(CarRunState::CarUniform), in_task_count(0), out_task_count(0), run_distance(0) {}


/**
 * @brief 初始化穿梭车
 */
void ShuttleCar::init(void)
{
    initial_pos = ConnectorPos[14] - id * (CARLENGTH + MINDISTANCE);
}



/**
 * @brief 更新车辆状态
 * @param dt 时间差，单位纳秒
 */
void ShuttleCar::update(Uint64 dt)
{
    switch (state)
    {
    case CarState::CarIdle: // 空闲状态
        CarIdleTime += dt;
        if (task.start_connector != -1)    // 如果有任务
        {
            state = CarState::CarToGet;    // 切换到 前往取货点 状态
            if (task.task_type == 0) { in_task_count++; }
            else if (task.task_type == 1) { out_task_count++; }
            // std::cout << "Car " << id << " switch to CarToGet state, task: " << task.start_connector << " -> " << task.end_connector << std::endl;
            return;
        }
        if (id == 0 && isAllCarIdle() && isAllConnectorIdle())
        {
            state = CarState::CarInit;
            task_over = true;
            // std::cout << "Car " << id << " switch to CarInit state." << std::endl;
        }
        else
        {        
            move_to_pos_dt(fmod(position - 1000 + TrackSplit[4], TrackSplit[4]), dt);    // 以最大速度巡航
        }
        break;
    case CarState::CarToGet: // 前往取货点
        CarToGetTime += dt;
        if (fmod(position - ConnectorPos[task.start_connector] + TrackSplit[4], TrackSplit[4]) < 0.01)    // 如果到达取货点
        {
            position = ConnectorPos[task.start_connector];    // 确保位置准确
            state = CarState::CarGetting;    // 切换到 正在获取货物 状态
            time_temp = 0;
            ConnectorList[task.start_connector].start_work_with_car();
            message_list.push_back(NS2String(simulation_time) + ", " + std::to_string(id + 1) + ", " + std::to_string(task.id) + ", 有货");
            // std::cout << "Car " << id << " switch to CarGetting state." << std::endl;
        }
        else
        {
            move_to_pos_dt(ConnectorPos[task.start_connector], dt);    // 向取货点移动
        }
        break;
    case CarState::CarGetting: // 正在获取货物
        CarGettingTime += dt;
        time_temp += dt;    // 累计时间
        if (time_temp >= LOADTIME)    // 完成取货
        {
            time_temp = static_cast<Uint64>(LOADTIME);
            task.get_time = simulation_time;
            state = CarState::CarToPut;    // 切换到 前往放置点 状态
            // std::cout << "Car " << id << " switch to CarToPut state." << std::endl;
        }
        break;
    case CarState::CarToPut: // 前往收货点
        CarToPutTime += dt;
        if (fmod(position - ConnectorPos[task.end_connector] + TrackSplit[4], TrackSplit[4]) < 0.01)    // 如果到达放置点
        {
            position = ConnectorPos[task.end_connector];    // 确保位置准确
            if (ConnectorList[task.end_connector].state == ConnectorState::ConnectorIdle)
            {
                state = CarState::CarPutting;    // 切换到 正在放置货物 状态
                time_temp = 0;
                ConnectorList[task.end_connector].start_work_with_car();
                // std::cout << "Car " << id << " switch to CarPutting state." << std::endl;
            }
            else
            {
                state = CarState::CarWaitToPut;    // 切换到 等待放置货物 状态
                // std::cout << "Car " << id << " switch to CarWaitToPut state." << std::endl;
            }
        }
        else
        {
            move_to_pos_dt(ConnectorPos[task.end_connector], dt);    // 向放置点移动
        }
        break;
    case CarState::CarPutting: // 正在放置货物
        CarPuttingTime += dt;
        time_temp += dt;
        if (time_temp >= LOADTIME)    // 完成放置
        {
            time_temp = static_cast<Uint64>(LOADTIME);
            task.put_time = simulation_time;
            task.car_id = id;
            if (task.task_type == 0) { in_task_count++; task.end_time = simulation_time + CS_LOAD_TIME; }
            else if (task.task_type == 1) { out_task_count++; task.end_time = simulation_time + CP_LOAD_TIME; }

            task_list_.push_back(task);
            message_list.push_back(NS2String(simulation_time) + ", " + std::to_string(id + 1) + ", " + std::to_string(task.id) + ", 无货");

            if (next_task.start_connector != -1)
            {
                task = next_task;
                next_task = {-1, -1, -1, -1};
                state = CarState::CarToGet;
            }
            else
            {
                state = CarState::CarIdle;
                task = {-1, -1, -1, -1};
            }
            // state = CarState::CarIdle;    // 切换到 空闲 状态
            // task = {-1, -1, -1, -1};    // 清空任务
            // std::cout << "Car " << id << " switch to CarIdle state." << std::endl;
        }
        break;
    case CarState::CarWaitToPut: // 等待放置货物
        CarWaitToPutTime += dt;
        if (ConnectorList[task.end_connector].state == ConnectorState::ConnectorIdle)
        {
                state = CarState::CarPutting;    // 切换到 正在放置货物 状态
                time_temp = 0;
                ConnectorList[task.end_connector].start_work_with_car();
                // std::cout << "Car " << id << " switch to CarPutting state." << std::endl;
        }
        break;
    case CarState::CarInit: // 回到初始位置
        CarInitTime += dt;
        if (fmod(position - initial_pos + TrackSplit[4], TrackSplit[4]) > 0.01)    // 如果没有到达初始位置
        {
            move_to_pos_dt(initial_pos, dt);    // 向初始位置移动
        }
        else if (id == 0)
        {
            for (int i = 1; i < CarNum; i++)
            {
                CarList[i].state = CarState::CarInit;
                // std::cout << "Car " << CarList[i].id << " switch to CarInit state." << std::endl;
            }
        }
        else if (id == CarNum - 1)
        {
            switch_simulation();
        }
        break;
    case CarState::CarWaiting: // 等待前车运动
        CarWaitingTime += dt;
        if (front->speed > 0)
        {
            state = state_temp;
        }
        break;
    case CarState::CarWaitToGet: // 等待获取货物
        CarIdleTime += dt; // 仍然算作空闲时间
        if (ConnectorList[task.start_connector].state == ConnectorState::WaitForCar)
        {
            state = CarState::CarGetting; // 切换到 正在获取货物 状态
            time_temp = 0;
            ConnectorList[task.start_connector].start_work_with_car();
            // std::cout << "Car " << id << " switch to CarGetting state." << std::endl;
        }
        break;
    default:
        break;
    }
}

/**
 * @brief 获取前方有空余距离的穿梭车
 * @return 返回指向空闲穿梭车的指针
 */
ShuttleCar* GetFreeCar(void)
{
    for (int i = 0; i < CarNum; i++)
    {
        if (CarList[i].get_free_distance() > 0)
        {
            return CarList + i;
        }
    }
    return nullptr;
}

/**
 * @brief 获取距离指定编号接口设备最近的空闲穿梭车
 * @param id 接口设备编号
 * @return 返回指向空闲穿梭车的指针
 * @note 如果没有空闲穿梭车，返回 nullptr
 */
ShuttleCar* GetFreeCar(int id)
{
    double min_distance = TrackSplit[4];
    ShuttleCar* free_car = nullptr;
    for (int i = 0; i < CarNum; i++)
    {
        if (CarList[i].state != CarState::CarIdle) { continue; }
        double distance = fmod(ConnectorPos[id] - CarList[i].position + TrackSplit[4], TrackSplit[4]);
        if (distance < min_break_distance(CarList[i].speed, 0)) { continue; }
        if (distance < min_distance)
        {
            min_distance = distance;
            free_car = CarList + i;
        }
    }
    return free_car;
}

/**
 * @brief 获取车辆前方的空余距离
 * @return 返回车辆前方的空余距离，单位 mm
 */
double ShuttleCar::get_free_distance(void)
{
    int self = GetTrackIndex(position);
    int other = GetTrackIndex(front->position);

    if (self % 2 == 1 || other % 2 == 1)
    {
        return fmod(front->position - position + TrackSplit[4], TrackSplit[4]) - CARLENGTH - MINDISTANCE_CURVE;
    }
    else
    {
        return fmod(front->position - position + TrackSplit[4], TrackSplit[4]) - CARLENGTH - MINDISTANCE;
    }
}

/**
 * @brief 将车辆移动向指定位置移动
 * @param pos 目标位置，单位 mm
 * @param dt 时间差，单位纳秒
 * @note 该函数会正确处理加减速，车辆间距等情况
 */
void ShuttleCar::move_to_pos_dt(double pos, Uint64 dt)
{
    // 是否到达目标位置
    if (fmod(pos - position + TrackSplit[4], TrackSplit[4]) <= 0.01)
    {
        position = pos;
        speed = 0;
        return;
    }

    int self_track = GetTrackIndex(position);    // 当前车辆所在轨道索引
    // 是否接近目标位置
    bool close_target = fmod(pos - position + TrackSplit[4], TrackSplit[4]) <= min_break_distance(speed, 0);
    // 是否接近弯道
    bool close_curve = (self_track % 2 == 0 && fmod(TrackSplit[self_track + 1] - position + TrackSplit[4], TrackSplit[4]) <= min_break_distance(speed, maxspeed_curve));
    // 是否接近前方车辆
    bool close_front = (speed >= front->speed && get_free_distance() <= min_break_distance(speed, front->speed));
    // 是否超出最大速度
    bool over_speed = (self_track % 2 == 0 && speed - maxspeed_stright > ACCELERATION * dt) || (self_track % 2 == 1 && speed - maxspeed_curve > ACCELERATION * dt);
    // 是否低于最大速度
    bool low_speed = (self_track % 2 == 0 && maxspeed_stright - speed > ACCELERATION * dt) || (self_track % 2 == 1 && maxspeed_curve - speed > ACCELERATION * dt);

    if (close_target || close_curve || close_front || over_speed)    // 需要减速
    {
        if (speed > 0)
        {
            double d = min_break_distance(speed, speed - ACCELERATION * dt);
            position += d;
            // run_distance += d;
            speed -= ACCELERATION * dt;
            if (speed < 0)
            {
                speed = 0;
                CarStopCount++;
                if (state == CarState::CarToGet || state == CarState::CarToPut)
                {
                    state_temp = state;
                    state = CarState::CarWaiting;    // 切换到 等待前车运动 状态
                    // std::cout << "Car " << id << " switch to CarWaiting state." << std::endl;
                }
            }
        }
    }
    else if (low_speed)    // 可以加速
    {
        double d = min_break_distance(speed + ACCELERATION * dt, speed);
        position += d;
        // run_distance += d;
        speed += ACCELERATION * dt;
        if (self_track % 2 == 0 && speed > maxspeed_stright) speed = maxspeed_stright;
        else if (self_track % 2 == 1 && speed > maxspeed_curve) speed = maxspeed_curve;
    }
    else    // 匀速
    {
        double d = speed * dt;
        position += d;
        // run_distance += d;
    }
    if (position >= TrackSplit[4])
    {
        position = fmod(position, TrackSplit[4]);
    }
}

/**
 * @brief 获取车辆空闲时间
 * @return 返回车辆空闲时间，单位纳秒
 * @note 空闲时间指的是，没有任务时的时间和等待时间
 */
Uint64 ShuttleCar::get_CarIdleTime(void)
{
    return CarIdleTime + CarWaitingTime + CarWaitToPutTime + CarInitTime + CarWaitToGetTime;
}

/**
 * @brief 获取车辆工作时间
 * @return 返回车辆工作时间，单位纳秒
 * @note 工作时间指的是，车辆正在执行任务的时间
 */
Uint64 ShuttleCar::get_CarWorkTime(void)
{
    return CarToGetTime + CarGettingTime + CarToPutTime + CarPuttingTime;
}

Connector::Connector(int num) : state(ConnectorState::ConnectorIdle), id(num), time_temp(0), position(0)
{
    task_list.clear();
    if (num < 12 && num % 2 == 0)    // 入库接口
    {
        type = ConnectorType::CarToStoration;
    }
    else if (num < 12 && num % 2 == 1)    // 出库接口
    {
        type = ConnectorType::StorationToCar;
    }
    else if (num < 15)    // 出库口
    {
        type = ConnectorType::CarToPerson;
    }
    else    // 入库口
    {
        type = ConnectorType::PersonToCar;
    }
    ConnectorIdleTime = 0;
    ConnectorWaitForCarTime = 0;
    ConnectorWorkWithCarTime = 0;
    ConnectorWorkWithOtherTime = 0;
    ConnectorCallingCarTime = 0;
}


/**
 * @brief 初始化接口设备
 */
void Connector::init()
{
    position = ConnectorPos[id];
}

/**
 * @brief 更新接口设备状态
 * @param dt 时间差，单位纳秒
 */
void Connector::update(Uint64 dt)
{
    switch (state)
    {
    case ConnectorState::ConnectorIdle:
        ConnectorIdleTime += dt;
        if ((type == ConnectorType::StorationToCar || type == ConnectorType::PersonToCar) && !task_list.empty())    // 如果有任务
        {
            state = ConnectorState::WorkWithOther;
            time_temp = 0;
            task_list.front().start_time = simulation_time;
            // std::cout << "Connector " << id << " switch to WorkWithOther state." << std::endl;
            message_list.push_back(NS2String(simulation_time) + ", " + std::to_string(id + 1) + ", " + std::to_string(task_list.front().id) + ", 有货");
        }
        break;
    case ConnectorState::WaitForCar:
        ConnectorWaitForCarTime += dt;
        break;
    case ConnectorState::WorkWithCar:
        ConnectorWorkWithCarTime += dt;
        time_temp += dt;
        if (time_temp >= LOADTIME)
        {
            time_temp = LOADTIME;
            if (type == ConnectorType::CarToStoration || type == ConnectorType::CarToPerson)
            {
                state = ConnectorState::WorkWithOther;
                time_temp = 0;
                // std::cout << "Connector " << id << " switch to WorkWithOther state." << std::endl;
            }
            else
            {
                message_list.push_back(NS2String(simulation_time) + ", " + std::to_string(id + 1) + ", " + std::to_string(task_list.front().id) + ", 无货");
                task_list.pop_front();
                state = ConnectorState::ConnectorIdle;
                // std::cout << "Connector " << id << " switch to ConnectorIdle state." << std::endl;
            }
        }
        break;
    case ConnectorState::WorkWithOther:
        ConnectorWorkWithOtherTime += dt;
        time_temp += dt;
        if (time_temp >= get_load_time())
        {
            time_temp = get_load_time();
            if (type == ConnectorType::StorationToCar || type == ConnectorType::PersonToCar)
            {
                state = ConnectorState::CallingCar;
                // std::cout << "Connector " << id << " switch to CallingCar state." << std::endl;
            }
            else
            {
                state = ConnectorState::ConnectorIdle;
                // std::cout << "Connector " << id << " switch to ConnectorIdle state." << std::endl;
            }
        }
        break;
    case ConnectorState::CallingCar:
        ConnectorCallingCarTime += dt;
        break;
    default:
        break;
    }
}

/**
 * @brief 获取与其他对象交接货物的时间
 * @return 返回交接货物的时间，单位纳秒
 */
Uint64 Connector::get_load_time()
{
    switch (type)
    {
    case ConnectorType::CarToStoration:
        return CS_LOAD_TIME;
    case ConnectorType::StorationToCar:
        return SC_LOAD_TIME;
    case ConnectorType::CarToPerson:
        return CP_LOAD_TIME;
    case ConnectorType::PersonToCar:
        return PC_LOAD_TIME;
    default:
        return 0; // 默认返回 0
    }
}

/**
 * @brief 穿梭车开始与接口设备交接货物
 */
void Connector::start_work_with_car()
{
    state = ConnectorState::WorkWithCar;    // 切换到 与穿梭车交接货物 状态
    time_temp = 0;    // 重置时间
    // std::cout << "Connector " << id << " switch to WorkWithCar state." << std::endl;
}

/**
 * @brief 获取接口设备工作时间
 * @return 返回接口设备工作时间，单位纳秒
 */
Uint64 Connector::get_work_time(void)
{
    return ConnectorWorkWithCarTime + ConnectorWorkWithOtherTime;
}

/**
 * @brief 获取接口设备空闲时间
 * @return 返回接口设备空闲时间，单位纳秒
 */
Uint64 Connector::get_idle_time(void)
{
    return ConnectorIdleTime + ConnectorWaitForCarTime + ConnectorCallingCarTime;
}

/**
 * @brief 检查所有接口设备是否都处于空闲状态
 * @return 如果所有接口设备都处于空闲状态，返回 true；否则返回 false
 */
bool isAllConnectorIdle(void)
{
    for (int i = 0; i < 18; i++)
    {
        if (ConnectorList[i].state != ConnectorState::ConnectorIdle)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief 检查所有穿梭车是否都处于空闲状态
 * @return 如果所有穿梭车都处于空闲状态，返回 true；否则返回 false
 */
bool isAllCarIdle(void)
{
    for (int i = 0; i < CarNum; i++)
    {
        if (CarList[i].state != CarState::CarIdle)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief 调度穿梭车
 */
void call_car(void)
{
    for (int i = 17; i >= 0; i--)
    {
        if (ConnectorList[i].state == ConnectorState::CallingCar)
        {
            ShuttleCar* free_car;
            free_car = GetFreeCar(ConnectorList[i].id);
            if (free_car != nullptr)    // 如果有空闲穿梭车
            {
                free_car->task = ConnectorList[i].task_list.front();
                free_car->state = CarState::CarToGet;
                free_car->look_for_next_task();    // 尝试寻找下一个任务
                ConnectorList[i].state = ConnectorState::WaitForCar;
            }
        }
    }
}

/**
 * @brief 穿梭车寻找下一个任务
 */
void ShuttleCar::look_for_next_task(void)
{
    if (task.end_connector < 12)    // 如果是入库任务，尝试找一个出库任务
    {
        for (int i = task.end_connector + 1; i < 12; i += 2)
        {
            if (ConnectorList[i].state == ConnectorState::CallingCar)
            {
                next_task = ConnectorList[i].task_list.front();
                ConnectorList[i].state = ConnectorState::WaitForCar;
                return;
            }
        }
    }
    else if (task.end_connector >= 12)    // 如果是出库任务，尝试找一个入库任务
    {
        for (int i = 12; i < 18; i++)
        {
            if (ConnectorList[i].position > ConnectorList[task.end_connector].position && ConnectorList[i].state == ConnectorState::CallingCar)
            {
                next_task = ConnectorList[i].task_list.front();
                ConnectorList[i].state = ConnectorState::WaitForCar;
                return;
            }
        }
    }
    if (next_task.start_connector == -1)    // 如果没有找到下一个任务
    {
        next_task = {-1, -1, -1, -1};    // 清空下一个任务
    }
}
