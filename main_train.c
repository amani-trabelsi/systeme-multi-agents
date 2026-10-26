#include "alea.h"
#include "episode.h"
#include "jeu.h"
#include "monde.h"
#include "politique.h"
#include "reinforce.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>

#define GENERATIONS 600
#define N_EPISODES 20
#define MAX_STEPS 200
#define GAMMA 0.99f
#define ALPHA0 0.05f
#define ALPHA_FIN 0.005f
#define BETA_BASE 0.10f
#define NB_THREADS 4

//sauvegarder une politique dans un fichier txt
void sauver(char nom[], Politique *pi) {
  FILE *f = fopen(nom, "w");
  if (!f)
    return;

  fprintf(f, "%d %d\n", NACT, NFEAT);

  for (int a = 0; a < NACT; a++) {
    for (int k = 0; k < NFEAT; k++)
      fprintf(f, "%f ", pi->theta[a][k]);
    fprintf(f, "\n");
  }

  fclose(f);
}
//les données de chaque thread
typedef struct {
  Politique *piL;
  Politique *piM;
  Baseline *blL;
  Baseline *blM;
  int n_episodes;
  Rng rng;
  GradAccum gL;
  GradAccum gM;
  float tot_capt;
  float tot_survie;
} Tache;
//fct executée par chaque thread
int worker(void *arg) {
  Tache *t = (Tache *)arg;
  //mettre les grad et les stat du thread a zero
  grad_reset(&t->gL);
  grad_reset(&t->gM);
  t->tot_capt = 0.f;
  t->tot_survie = 0.f;
  //chaque thread lance plusieurs episodes
  for (int e = 0; e < t->n_episodes; e++) {
    Monde M;
    Trajectoire trajs[NB_TOTAL];
    monde_init(&M, &t->rng);
    //jouer un episode complet et recuperer des stats
    StatsEpisode st =
        episode_run(&M, t->piL, t->piM, trajs, MAX_STEPS, 1, &t->rng);
    t->tot_capt += st.captures;
    t->tot_survie += st.survie_moy;
    //on accumule les grads de loup
    for (int i = 0; i < NB_LOUP; i++)
      if (trajs[i].T > 0)
        reinforce_accumuler(t->piL, &trajs[i], &t->gL, GAMMA, t->blL);
    //on accumule les grads des moutons
    for (int i = NB_LOUP; i < NB_TOTAL; i++)
      if (trajs[i].T > 0)
        reinforce_accumuler(t->piM, &trajs[i], &t->gM, GAMMA, t->blM);
  }
  return 0;
}

int main(void) {
  Rng rng = (Rng)time(NULL);

  Politique piL, piM;
  Baseline blL, blM;

  politique_init(&piL);
  politique_init(&piM);
  baseline_init(&blL);
  baseline_init(&blM);

  float raison = powf(ALPHA_FIN / ALPHA0, 1.f / (GENERATIONS - 1));

  Tache taches[NB_THREADS];
  thrd_t th[NB_THREADS];

  for (int j = 0; j < NB_THREADS; j++) {
    taches[j].piL = &piL;
    taches[j].piM = &piM;
    taches[j].blL = &blL;
    taches[j].blM = &blM;
    taches[j].n_episodes = N_EPISODES / NB_THREADS;
    taches[j].rng = rng + j * 100;
  }

  printf("Entrainement parallele\n");
  printf("gen | alpha | captures | survie\n");

  for (int gen = 0; gen < GENERATIONS; gen++) {
    float alpha = ALPHA0 * powf(raison, gen);

    for (int j = 0; j < NB_THREADS; j++)
      thrd_create(&th[j], worker, &taches[j]);

    for (int j = 0; j < NB_THREADS; j++) {
      int res;
      thrd_join(th[j], &res);
    }

    GradAccum gL, gM;
    grad_reset(&gL);
    grad_reset(&gM);

    float capt = 0.f;
    float surv = 0.f;

    for (int j = 0; j < NB_THREADS; j++) {
      grad_add(&gL, &taches[j].gL);
      grad_add(&gM, &taches[j].gM);
      capt += taches[j].tot_capt;
      surv += taches[j].tot_survie;
    }

    baseline_maj(&blL, &gL, BETA_BASE);
    baseline_maj(&blM, &gM, BETA_BASE);

    reinforce_maj(&piL, &gL, alpha);
    reinforce_maj(&piM, &gM, alpha);

    if (gen % 50 == 0)
      printf("%d | %.3f | %.2f | %.1f\n", gen, alpha, capt / N_EPISODES,
             surv / N_EPISODES);
  }

  sauver("politique_loup.txt", &piL);
  sauver("politique_mouton.txt", &piM);

  return 0;
}