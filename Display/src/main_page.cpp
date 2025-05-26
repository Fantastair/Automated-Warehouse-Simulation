#include <iostream>
#include <string>

#include "../../Core.h"
#include "../../SDL3.h"
#include "../../Display.h"

#define GRID_SIZE 60



/**
 * @brief 初始化主页面
 */
void main_page_Init(void)
{
    root = new BgUi(rm.getColor(FAKEWHITE));

    RectUi *grid = nullptr;
    for (int i = 1; i < WINDOW_WIDTH / GRID_SIZE; i++)
    {
        grid = new RectUi(2, 600, 0, rm.getColor_(DEEPWHITE), nullptr);
        grid->set_rect_midtop((float)(i * GRID_SIZE), 0);
        grid->join(*root);
    }
    for (int i = 1; i < 600 / GRID_SIZE; i++)
    {
        grid = new RectUi(WINDOW_WIDTH, 2, 0, rm.getColor_(DEEPWHITE), nullptr);
        grid->set_rect_midleft(0, (float)(i * GRID_SIZE));
        grid->join(*root);
    }

    RectUi *split_line = new RectUi(WINDOW_WIDTH, 48, 0, rm.getColor_(DARKBLUE), nullptr);
    split_line->set_rect_top(600 - 4);
    split_line->join(*root);
    TextUi *text = new TextUi("控制面板", rm.getFont("shuhei.ttf", 32), rm.getColor(FAKEWHITE));
    text->set_rect_center(split_line->rect.w / 2, split_line->rect.h / 2);
    text->join(*split_line);
    RectUi *border = new RectUi(WINDOW_WIDTH - 8, WINDOW_HEIGHT - 8, 8, nullptr, rm.getColor_(DARKBLUE));
    border->set_rect_topleft(4, 4);
    border->join(*root);

    RectUi *inner_track = new RectUi(
        (STRIGHT_TRACKLENGTH + CURVE_TRACKRADIUS * 2) * ZOOM_FACTOR,
        (CURVE_TRACKRADIUS * 2) * ZOOM_FACTOR,
        4,
        nullptr,
        rm.getColor_(DARKBLUE),
        (CURVE_TRACKRADIUS) * ZOOM_FACTOR
    );
    inner_track->set_rect_center(WINDOW_WIDTH / 2, 600 / 2);
    inner_track->join(*root);
    RectUi *outer_track = new RectUi(
        (STRIGHT_TRACKLENGTH + CURVE_TRACKRADIUS * 2 + TRACKWIDTH * 2) * ZOOM_FACTOR,
        (CURVE_TRACKRADIUS * 2 + TRACKWIDTH * 2) * ZOOM_FACTOR,
        4,
        nullptr,
        rm.getColor_(DARKBLUE),
        (CURVE_TRACKRADIUS + TRACKWIDTH) * ZOOM_FACTOR
    );
    outer_track->set_rect_center(WINDOW_WIDTH / 2, 600 / 2);
    outer_track->join(*root);
    RectUi *cover = new RectUi(2, 4, 0, rm.getColor_(FAKEWHITE), nullptr);
    cover->set_rect_topleft(10, 298);
    cover->join(*root);
    cover = new RectUi(2, 4, 0, rm.getColor_(FAKEWHITE), nullptr);
    cover->set_rect_topleft(58, 298);
    cover->join(*root);
    cover = new RectUi(2, 4, 0, rm.getColor_(FAKEWHITE), nullptr);
    cover->set_rect_topleft(WINDOW_WIDTH - 12, 298);
    cover->join(*root);
    cover = new RectUi(2, 4, 0, rm.getColor_(FAKEWHITE), nullptr);
    cover->set_rect_topleft(WINDOW_WIDTH - 60, 298);
    cover->join(*root);

    RectUi *connector;
    TextUi *arrow;
    for (int i = 0; i < 6; i++)
    {
        connector = new RectUi(
            CONNECTOR_WIDTH * ZOOM_FACTOR,
            (CONNECTOR_WIDTH * 2) * ZOOM_FACTOR,
            4,
            nullptr,
            rm.getColor_(DARKBLUE),
            8
        );
        connector->set_rect_bottomleft(inner_track->rect.x + (CURVE_TRACKRADIUS + CS_1_LEFTPAD + (CONNECTOR_WIDTH * 2 + CS2SC_DISTANCE + SC2CS_DISTANCE) * i) * ZOOM_FACTOR, outer_track->rect.y - 6);
        connector->join(*root);
        arrow = new TextUi(ICON_UP_ARROW, rm.getFont("iconfont.ttf", 32), rm.getColor(DARKBLUE));
        arrow->set_rect_center(connector->rect.w / 2, connector->rect.h / 2);
        arrow->join(*connector);

        connector = new RectUi(
            CONNECTOR_WIDTH * ZOOM_FACTOR,
            (CONNECTOR_WIDTH * 2) * ZOOM_FACTOR,
            4,
            nullptr,
            rm.getColor_(DARKBLUE),
            8
        );
        connector->set_rect_bottomleft(inner_track->rect.x + (CURVE_TRACKRADIUS + CS_1_LEFTPAD + (CONNECTOR_WIDTH * 2 + CS2SC_DISTANCE + SC2CS_DISTANCE) * i + CONNECTOR_WIDTH + CS2SC_DISTANCE) * ZOOM_FACTOR, outer_track->rect.y - 6);
        connector->join(*root);
        arrow = new TextUi(ICON_DOWN_ARROW, rm.getFont("iconfont.ttf", 32), rm.getColor(DARKBLUE));
        arrow->set_rect_center(connector->rect.w / 2, connector->rect.h / 2);
        arrow->join(*connector);
    }
    for (int i = 0; i < 3; i++)
    {
        connector = new RectUi(
            CONNECTOR_WIDTH * ZOOM_FACTOR,
            (CONNECTOR_WIDTH * 2) * ZOOM_FACTOR,
            4,
            nullptr,
            rm.getColor_(DARKBLUE),
            8
        );
        connector->set_rect_topleft(inner_track->rect.x + (CURVE_TRACKRADIUS + PC_18_LEFTPAD + (CONNECTOR_WIDTH + PC2PC_DISTANCE) * i) * ZOOM_FACTOR, outer_track->rect.y + outer_track->rect.h + 6);
        connector->join(*root);
        arrow = new TextUi(ICON_UP_ARROW, rm.getFont("iconfont.ttf", 32), rm.getColor(DARKBLUE));
        arrow->set_rect_center(connector->rect.w / 2, connector->rect.h / 2);
        arrow->join(*connector);
        
        connector = new RectUi(
            CONNECTOR_WIDTH * ZOOM_FACTOR,
            (CONNECTOR_WIDTH * 2) * ZOOM_FACTOR,
            4,
            nullptr,
            rm.getColor_(DARKBLUE),
            8
        );
        connector->set_rect_topleft(inner_track->rect.x + (CURVE_TRACKRADIUS + PC_18_LEFTPAD + (CONNECTOR_WIDTH + PC2PC_DISTANCE) * i + CONNECTOR_WIDTH * 3 + PC2PC_DISTANCE * 2 + PC2CP_DISTANCE) * ZOOM_FACTOR, outer_track->rect.y + outer_track->rect.h + 6);
        connector->join(*root);
        arrow = new TextUi(ICON_DOWN_ARROW, rm.getFont("iconfont.ttf", 32), rm.getColor(DARKBLUE));
        arrow->set_rect_center(connector->rect.w / 2, connector->rect.h / 2);
        arrow->join(*connector);
    }
}
