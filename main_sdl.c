#include "alea.h"
#include "episode.h"
#include "jeu.h"
#include "monde.h"
#include "politique.h"
#include "rendu.h"

#include <stdio.h>
#include <time.h>

// charger la politique a partir du fichier txt
int charger(char nom[], Politique *pi) {
  FILE *f = fopen(nom, "r");
  if (!f)
    return 0;

  int nact, nfeat;

  if (fscanf(f, "%d %d", &nact, &nfeat) != 2) {
    fclose(f);
    return 0;
  }

  for (int a = 0; a < NACT; a++)
    for (int k = 0; k < NFEAT; k++)
      if (fscanf(f, "%f", &pi->theta[a][k]) != 1) {
        fclose(f);
        return 0;
      }

  fclose(f);
  return 1;
}
int main(void) {
  Rng rng = (Rng)time(NULL);

  Politique piL, piM;
  politique_init(&piL);
  politique_init(&piM);

  if (!charger("politique_loup.txt", &piL) ||
      !charger("politique_mouton.txt", &piM)) {
    printf("Politiques introuvables lance d'abord ./train.\n");
    return 1;
  }

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *win = SDL_CreateWindow(
      "Loups & Moutons", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      L * TILE, H * TILE, SDL_WINDOW_SHOWN);

  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

  rendu_init(ren);

  Monde M;
  monde_init(&M, &rng);

  int run = 1;

  while (run) {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        run = 0;
    }

    if (episode_un_pas(&M, &piL, &piM, &rng) == 0)
      monde_init(&M, &rng);

    dessiner(ren, &M);
    SDL_Delay(250);
  }

  rendu_quit();
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}