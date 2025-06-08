#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <unordered_map>
#include <string>

/**
 * @brief 资源管理类
 * @note 该类用于管理图像和字体资源
 */
class ResourceManager
{
    #ifdef MSVC_BUILD
        const std::string IMAGE_PATH = "/res/image/";          // 图像路径
        const std::string FONT_PATH = "/res/font/";            // 字体路径
    #else
        const std::string IMAGE_PATH = "/../Resources/res/image/";          // 图像路径
        const std::string FONT_PATH = "/../Resources/res/font/";            // 字体路径
    #endif
    std::unordered_map<std::string, SDL_Texture*> textures_;    // 图像字典
    std::unordered_map<std::string, TTF_Font*> fonts_;          // 字体字典
    std::unordered_map<std::string, SDL_Color> colors_;         // 颜色字典

    void loadImage(const std::string &path);                    // 加载图像
    void loadFont(const std::string &path, float font_size);    // 加载字体
    void loadColor(const std::string &color_name);              // 加载颜色

public:
    ResourceManager() = default;     // 构造函数
    ~ResourceManager() = default;    // 析构函数
    
    SDL_Texture* getImage(const std::string &path);                 // 获取图像
    TTF_Font* getFont(const std::string &path, float font_size);    // 获取字体
    SDL_Color getColor(const std::string &color_name);              // 获取颜色
    SDL_Color* getColor_(const std::string &color_name);            // 获取颜色指针

    void Clean(void);    // 释放资源
};

extern ResourceManager rm;    // 资源管理器实例

void pre_load(ResourceManager &r);    // 预加载资源

#endif
