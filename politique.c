#include "politique.h"
#include <math.h>


void politique_init(Politique *pi)
{
    for (int a=0;a<NACT;a++)
        for (int k=0;k<NFEAT;k++)
            pi->theta[a][k]=0.f;   /* logits nuls -> proba uniforme */
}

void politique_proba(const Politique *pi, const float phi[NFEAT], float P[NACT])
{
    float z[NACT];
    for (int a=0;a<NACT;a++){
        float s=0.f;
        for (int k=0;k<NFEAT;k++) s+=pi->theta[a][k]*phi[k];
        z[a]=s;
    }
    float zmax=z[0];
    for (int a=1;a<NACT;a++) if(z[a]>zmax) zmax=z[a];   /* centrage anti-overflow */

    float somme=0.f;
    for (int a=0;a<NACT;a++){ P[a]=expf(z[a]-zmax); somme+=P[a]; }
    for (int a=0;a<NACT;a++) P[a]/=somme;
}
int politique_tirer(const float P[NACT], Rng *rng)
{
    float r=rng_float(rng), cumul=0.f;
    for (int a=0;a<NACT;a++){ cumul+=P[a]; if(r<=cumul) return a; }
    return NACT-1;
}
