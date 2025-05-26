#include <vector>
#include <cmath>
#include <algorithm>

#include "../../SDL3.h"
#include "../../Display.h"

/**
 * @brief 绘制圆形
 * @param centerx 圆心x坐标
 * @param centery 圆心y坐标
 * @param radius 半径
 * @param width 圆环边框宽度，为0表示不绘制边框
 * @param bg 圆环填充色，为nullptr表示不填充
 * @param fg 圆环边框色，为nullptr表示不绘制边框
 */
void Draw_Circle(float centerx, float centery, float radius, float width, SDL_Color* bg, SDL_Color* fg)
{
    // 计算填充半径
    float fill_radius = std::max(radius - width, 0.0f);

    // 绘制填充部分
    if (bg != nullptr && fill_radius > 0)
    {
        std::vector<SDL_FPoint> points;

        int start_y = static_cast<int>(std::floor(centery - fill_radius));
        int end_y = static_cast<int>(std::ceil(centery + fill_radius));

        for (int iy = start_y; iy <= end_y; ++iy)
        {
            float y = static_cast<float>(iy) + 0.5f;
            float dy = y - centery;
            float dx_sq = fill_radius * fill_radius - dy * dy;

            if (dx_sq < 0) continue;

            float dx = std::sqrt(dx_sq);
            int start_x = static_cast<int>(std::floor(centerx - dx));
            int end_x = static_cast<int>(std::ceil(centerx + dx));

            for (int ix = start_x; ix <= end_x; ++ix)
            {
                float x = static_cast<float>(ix) + 0.5f;
                points.emplace_back(SDL_FPoint{x, y});
            }
        }

        if (!points.empty())
        {
            SDL_SetRenderDrawColor(renderer, bg->r, bg->g, bg->b, bg->a);
            SDL_RenderPoints(renderer, points.data(), static_cast<int>(points.size()));
        }
    }

    // 绘制边框部分
    if (fg != nullptr && width > 0)
    {
        std::vector<SDL_FPoint> border_points;

        int start_y = static_cast<int>(std::floor(centery - radius));
        int end_y = static_cast<int>(std::ceil(centery + radius));

        for (int iy = start_y; iy <= end_y; ++iy)
        {
            float y = static_cast<float>(iy) + 0.5f;
            float dy = y - centery;
            float outer_dx_sq = radius * radius - dy * dy;

            if (outer_dx_sq < 0) continue;

            float outer_dx = std::sqrt(outer_dx_sq);
            float inner_dx_sq = fill_radius * fill_radius - dy * dy;

            if (inner_dx_sq < 0)
            {
                // 整个外圆范围都属于边框
                int start_x = static_cast<int>(std::floor(centerx - outer_dx));
                int end_x = static_cast<int>(std::ceil(centerx + outer_dx));

                for (int ix = start_x; ix <= end_x; ++ix)
                {
                    float x = static_cast<float>(ix) + 0.5f;
                    border_points.emplace_back(SDL_FPoint{x, y});
                }
            }
            else
            {
                float inner_dx = std::sqrt(inner_dx_sq);
                // 左右两边的区段
                int left_start = static_cast<int>(std::floor(centerx - outer_dx));
                int left_end = static_cast<int>(std::ceil(centerx - inner_dx)) - 1;

                for (int ix = left_start; ix <= left_end; ++ix)
                {
                    float x = static_cast<float>(ix) + 0.5f;
                    border_points.emplace_back(SDL_FPoint{x, y});
                }

                int right_start = static_cast<int>(std::ceil(centerx + inner_dx));
                int right_end = static_cast<int>(std::floor(centerx + outer_dx));

                for (int ix = right_start; ix <= right_end; ++ix)
                {
                    float x = static_cast<float>(ix) + 0.5f;
                    border_points.emplace_back(SDL_FPoint{x, y});
                }
            }
        }

        if (!border_points.empty())
        {
            SDL_SetRenderDrawColor(renderer, fg->r, fg->g, fg->b, fg->a);
            SDL_RenderPoints(renderer, border_points.data(), static_cast<int>(border_points.size()));
        }
    }
}


