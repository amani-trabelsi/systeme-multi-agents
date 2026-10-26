#include "episode.h"
#include "monde.h"
#include <stdlib.h>

static const int DX[NACT] = {0, 1, 0, -1}, DY[NACT] = {-1, 0, 1, 0};

static int manhattan(const Monde *M, int i, int j) {
  return abs(M->ag[i].x - M->ag[j].x) + abs(M->ag[i].y - M->ag[j].y);
}
static void agir(Monde *M, int idx, int act) {
  int nx = M->ag[idx].x + DX[act];
  int ny = M->ag[idx].y + DY[act];
  if (libre(M, nx, ny)) {
    M->ag[idx].x = nx;
    M->ag[idx].y = ny;
  }
}
static int loups_adjacents(const Monde *M, int s) {
  int nb = 0;
  for (int w = 0; w < NB_LOUP; w++)
    if (M->ag[w].vivant && manhattan(M, w, s) <= 1)
      nb++;
  return nb;
}
static int dir_perc(const Perception *p) {
  if (p->sx_loup > 0)
    return EST;
  if (p->sx_loup < 0)
    return OUEST;
  if (p->sy_loup > 0)
    return SUD;
  if (p->sy_loup < 0)
    return NORD;
  return NORD;
}
static int maj_energie(Agent *a, float delta) {
  a->energie += delta;
  if (a->energie > ENERGIE_MAX)
    a->energie = ENERGIE_MAX;
  return (a->energie <= 0.f);
}

StatsEpisode episode_run(Monde *M, const Politique *piL, const Politique *piM,
                         Trajectoire trajs[NB_TOTAL], int max_steps,
                         int apprend, Rng *rng) {
  StatsEpisode st = {0, 0.f, 0, 0, 0.f, 0.f};
  int act[NB_TOTAL], vie[NB_TOTAL];
  Perception perc_c[NB_TOTAL];
  float d_avant[NB_TOTAL];

  for (int i = 0; i < NB_TOTAL; i++) {
    trajs[i].T = 0;
    vie[i] = 0;
  }

  for (int step = 0; step < max_steps; step++) {
    int moutons = 0;
    for (int i = NB_LOUP; i < NB_TOTAL; i++)
      if (M->ag[i].vivant)
        moutons++;
    if (moutons == 0)
      break;

    monde_repousse(M);

    for (int i = 0; i < NB_TOTAL; i++) {
      if (!M->ag[i].vivant)
        continue;
      perc_c[i] = perception(M, i);
      float phi[NFEAT];
      perception_features(&perc_c[i], phi);
      float P[NACT];
      politique_proba(M->ag[i].loup ? piL : piM, phi, P);
      act[i] = politique_tirer(P, rng);
      for (int u = 0; u < NACT; u++)
        M->ag[i].carac[u] = P[u];
      M->ag[i].regard = dir_perc(&perc_c[i]);
      M->ag[i].bouge = act[i];
      d_avant[i] = perc_c[i].dloup;
    }

    for (int i = 0; i < NB_LOUP; i++)
      if (M->ag[i].vivant)
        agir(M, i, act[i]);
    for (int i = NB_LOUP; i < NB_TOTAL; i++)
      if (M->ag[i].vivant)
        agir(M, i, act[i]);

    int pris[NB_TOTAL];
    for (int i = 0; i < NB_TOTAL; i++)
      pris[i] = 0;
    for (int s = NB_LOUP; s < NB_TOTAL; s++)
      if (M->ag[s].vivant && loups_adjacents(M, s) >= CAPTURE_MIN_LOUPS)
        pris[s] = 1;

    for (int i = 0; i < NB_TOTAL; i++) {
      if (!M->ag[i].vivant)
        continue;
      int x = M->ag[i].x;
      int y = M->ag[i].y;
      float r;
      int mort = 0;

      int faim = maj_energie(&M->ag[i], -COUT_METABOLIQUE);

      if (M->ag[i].loup) {
        r = R_STEP_LOUP;

        float d_ap = perception(M, i).dloup;
        if (d_avant[i] < 1e8f && d_ap < 1e8f)
          r += K_APPROACH * (d_avant[i] - d_ap);

        if (perc_c[i].rx_region == DX[act[i]] && perc_c[i].ry_region == 0)
          r += K_REG_LOUP;
        if (perc_c[i].ry_region == DY[act[i]] && perc_c[i].rx_region == 0)
          r += K_REG_LOUP;

        for (int s = NB_LOUP; s < NB_TOTAL; s++)
          if (pris[s] && manhattan(M, i, s) <= 1) {
            r += R_CATCH;
            maj_energie(&M->ag[i], GAIN_CHASSE_E);
            break;
          }

        if (faim) {
          r += R_FAIM_LOUP;
          mort = 1;
          st.loups_faim++;
        }

      } else {
        if (pris[i]) {
          r = R_ATTRAPE;
          mort = 1;
        } else {
          r = R_SURVIE;

          float d_ap = perception(M, i).dloup;
          if (d_avant[i] < 1e8f && d_ap < 1e8f)
            r += K_FUITE * (d_ap - d_avant[i]);

          if (perc_c[i].rx_region == DX[act[i]] && perc_c[i].ry_region == 0)
            r += K_REG_MOUTON;
          if (perc_c[i].ry_region == DY[act[i]] && perc_c[i].rx_region == 0)
            r += K_REG_MOUTON;

          if (M->grille[y][x] == 0 && M->herbe[y][x] == 0) {
            r += R_HERBE;
            maj_energie(&M->ag[i], GAIN_HERBE_E);
            M->herbe[y][x] = HERBE_REPOUSSE;
          }

          if (faim) {
            r += R_FAIM_MOUTON;
            mort = 1;
            st.moutons_faim++;
          }
        }
      }

      if (apprend && trajs[i].T < TMAX) {
        int t = trajs[i].T;
        trajs[i].pas[t].perc = perc_c[i];
        trajs[i].pas[t].action = act[i];
        trajs[i].pas[t].reward = r;
        trajs[i].T++;
      }

      vie[i]++;
      if (mort)
        M->ag[i].vivant = 0;
    }

    for (int s = NB_LOUP; s < NB_TOTAL; s++)
      if (pris[s])
        st.captures++;
  }

  int somme = 0;
  for (int i = NB_LOUP; i < NB_TOTAL; i++)
    somme += vie[i];
  st.survie_moy = (float)somme / (float)NB_MOUTON;

  float el = 0.f, em = 0.f;
  for (int i = 0; i < NB_LOUP; i++)
    el += M->ag[i].energie;
  for (int i = NB_LOUP; i < NB_TOTAL; i++)
    em += M->ag[i].energie;
  st.ener_loup = el / (float)NB_LOUP;
  st.ener_mouton = em / (float)NB_MOUTON;
  return st;
}

