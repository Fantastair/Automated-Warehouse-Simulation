#include <cmath>

#include "../../SDL3.h"
#include "../../Core.h"

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
Connector ConnectorList[18];

double ConnectorPos[18];        // 接口位置
ShuttleCar CarList[7];          // 穿梭车列表

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
        CarList[i].front = CarList + (i+1) % num;
        CarList[i].back = CarList + (i-1+num) % num;
    }
}

/**
 * @brief 重置车辆位置
 */
void Reset_CarPos(void)
{
    CarList[0].position = ConnectorPos[14];
    for (int i = 1; i < CarNum; i++)
    {
        CarList[i].position = CarList[i-1].position + CARLENGTH + MINDISTANCE;
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
