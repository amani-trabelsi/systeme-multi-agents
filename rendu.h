#ifndef RENDU_H
#define RENDU_H

#include "jeu.h"
#include <SDL2/SDL.h>

int rendu_init(SDL_Renderer *ren);
void rendu_quit(void);
void dessiner(SDL_Renderer *ren, Monde *M);

#endif