int episode_un_pas(Monde *M, const Politique *piL, const Politique *piM,
                   Rng *rng) {
  monde_repousse(M);

  for (int i = 0; i < NB_TOTAL; i++) {
    if (!M->ag[i].vivant)
      continue;

    Perception p = perception(M, i);
    float phi[NFEAT], P[NACT];

    perception_features(&p, phi);
    politique_proba(M->ag[i].loup ? piL : piM, phi, P);

    int a = politique_tirer(P, rng);

    for (int u = 0; u < NACT; u++)
      M->ag[i].carac[u] = P[u];

    M->ag[i].regard = dir_perc(&p);
    M->ag[i].bouge = a;
  }

  for (int i = 0; i < NB_LOUP; i++)
    if (M->ag[i].vivant)
      agir(M, i, M->ag[i].bouge);
  for (int i = NB_LOUP; i < NB_TOTAL; i++)
    if (M->ag[i].vivant)
      agir(M, i, M->ag[i].bouge);

  for (int i = 0; i < NB_TOTAL; i++)
    if (M->ag[i].vivant)
      maj_energie(&M->ag[i], -COUT_METABOLIQUE);

  for (int s = NB_LOUP; s < NB_TOTAL; s++)
    if (M->ag[s].vivant && loups_adjacents(M, s) >= CAPTURE_MIN_LOUPS) {
      M->ag[s].vivant = 0;
      M->score++;
      for (int w = 0; w < NB_LOUP; w++)
        if (M->ag[w].vivant && manhattan(M, w, s) <= 1)
          maj_energie(&M->ag[w], GAIN_CHASSE_E);
    }

  for (int i = NB_LOUP; i < NB_TOTAL; i++) {
    if (!M->ag[i].vivant)
      continue;
    int x = M->ag[i].x, y = M->ag[i].y;
    if (M->grille[y][x] == 0 && M->herbe[y][x] == 0) {
      maj_energie(&M->ag[i], GAIN_HERBE_E);
      M->herbe[y][x] = HERBE_REPOUSSE;
    }
  }

  for (int i = 0; i < NB_TOTAL; i++)
    if (M->ag[i].vivant && M->ag[i].energie <= 0.f)
      M->ag[i].vivant = 0;

  int v = 0;
  for (int i = NB_LOUP; i < NB_TOTAL; i++)
    if (M->ag[i].vivant)
      v++;
  return v;
}
