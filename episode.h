#ifndef EPISODE_H
#define EPISODE_H

#include "jeu.h"
#include "politique.h"
#include "alea.h"

//loup
#define R_CATCH        10.0f  
#define R_STEP_LOUP    -0.05f  
#define K_APPROACH      0.5f   
#define K_REG_LOUP      0.15f  
#define R_FAIM_LOUP   -10.0f   

//mouton
#define R_SURVIE        0.1f                 
#define R_ATTRAPE     -10.0f   
#define R_HERBE         0.3f   
#define K_FUITE         0.4f   
#define K_REG_MOUTON    0.15f  
#define R_FAIM_MOUTON -10.0f   


typedef struct {
    int   captures;          
    float survie_moy;       
    int   loups_faim;          
    int   moutons_faim;                     
    float ener_loup;           
    float ener_mouton;     
} StatsEpisode;

StatsEpisode episode_run(Monde *M, const Politique *piL, const Politique *piM,
                            Trajectoire trajs[NB_TOTAL], int max_steps,
                            int apprend, Rng *rng);


int episode_un_pas(Monde *M, const Politique *piL, const Politique *piM, Rng *rng);

#endif
