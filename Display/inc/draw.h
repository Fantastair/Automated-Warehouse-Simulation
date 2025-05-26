#ifndef __DRAW_H__
#define __DRAW_H__

void Draw_Circle(float centerx, float centery, float radius, float width=0, SDL_Color* bg=nullptr, SDL_Color* fg=nullptr);
void Draw_Circle(float centerx, float centery, float radius, bool topleft_radius, bool topright_radius, bool bottomleft_radius, bool bottomright_radius, float width, SDL_Color* bg, SDL_Color* fg);

void Draw_Rect(SDL_FRect rect, float width, SDL_Color* bg, SDL_Color* fg);
void Draw_Rect(SDL_FRect rect, float width, SDL_Color* bg, SDL_Color* fg, float radius);
void Draw_Rect(SDL_FRect rect, float width, SDL_Color* bg, SDL_Color* fg, float topleft_radius, float topright_radius, float bottomleft_radius, float bottomright_radius);

#endif
