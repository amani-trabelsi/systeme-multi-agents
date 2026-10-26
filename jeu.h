#ifndef JEU_H
#define JEU_H

#define L 20
#define H 20
#define TILE 32         

#define NB_LOUP 12
#define NB_MOUTON  12
#define NB_TOTAL (NB_LOUP + NB_MOUTON)

enum { NORD, EST, SUD, OUEST, NACT };

#define REG_COLS 2
#define REG_ROWS 2
#define NB_REGIONS (REG_COLS*REG_ROWS)

#define HERBE_REPOUSSE 25   
#define VISION_HERBE 4   
#define CAPTURE_MIN_LOUPS 2  

#define NB_PRAIRIES 3  
#define RAYON_PRAIRIE 3   
#define ARIDE (-1)  

#define ENERGIE_INIT 20.0f
#define ENERGIE_MAX 40.0f  
#define COUT_METABOLIQUE 0.20f  
#define GAIN_HERBE_E 4.0f  
#define GAIN_CHASSE_E 8.0f  

typedef struct {
    float dloup;       
    int   sx_loup;       
    int   sy_loup;
    int   herbe;        
    int   hx_herbe;      
    int   hy_herbe;
    int   rx_region;     
    int   ry_region;
} Perception;

#define NFEAT  9
#define TMAX   200
typedef struct {
    int   vivant;
    int   x, y;              
    int   loup;             
    float energie;          
    float carac[NACT];       
    int   regard;            
    int   bouge;             
} Agent;

typedef struct {
    int   grille[H][L];     
    int   herbe [H][L];     
    Agent ag[NB_TOTAL];
    int   score;
} Monde;

typedef struct {
    Perception perc;       
    int        action;
    float      reward;
} Transition;

typedef struct {
    Transition pas[TMAX];
    int        T;
} Trajectoire;

typedef struct {
    float theta[NACT][NFEAT];  
} Politique;

#endif
