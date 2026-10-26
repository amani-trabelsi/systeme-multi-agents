#include "reinforce.h"
#include "monde.h"      /* perception_features : perception -> phi */
#include <math.h>
#include <string.h>


void grad_reset(GradAccum *g){
    for (int a=0;a<NACT;a++) 
        for(int k=0;k<NFEAT;k++) 
            g->D[a][k]=0.f;
    g->ntraj=0;
    memset(g->sumG, 0, sizeof g->sumG);
    memset(g->cntG, 0, sizeof g->cntG);
}
void grad_add(GradAccum *dst, const GradAccum *src){
    for (int a=0;a<NACT;a++) for(int k=0;k<NFEAT;k++) dst->D[a][k]+=src->D[a][k];
    dst->ntraj += src->ntraj;
    for (int t=0;t<TMAX;t++){ dst->sumG[t]+=src->sumG[t]; dst->cntG[t]+=src->cntG[t]; }
}

void baseline_init(Baseline *bl){
    for (int t=0;t<TMAX;t++) 
        bl->b[t]=0.f;
}
void baseline_maj(Baseline *bl, const GradAccum *g, float beta){
    for (int t=0;t<TMAX;t++){
        if (g->cntG[t]>0){
            float gmoy = (float)(g->sumG[t]/(double)g->cntG[t]);
            bl->b[t] += beta*(gmoy - bl->b[t]);
        }
    }
}

void reinforce_accumuler(const Politique *pi, const Trajectoire *tr,
                         GradAccum *g, float gamma, const Baseline *bl)
{
    int T=tr->T; float G=0.f;
    for (int u=0;u<T;u++){
        int t=T-1-u;
        const Transition *pas=&tr->pas[t];

        G  = pas->reward + gamma*G;            

        g->sumG[t] += G; g->cntG[t] += 1;

        float avantage = G - bl->b[t];        
        float GG = powf(gamma,(float)t)*avantage;

        float phi[NFEAT]; perception_features(&pas->perc, phi);  
        float P[NACT]; politique_proba(pi, phi, P);
        int at=pas->action;
        for (int a=0;a<NACT;a++)
            for (int k=0;k<NFEAT;k++){
                float phik=phi[k];
                float modif=(a==at?phik:0.f) - P[a]*phik;   
                g->D[a][k]+=GG*modif;
            }
    }
    g->ntraj += 1;
}
void reinforce_maj(Politique *pi, const GradAccum *g, float alpha)
{
    if (g->ntraj==0) return;
    float invN=1.f/(float)g->ntraj;
    for (int a=0;a<NACT;a++) for(int k=0;k<NFEAT;k++)
        pi->theta[a][k] += alpha * g->D[a][k] * invN;   /* signe + : montee */
}