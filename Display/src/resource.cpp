#include "../../SDL3.h"
#include "../../Display.h"

#include <unordered_map>
#include <string>

 extern std::string work_path;    // 工作目录

/**
 * @brief 加载图像
 * @param path 图像路径
 */
void ResourceManager::loadImage(const std::string &path)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, path.c_str());
    if (texture == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load image: %s", path.c_str());
        return;
    }
    textures_[path] = texture;
}

/**
 * @brief 加载字体
 * @param path 字体路径
 * @param font_size 字体大小
 */
void ResourceManager::loadFont(const std::string &path, float font_size)
{
    TTF_Font *font = TTF_OpenFont(path.c_str(), font_size);
    if (font == nullptr)
    {
        // SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load font: %s", path.c_str());
        return;
    }
    fonts_[path + std::to_string(font_size)] = font;
}

/**
 * @brief 加载颜色
 * @param color_name 颜色名称
 * @return 颜色对象
 */
void ResourceManager::loadColor(const std::string &color_name)
{
    SDL_Color color;
    unsigned int r, g, b;
    #ifdef MSVC_BUILD
    if (sscanf_s(color_name.c_str(), "#%02x%02x%02x", &r, &g, &b) != 3)
    #else
    if (sscanf(color_name.c_str(), "#%02x%02x%02x", &r, &g, &b) != 3)
    #endif
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to parse color: %s", color_name.c_str());
        return;
    }
    color.r = static_cast<Uint8>(r);
    color.g = static_cast<Uint8>(g);
    color.b = static_cast<Uint8>(b);
    color.a = 255;
    #ifdef DARKMODE
    color.r = 255 - color.r;
    color.g = 255 - color.g;
    color.b = 255 - color.b;
    #endif
    colors_[color_name] = color;
}

/**
 * @brief 获取图像
 * @param path 图像路径
 * @return 图像纹理指针
 */
SDL_Texture* ResourceManager::getImage(const std::string &path)
{
    std::string file_path = work_path + IMAGE_PATH + path;    // 拼接图像路径
    auto iter = textures_.find(file_path);
    if (iter == textures_.end())
    {
        loadImage(file_path);
        iter = textures_.find(file_path);
    }
    if (iter == textures_.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get image: %s", path.c_str());
        return nullptr;
    }
    return iter->second;
}

/**
 * @brief 获取字体
 * @param path 字体路径
 * @param font_size 字体大小
 * @return 字体指针
 */
TTF_Font* ResourceManager::getFont(const std::string &path, float font_size)
{
    std::string file_path = work_path + FONT_PATH + path;    // 拼接字体路径
    auto iter = fonts_.find(file_path + std::to_string(font_size));
    if (iter == fonts_.end())
    {
        loadFont(file_path, font_size);
        iter = fonts_.find(file_path + std::to_string(font_size));
    }
    if (iter == fonts_.end())
    {
        // SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get font: %s", file_path.c_str());
        return nullptr;
    }
    return iter->second;
}

/**
 * @brief 获取颜色
 * @param color_name 颜色名称
 * @return 颜色对象
 */
SDL_Color ResourceManager::getColor(const std::string &color_name)
{
    auto iter = colors_.find(color_name);
    if (iter == colors_.end())
    {
        loadColor(color_name);
        iter = colors_.find(color_name);
    }
    if (iter == colors_.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get color: %s", color_name.c_str());
        return {0, 0, 0, 255};
    }
    return iter->second;
}

/**
 * @brief 获取颜色指针
 * @param color_name 颜色名称
 * @return 颜色指针
 */
SDL_Color* ResourceManager::getColor_(const std::string &color_name)
{
    auto iter = colors_.find(color_name);
    if (iter == colors_.end())
    {
        loadColor(color_name);
        iter = colors_.find(color_name);
    }
    if (iter == colors_.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get color pointer: %s", color_name.c_str());
        return nullptr;
    }
    return &(iter->second);
}

/**
 * @brief 释放资源
 */
void ResourceManager::Clean(void)
{
    for (auto &texture : textures_)
    {
        SDL_DestroyTexture(texture.second);
    }
    textures_.clear();

    for (auto &font : fonts_)
    {
        TTF_CloseFont(font.second);
    }
    fonts_.clear();
}

ResourceManager rm;    // 资源管理器实例

/**
 * @brief 预加载资源
 * @param r 资源管理器实例
 */
void pre_load(ResourceManager &r)
{
    r.getImage("icon.png");
    r.getFont("deyi.ttf", 16);
    r.getFont("deyi.ttf", 20);
    r.getFont("deyi.ttf", 24);
    r.getFont("deyi.ttf", 32);
    r.getFont("deyi.ttf", 48);
    r.getFont("deyi.ttf", 64);
    r.getFont("deyi.ttf", 72);
    r.getFont("deyi.ttf", 128);
    r.getFont("shuhei.ttf", 16);
    r.getFont("shuhei.ttf", 20);
    r.getFont("shuhei.ttf", 24);
    r.getFont("shuhei.ttf", 32);
    r.getFont("shuhei.ttf", 48);
    r.getFont("shuhei.ttf", 64);
    r.getFont("shuhei.ttf", 72);
    r.getFont("shuhei.ttf", 128);
}
