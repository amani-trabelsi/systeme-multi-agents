#include "rendu.h"
#include <SDL2/SDL_image.h>

SDL_Texture *texLoup = NULL;
SDL_Texture *texMouton = NULL;
SDL_Texture *texFond = NULL;
SDL_Texture *texHerbe = NULL;
SDL_Texture *texObstacle = NULL;

#define COLS 4
#define ROWS 5

#define HERBE_COLS 3
#define HERBE_ROWS 2

#define OBSTACLE_COLS 3
#define OBSTACLE_ROWS 3

int wLoup;
int hLoup;
int wMouton;
int hMouton;
int wFond;
int hFond;
int wHerbe;
int hHerbe;
int wObstacle;
int hObstacle;

int frame = 0;

int rendu_init(SDL_Renderer *ren) {
  SDL_Surface *s;

  IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

  s = IMG_Load("sprites/loup.png");
  if (s) {
    wLoup = s->w;
    hLoup = s->h;
    texLoup = SDL_CreateTextureFromSurface(ren, s);
    SDL_FreeSurface(s);
  }

  s = IMG_Load("sprites/mouton.png");
  if (s) {
    wMouton = s->w;
    hMouton = s->h;
    texMouton = SDL_CreateTextureFromSurface(ren, s);
    SDL_FreeSurface(s);
  }

  s = IMG_Load("sprites/herbe_texture.png");
  if (s) {
    wFond = s->w;
    hFond = s->h;
    texFond = SDL_CreateTextureFromSurface(ren, s);
    SDL_FreeSurface(s);
  }

  s = IMG_Load("sprites/herbe.png");
  if (s) {
    wHerbe = s->w;
    hHerbe = s->h;
    texHerbe = SDL_CreateTextureFromSurface(ren, s);
    SDL_FreeSurface(s);
  }

  s = IMG_Load("sprites/roche.png");
  if (s) {
    wObstacle = s->w;
    hObstacle = s->h;
    texObstacle = SDL_CreateTextureFromSurface(ren, s);
    SDL_FreeSurface(s);
  }

  return texLoup != NULL && texMouton != NULL && texFond != NULL &&
         texHerbe != NULL && texObstacle != NULL;
}

void rendu_quit(void) {
  if (texLoup)
    SDL_DestroyTexture(texLoup);

  if (texMouton)
    SDL_DestroyTexture(texMouton);

  if (texFond)
    SDL_DestroyTexture(texFond);

  if (texHerbe)
    SDL_DestroyTexture(texHerbe);

  if (texObstacle)
    SDL_DestroyTexture(texObstacle);

  IMG_Quit();
}

void rect(SDL_Renderer *r, int x, int y, int w, int h, int R, int G, int B) {
  SDL_Rect rc;

  SDL_SetRenderDrawColor(r, R, G, B, 255);

  rc.x = x;
  rc.y = y;
  rc.w = w;
  rc.h = h;

  SDL_RenderFillRect(r, &rc);
}

void image_fond(SDL_Renderer *ren) {
  SDL_Rect dest;

  dest.x = 0;
  dest.y = 0;
  dest.w = L * TILE;
  dest.h = H * TILE;

  SDL_RenderCopy(ren, texFond, NULL, &dest);
}

void dessiner_sprite_sheet(SDL_Renderer *ren, SDL_Texture *tex, int image_w,
                           int image_h, int nb_cols, int nb_rows, int ligne,
                           int col, int gx, int gy, float taille,
                           int flip_horizontal) {//taille indique combien de cases de hauteur le sprite doit prendre à l’écran.
  SDL_Rect source;//la partie de l'image qu'on va decouper 
  SDL_Rect dest;//l'endroit ou on va dessiner la fenetre 
  SDL_RendererFlip flip; //indique si on retourne le frame ou non 

  int frame_w;//largeur de la petite frame 
  int frame_h;//hauteur """"
  float zoom;//facteur d'agrandissement 

  frame_w = image_w / nb_cols;
  frame_h = image_h / nb_rows;

  source.x = col * frame_w;//(x,y):la position de frame dans le sprite sheetb 
  source.y = ligne * frame_h;
  source.w = frame_w;
  source.h = frame_h;

  zoom = (TILE * taille) / source.h; //calcul de facteur pour affciher le frame a la bonne taille 

  dest.w = source.w * zoom;
  dest.h = source.h * zoom;
  dest.x = gx * TILE + (TILE - dest.w) / 2;
  dest.y = gy * TILE + (TILE - dest.h) / 2;

  if (flip_horizontal) {
    flip = SDL_FLIP_HORIZONTAL;
  } else {
    flip = SDL_FLIP_NONE;
  }

  SDL_RenderCopyEx(ren, tex, &source, &dest, 0, NULL, flip);
  //on copie la partie src de la sprite sheet dans dest dans le bon endroit 
}
void barre_energie(SDL_Renderer *r, int gx, int gy, float frac) {
  int x = gx * TILE + 3;
  int y = gy * TILE + 1;
  int w = TILE - 6;

  if (frac < 0)
    frac = 0;
  if (frac > 1)
    frac = 1;

  rect(r, x, y, w, 4, 30, 30, 30);
  rect(r, x, y, w * frac, 4, 255 * (1 - frac), 255 * frac, 40);
}

