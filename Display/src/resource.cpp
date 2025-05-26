#include "../../SDL3.h"
#include "../../Display.h"

#include <unordered_map>
#include <string>

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
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load font: %s", path.c_str());
        return;
    }
    fonts_[path + std::to_string(font_size)] = font;
}

/**
 * @brief 获取图像
 * @param path 图像路径
 * @return 图像纹理指针
 */
SDL_Texture *ResourceManager::getImage(const std::string &path)
{
    std::string file_path = IMAGE_PATH + path;    // 拼接图像路径
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
TTF_Font *ResourceManager::getFont(const std::string &path, float font_size)
{
    std::string file_path = FONT_PATH + path;    // 拼接字体路径
    auto iter = fonts_.find(file_path + std::to_string(font_size));
    if (iter == fonts_.end())
    {
        loadFont(file_path, font_size);
        iter = fonts_.find(file_path + std::to_string(font_size));
    }
    if (iter == fonts_.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get font: %s", file_path.c_str());
        return nullptr;
    }
    return iter->second;
}

/**
 * @brief 释放资源
 */
void ResourceManager::Quit(void)
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
    r;
    // r.getImage("icon.png");
    // r.getFont("deyi.ttf", 16);
    // r.getFont("deyi.ttf", 20);
    // r.getFont("deyi.ttf", 24);
    // r.getFont("deyi.ttf", 32);
    // r.getFont("deyi.ttf", 48);
    // r.getFont("deyi.ttf", 64);
    // r.getFont("deyi.ttf", 72);
    // r.getFont("deyi.ttf", 128);
    // r.getFont("shuhei.ttf", 16);
    // r.getFont("shuhei.ttf", 20);
    // r.getFont("shuhei.ttf", 24);
    // r.getFont("shuhei.ttf", 32);
    // r.getFont("shuhei.ttf", 48);
    // r.getFont("shuhei.ttf", 64);
    // r.getFont("shuhei.ttf", 72);
    // r.getFont("shuhei.ttf", 128);
}
