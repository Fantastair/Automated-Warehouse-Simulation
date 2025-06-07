#include <list>
#include <random>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "../../SDL3.h"
#include "../../Core.h"
#include "../../Display.h"


/**
 * @brief 获取当前时间戳
 * @note 时间戳表示从程序初始化到调用此函数时的时间
 * @return 当前时间戳，单位纳秒
 */
Uint64 GetNS(void)
{
    return SDL_GetTicksNS();
}


/**
 * @brief 弧长转角度
 * @param arc 弧长，单位 mm
 * @param radius 圆的半径，单位 mm
 * @return 转换后的角度，弧度制
 */
double arc2angle(double arc, double radius)
{
    return arc / radius;
}

/**
 * @brief 弧度转角度
 * @param angle 弧度
 * @return 转换后的角度，单位度
 */
double angle2degree(double angle)
{
    return angle * 180.0 / SDL_PI_D;
}

/**
 * @brief 计算最小制动距离
 * @param current_speed 当前速度，单位 mm/ns
 * @param target_speed 目标速度，单位 mm/ns
 * @return 最小制动距离，单位 mm
 */
double min_break_distance(double current_speed, double target_speed)
{
    return (current_speed * current_speed - target_speed * target_speed) / (2 * ACCELERATION);
}

/**
 * @brief 初始化随机数生成器
 * @note 该函数在程序启动时调用一次
 */
void RandomInit(void)
{
    std::random_device rd;
    std::mt19937 generator(rd());
    std::srand(static_cast<unsigned int>(generator()));
}

/**
 * @brief 生成随机整数
 * @param a 最小值
 * @param b 最大值
 * @return 返回 [a, b] 范围内的随机整数
 */
int RandomInt(double a, double b)
{
    return static_cast<int>(std::rand() % static_cast<int>(b - a + 1) + a);
}

/**
 * @brief 生成随机浮点数
 * @param a 最小值
 * @param b 最大值
 * @return 返回 [a, b] 范围内的随机浮点数
 */
double RandomDouble(double a, double b)
{
    return a + (b - a) * (static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX));
}

/**
 * @brief 生成随机任务
 * @param task 任务引用
 * @note 该函数会随机生成入库或出库任务
 */
void GenerateRandomTask(CarTask &task)
{
    // 生成随机任务
    task.task_type = RandomInt(0, 1);    // 随机任务类型，0 表示入库，1 表示出库
    if (task.task_type == 0)    // 入库任务
    {
        task.start_connector = RandomInt(15, 17);    // 随机入库接口
        task.end_connector = RandomInt(0, 5) * 2;    // 随机出库接口
    }
    else    // 出库任务
    {
        task.start_connector = RandomInt(0, 5) * 2 + 1;    // 随机入库接口
        task.end_connector = RandomInt(12, 14);    // 随机出库接口
    }
}

/**
 * @brief 将纳秒时间转换为字符串
 * @param ns 纳秒时间
 * @return 返回格式化后的时间字符串 xx:xx:xx.xx
 */
std::string NS2String(Uint64 ns)
{
    Uint64 seconds = ns / 1'000'000'000;
    Uint64 milliseconds = (ns % 1'000'000'000) / 1'000'000;

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%02llu:%02llu:%02llu.%01llu", seconds / 3600, (seconds % 3600) / 60, seconds % 60, milliseconds / 100);
    return std::string(buffer);
}

/**
 * @brief 从 CSV 文件加载任务列表
 * @param task_list 任务列表引用
 * @param file_path CSV 文件路径
 */
void load_task_csv(std::list<CarTask> &task_list, const std::string &file_path)
{
    // 清空任务列表
    task_list.clear();
    
    // 打开文件
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return; // 文件打开失败直接返回
    }

    std::string line;
    // 跳过UTF-8 BOM（如果存在）
    char bom[3] = {0};
    file.read(bom, 3);
    if (bom[0] != '\xEF' || bom[1] != '\xBB' || bom[2] != '\xBF') {
        file.seekg(0); // 如果不是BOM，重置文件指针
    }

    // 跳过标题行
    std::getline(file, line);

    // 逐行读取数据
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        CarTask task;
        
        // 解析任务编号 (字段1)
        if (!std::getline(ss, token, ',')) continue;
        task.id = std::stoi(token);

        // 跳过物料编号 (字段2)
        if (!std::getline(ss, token, ',')) continue;

        // 解析任务类型 (字段3)
        if (!std::getline(ss, token, ',')) continue;
        task.task_type = (token == "入库") ? 0 : 1;

        // 解析起始设备 (字段4) - 转换为0开始索引
        if (!std::getline(ss, token, ',')) continue;
        // 设备编号减1转换为索引
        task.start_connector = std::stoi(token) - 1;

        // 解析目的设备 (字段5) - 转换为0开始索引
        if (!std::getline(ss, token)) continue;
        // 设备编号减1转换为索引
        task.end_connector = std::stoi(token) - 1;

        // 添加到任务列表
        task_list.push_back(task);
    }
}