void dessiner(SDL_Renderer *ren, Monde *M) {
  int x;//(x et y )servent a parcourir les case de la grille 
  int y;
  int i;//sert à parcourir les agents
  int ligne;//sert à choisir la ligne d’animation du loup/mouton
  int col_sprite; //col de frame 
  int ligne_sprite;//ligne de frame //pour 

  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);

  if (texFond) {
    image_fond(ren);
  }

  // Dessiner l'herbe, mais pas sur les obstacles 
  for (y = 0; y < H; y++) {
    for (x = 0; x < L; x++) {
      if (M->grille[y][x] == 0) {//case vlibre il n'ya pas d'obstacle 
        if (M->herbe[y][x] == 0) {//il y a de l'herbe 
          if (texHerbe) {
            col_sprite = x % HERBE_COLS;//choisit une col de la sprite 
            ligne_sprite = y % HERBE_ROWS;

            dessiner_sprite_sheet(ren, texHerbe, wHerbe, hHerbe, HERBE_COLS,
                                  HERBE_ROWS, ligne_sprite, col_sprite, x, y,
                                  2.0f, 0);
          }
        } else if (M->herbe[y][x] == ARIDE) {
          //on fait rien on laisse le fond tel qu'il est 
        } else {
          //si herbe mangé recmement on laisse le fond  
        }
      }
    }
  }

  frame = frame + 1;
  frame = frame % COLS;

  // Dessiner les loups et les moutons 
  for (i = 0; i < NB_TOTAL; i++) {
    if (M->ag[i].vivant) {
      if (M->ag[i].bouge) {
        ligne = 1;
      } else {
        ligne = 0;
      }

      if (M->ag[i].loup) {
        if (texLoup) {
          dessiner_sprite_sheet(ren, texLoup, wLoup, hLoup, COLS, ROWS, ligne,
                                frame, M->ag[i].x, M->ag[i].y, 2.1f,
                                M->ag[i].regard == OUEST);
        } else {
          rect(ren, M->ag[i].x * TILE, M->ag[i].y * TILE, TILE, TILE, 60, 60,
               80);
        }
      } else {
        if (texMouton) {
          dessiner_sprite_sheet(ren, texMouton, wMouton, hMouton, COLS, ROWS,
                                ligne, frame, M->ag[i].x, M->ag[i].y, 2.1f,
                                M->ag[i].regard == OUEST);
        } else {
          rect(ren, M->ag[i].x * TILE, M->ag[i].y * TILE, TILE, TILE, 240, 240,
               235);
        }
      }

      barre_energie(ren, M->ag[i].x, M->ag[i].y,
                    M->ag[i].energie / ENERGIE_MAX);
    }
  }

  //partie dessiner les obstacles
  for (y = 0; y < H; y++) {
    for (x = 0; x < L; x++) {
      if (M->grille[y][x] == 1) {
        if (texObstacle) {
        col_sprite = x % OBSTACLE_COLS;
        ligne_sprite = y % OBSTACLE_ROWS;

          dessiner_sprite_sheet(ren, texObstacle, wObstacle, hObstacle,
                                OBSTACLE_COLS, OBSTACLE_ROWS, ligne_sprite,
                                col_sprite, x, y, 2.0f, 0);
        } else {
          rect(ren, x * TILE, y * TILE, TILE, TILE, 110, 95, 80);
        }
      }
    }
  }

  SDL_RenderPresent(ren);
}