/**
 * @brief 绘制圆形
 * @param centerx 圆心x坐标
 * @param centery 圆心y坐标
 * @param radius 半径
 * @param topleft_radius 是否绘制左上角四分之一圆
 * @param topright_radius 是否绘制右上角四分之一圆
 * @param bottomleft_radius 是否绘制左下角四分之一圆
 * @param bottomright_radius 是否绘制右下角四分之一圆
 * @param width 圆环边框宽度，为0表示不绘制边框
 * @param bg 圆环填充色，为nullptr表示不填充
 * @param fg 圆环边框色，为nullptr表示不绘制边框
 */
void Draw_Circle(float centerx, float centery, float radius, bool topleft_radius, bool topright_radius, bool bottomleft_radius, bool bottomright_radius, float width, SDL_Color* bg, SDL_Color* fg)
{
    const float fill_radius = std::max(radius - width, 0.0f);
    const bool need_fill = bg && fill_radius > 0;
    const bool need_border = fg && width > 0;

    std::vector<SDL_FPoint> fill_points;
    std::vector<SDL_FPoint> border_points;

    // 遍历垂直范围
    const int min_y = static_cast<int>(std::floor(centery - radius));
    const int max_y = static_cast<int>(std::ceil(centery + radius));

    for (int iy = min_y; iy <= max_y; ++iy)
    {
        const float y = static_cast<float>(iy) + 0.5f;
        const float dy = y - centery;
        const float dy_sq = dy * dy;

        // 处理填充区域
        if (need_fill)
        {
            const float fill_dx_sq = fill_radius * fill_radius - dy_sq;
            if (fill_dx_sq >= 0)
            {
                const float fill_dx = std::sqrt(fill_dx_sq);
                const int start_x = static_cast<int>(centerx - fill_dx);
                const int end_x = static_cast<int>(centerx + fill_dx);

                for (int ix = start_x; ix <= end_x; ++ix)
                {
                    const float x = static_cast<float>(ix) + 0.5f;
                    const bool is_left = x <= centerx;
                    const bool is_right = x >= centerx;
                    const bool is_top = y <= centery;
                    const bool is_bottom = y >= centery;

                    if ((is_left && is_top && topleft_radius) ||
                        (is_right && is_top && topright_radius) ||
                        (is_left && is_bottom && bottomleft_radius) ||
                        (is_right && is_bottom && bottomright_radius)) 
                    {
                        fill_points.emplace_back(SDL_FPoint{x, y});
                    }
                }
            }
        }

        // 处理边框区域
        if (need_border)
        {
            const float outer_dx_sq = radius * radius - dy_sq;
            if (outer_dx_sq < 0) continue;

            const float outer_dx = std::sqrt(outer_dx_sq);
            const int start_x = static_cast<int>(centerx - outer_dx);
            const int end_x = static_cast<int>(centerx + outer_dx);

            for (int ix = start_x; ix <= end_x; ++ix)
            {
                const float x = static_cast<float>(ix) + 0.5f;
                const float dx = x - centerx;
                const float distance_sq = dx * dx + dy_sq;

                // 判断是否在边框范围内
                const bool in_border = (distance_sq > fill_radius * fill_radius) && (distance_sq <= radius * radius);

                if (in_border)
                {
                    const bool is_left = x <= centerx;
                    const bool is_right = x >= centerx;
                    const bool is_top = y <= centery;
                    const bool is_bottom = y >= centery;

                    if ((is_left && is_top && topleft_radius) ||
                        (is_right && is_top && topright_radius) ||
                        (is_left && is_bottom && bottomleft_radius) ||
                        (is_right && is_bottom && bottomright_radius)) 
                    {
                        border_points.emplace_back(SDL_FPoint{x, y});
                    }
                }
            }
        }
    }

    // 批量绘制
    if (!fill_points.empty())
    {
        SDL_SetRenderDrawColor(renderer, bg->r, bg->g, bg->b, bg->a);
        SDL_RenderPoints(renderer, fill_points.data(), static_cast<int>(fill_points.size()));
    }
    if (!border_points.empty())
    {
        SDL_SetRenderDrawColor(renderer, fg->r, fg->g, fg->b, fg->a);
        SDL_RenderPoints(renderer, border_points.data(), static_cast<int>(border_points.size()));
    }
}


