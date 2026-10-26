#ifndef REINFORCE_H
#define REINFORCE_H

#include "jeu.h"
#include "politique.h"

typedef struct {
    float  D[NACT][NFEAT];
    int    ntraj;
    double sumG[TMAX];  
    long   cntG[TMAX];   
} GradAccum;

typedef struct { float b[TMAX]; } Baseline;
void baseline_init(Baseline *bl);  
void baseline_maj (Baseline *bl, const GradAccum *g, float beta);
void grad_reset(GradAccum *g);
void grad_add(GradAccum *dst, const GradAccum *src); 
void reinforce_accumuler(const Politique *pi, const Trajectoire *tr,
                         GradAccum *g, float gamma, const Baseline *bl);
void reinforce_maj(Politique *pi, const GradAccum *g, float alpha);

#endif