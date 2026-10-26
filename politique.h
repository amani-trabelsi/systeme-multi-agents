#ifndef POLITIQUE_H
#define POLITIQUE_H

#include "jeu.h"
#include "alea.h"

void politique_init(Politique *pi);  
void politique_proba(const Politique *pi, const float phi[NFEAT], float P[NACT]);
int  politique_tirer(const float P[NACT], Rng *rng);

#endif