/**
 * @brief 绘制矩形
 * @param rect 矩形区域
 * @param width 矩形边框宽度，为0表示不绘制边框
 * @param bg 矩形填充色，为nullptr表示不填充
 * @param fg 矩形边框色，为nullptr表示不绘制边框
 * @note 边框的中心为矩形边缘
 */
void Draw_Rect(SDL_FRect rect, float width, SDL_Color* bg, SDL_Color* fg)
{
    if (width > 0)
    {
        SDL_FRect border_rects[4] = {
            {rect.x - width/2, rect.y - width/2, rect.w + width, width}, // 上边框
            {rect.x - width/2, rect.y + rect.h - width/2, rect.w + width, width}, // 下边框
            {rect.x - width/2, rect.y + width/2, width, rect.h - width}, // 左边框
            {rect.x + rect.w - width/2, rect.y + width/2, width, rect.h - width}  // 右边框
        };
        SDL_SetRenderDrawColor(renderer, fg->r, fg->g, fg->b, fg->a);
        SDL_RenderFillRects(renderer, border_rects, 4);
        rect.w -= width;
        rect.h -= width;
        rect.x += width/2;
        rect.y += width/2;
    }
    if (bg != nullptr)
    {
        SDL_SetRenderDrawColor(renderer, bg->r, bg->g, bg->b, bg->a);
        SDL_RenderFillRect(renderer, &rect);
    }
}

/**
 * @brief 绘制圆角矩形
 * @param rect 矩形区域
 * @param width 矩形边框宽度，为0表示不绘制边框
 * @param bg 矩形填充色，为nullptr表示不填充
 * @param fg 矩形边框色，为nullptr表示不绘制边框
 * @param radius 圆角半径
 */
void Draw_Rect(SDL_FRect rect, float width, SDL_Color* bg, SDL_Color* fg, float radius)
{
    // 统一各角半径并限制最大值
    const float max_radius = std::min(rect.w, rect.h) / 2;
    radius = std::min(radius, max_radius);
    Draw_Rect(rect, width, bg, fg, radius, radius, radius, radius);
}

/**
 * @brief 绘制圆角矩形
 * @param rect 矩形区域
 * @param width 矩形边框宽度，为0表示不绘制边框
 * @param bg 矩形填充色，为nullptr表示不填充
 * @param fg 矩形边框色，为nullptr表示不绘制边框
 * @param topleft_radius 左上角圆角半径
 * @param topright_radius 右上角圆角半径
 * @param bottomleft_radius 左下角圆角半径
 * @param bottomright_radius 右下角圆角半径
 */
