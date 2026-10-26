#ifndef MONDE_H
#define MONDE_H

#include "jeu.h"
#include "alea.h"

int  dans(int x, int y);
int  libre(const Monde *M, int x, int y);
void monde_init(Monde *M, Rng *rng);
int  region_de(int x, int y);  
void region_centre(int r, int *cx, int *cy); 
void monde_repousse(Monde *M);
Perception perception(const Monde *M, int idx);
void perception_features(const Perception *p, float phi[NFEAT]);

#endif