void Draw_Rect(SDL_FRect rect, float width, SDL_Color* bg, SDL_Color* fg, float topleft_radius, float topright_radius, float bottomleft_radius, float bottomright_radius)
{
    // 限制各角半径不超过可用空间
    topleft_radius = std::min(topleft_radius, std::min(rect.w, rect.h)/2);
    topright_radius = std::min(topright_radius, std::min(rect.w, rect.h)/2);
    bottomleft_radius = std::min(bottomleft_radius, std::min(rect.w, rect.h)/2);
    bottomright_radius = std::min(bottomright_radius, std::min(rect.w, rect.h)/2);

    const bool need_fill = bg != nullptr;
    const bool need_border = fg != nullptr && width > 0;

    // 绘制四个圆角
    if (need_fill || need_border)
    {
        Draw_Circle(rect.x + topleft_radius, rect.y + topleft_radius, topleft_radius, true, false, false, false, width, bg, fg);                                  // 左上角圆
        Draw_Circle(rect.x + rect.w - topright_radius, rect.y + topright_radius, topright_radius, false, true, false, false, width, bg, fg);                      // 右上角圆
        Draw_Circle(rect.x + bottomleft_radius, rect.y + rect.h - bottomleft_radius, bottomleft_radius, false, false, true, false, width, bg, fg);                // 左下角圆
        Draw_Circle(rect.x + rect.w - bottomright_radius, rect.y + rect.h - bottomright_radius, bottomright_radius, false, false, false, true, width, bg, fg);    // 右下角圆
    }

    // 绘制填充中心区域
    if (need_fill)
    {
        SDL_FRect center_rect = {
            rect.x + std::max(topleft_radius, bottomleft_radius),
            rect.y + std::max(topleft_radius, topright_radius),
            rect.w - std::max(topleft_radius, bottomleft_radius) - std::max(topright_radius, bottomright_radius),
            rect.h - std::max(topleft_radius, topright_radius) - std::max(bottomleft_radius, bottomright_radius)
        };
        if (center_rect.w > 0 && center_rect.h > 0)
        {
            SDL_SetRenderDrawColor(renderer, bg->r, bg->g, bg->b, bg->a);
            SDL_RenderFillRect(renderer, &center_rect);
        }

        // 绘制横向填充条
        SDL_FRect top_rect = {
            rect.x + topleft_radius,
            rect.y,
            rect.w - topleft_radius - topright_radius,
            std::max(topleft_radius, topright_radius)
        };
        SDL_FRect bottom_rect = {
            rect.x + bottomleft_radius,
            rect.y + rect.h - std::max(bottomleft_radius, bottomright_radius),
            rect.w - bottomleft_radius - bottomright_radius,
            std::max(bottomleft_radius, bottomright_radius)
        };
        if (top_rect.h > 0 && top_rect.w > 0) SDL_RenderFillRect(renderer, &top_rect);
        if (bottom_rect.h > 0 && bottom_rect.w > 0) SDL_RenderFillRect(renderer, &bottom_rect);

        // 绘制纵向填充条
        SDL_FRect left_rect = {
            rect.x,
            rect.y + topleft_radius,
            std::max(topleft_radius, bottomleft_radius),
            rect.h - topleft_radius - bottomleft_radius
        };
        SDL_FRect right_rect = {
            rect.x + rect.w - std::max(topright_radius, bottomright_radius),
            rect.y + topright_radius,
            std::max(topright_radius, bottomright_radius),
            rect.h - topright_radius - bottomright_radius
        };
        if (left_rect.w > 0 && left_rect.h > 0) SDL_RenderFillRect(renderer, &left_rect);
        if (right_rect.w > 0 && right_rect.h > 0) SDL_RenderFillRect(renderer, &right_rect);
    }

    // 绘制边框直线部分
    if (need_border) {
        // 上边框
        SDL_FRect top_border = {
            rect.x + topleft_radius - width/2,
            rect.y - width/2,
            rect.w - topleft_radius - topright_radius + width,
            width
        };
        // 下边框
        SDL_FRect bottom_border = {
            rect.x + bottomleft_radius - width/2,
            rect.y + rect.h - width/2,
            rect.w - bottomleft_radius - bottomright_radius + width,
            width
        };
        // 左边框
        SDL_FRect left_border = {
            rect.x - width/2,
            rect.y + topleft_radius - width/2,
            width,
            rect.h - topleft_radius - bottomleft_radius + width
        };
        // 右边框
        SDL_FRect right_border = {
            rect.x + rect.w - width/2,
            rect.y + topright_radius - width/2,
            width,
            rect.h - topright_radius - bottomright_radius + width
        };

        SDL_SetRenderDrawColor(renderer, fg->r, fg->g, fg->b, fg->a);
        SDL_RenderFillRect(renderer, &top_border);
        SDL_RenderFillRect(renderer, &bottom_border);
        SDL_RenderFillRect(renderer, &left_border);
        SDL_RenderFillRect(renderer, &right_border);
    }
}
