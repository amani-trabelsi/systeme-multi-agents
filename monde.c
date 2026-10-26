#include "monde.h"
#include <math.h>

int dans(int x, int y){ 
    return x>=0 && y>=0 && x<L && y<H; 
}


static int occupee(const Monde *M, int x, int y){
    for (int i=0;i<NB_TOTAL;i++)
        if (M->ag[i].vivant && M->ag[i].x==x && M->ag[i].y==y) return 1;
    return 0;
}

int libre(const Monde *M, int x, int y){
    return dans(x,y) && M->grille[y][x]==0 && !occupee(M,x,y);
}
int region_de(int x, int y){
    int rx = x*REG_COLS/L;
    if (rx>=REG_COLS) 
        rx=REG_COLS-1;
    int ry = y*REG_ROWS/H; 
    if (ry>=REG_ROWS) 
        ry=REG_ROWS-1;
    return ry*REG_COLS + rx;
}
void region_centre(int r, int *cx, int *cy){
    int rx = r%REG_COLS, ry = r/REG_COLS;
    *cx = (int)((rx+0.5f)*L/REG_COLS);
    *cy = (int)((ry+0.5f)*H/REG_ROWS);
}


void monde_init(Monde *M, Rng *rng){
    /* tirage des centres de prairies */
    int pcx[NB_PRAIRIES], pcy[NB_PRAIRIES];
    for (int k=0;k<NB_PRAIRIES;k++){ pcx[k]=rng_int(rng,L); pcy[k]=rng_int(rng,H); }

    for (int y=0;y<H;y++)
        for (int x=0;x<L;x++){
            M->grille[y][x]=0;

            /* la case est-elle dans le rayon d'une prairie ? */
            int dans_prairie=0;
            for (int k=0;k<NB_PRAIRIES;k++){
                int dx=x-pcx[k], dy=y-pcy[k];
                if (dx*dx+dy*dy <= RAYON_PRAIRIE*RAYON_PRAIRIE){ dans_prairie=1; break; }
            }

            if (!dans_prairie){
                M->herbe[y][x]=ARIDE;                 /* aride : jamais d'herbe */
            } else if (rng_int(rng,100)<55){
                M->herbe[y][x]=0;                     /* herbe dispo */
            } else {
                M->herbe[y][x]=1+rng_int(rng,HERBE_REPOUSSE);  /* repousse en cours */
            }
        }

    for (int k=0;k<8;k++)
        M->grille[rng_int(rng,H)][rng_int(rng,L)]=1;

    for (int i=0;i<NB_TOTAL;i++){
        Agent *a=&M->ag[i];
        a->vivant=1;
        a->loup=(i<NB_LOUP);
        a->x=a->y=-1;
        a->energie=ENERGIE_INIT;       /* energie de depart commune */
        a->regard=a->bouge=NORD;
        for (int u=0;u<NACT;u++) a->carac[u]=1.f/NACT;
    }

    for (int i=0;i<NB_TOTAL;i++){
        int x,y;
        do { x=rng_int(rng,L); y=rng_int(rng,H); } while(!libre(M,x,y));
        M->ag[i].x=x;
        M->ag[i].y=y;
    }

    M->score=0;
}

void monde_repousse(Monde *M)
{
    for (int y=0;y<H;y++)
        for (int x=0;x<L;x++)
            if (M->herbe[y][x]>0) M->herbe[y][x]--;
}


static int signe(int v){ return (v>0)-(v<0); }   /* -1, 0, +1 */
static int meilleure_region(const Monde *M, int idx)
{
    int loup = M->ag[idx].loup;
    int compte[NB_REGIONS] = {0};

    if (loup){
        /* compter les moutons vivants par region */
        for (int i=NB_LOUP;i<NB_TOTAL;i++)
            if (M->ag[i].vivant)
                compte[region_de(M->ag[i].x, M->ag[i].y)]++;
    } else {
        /* compter l'herbe disponible par region */
        for (int y=0;y<H;y++)
            for (int x=0;x<L;x++)
                if (M->grille[y][x]==0 && M->herbe[y][x]==0)
                    compte[region_de(x,y)]++;
    }

    int best=-1, bestv=0;
    for (int r=0;r<NB_REGIONS;r++)
        if (compte[r]>bestv){ bestv=compte[r]; best=r; }
    return best;
}

Perception perception(const Monde *M, int idx)
{
    Perception p = {1e9f,0,0,0,0,0,0,0};
    int loup=M->ag[idx].loup, x=M->ag[idx].x, y=M->ag[idx].y;

    float best=1e18f; int found=-1;
    for (int i=0;i<NB_TOTAL;i++){
        if (i==idx || !M->ag[i].vivant || M->ag[i].loup==loup) continue;
        float dx=M->ag[i].x-x, dy=M->ag[i].y-y, d=dx*dx+dy*dy;
        if (d<best){ best=d; found=i; }
    }
    if (found>=0){
        int dx=M->ag[found].x-x, dy=M->ag[found].y-y;
        p.dloup   = sqrtf((float)(dx*dx+dy*dy));
        p.sx_loup = signe(dx);
        p.sy_loup = signe(dy);
    }

    if (!loup){
        int bd=1<<30, gx=0, gy=0, ok=0;
        for (int ddy=-VISION_HERBE; ddy<=VISION_HERBE; ddy++)
            for (int ddx=-VISION_HERBE; ddx<=VISION_HERBE; ddx++){
                int nx=x+ddx, ny=y+ddy;
                if (!dans(nx,ny) || M->grille[ny][nx]!=0 || M->herbe[ny][nx]!=0) continue;
                int d=ddx*ddx+ddy*ddy;
                if (d<bd){ bd=d; gx=nx; gy=ny; ok=1; }
            }
        if (ok){ p.herbe=1; p.hx_herbe=signe(gx-x); p.hy_herbe=signe(gy-y); }
    }

    int r = meilleure_region(M, idx);
    if (r>=0){
        int cx, cy; region_centre(r, &cx, &cy);
        p.rx_region = signe(cx - x);
        p.ry_region = signe(cy - y);
    }
    return p;
}


void perception_features(const Perception *p, float phi[NFEAT])
{
    phi[0]=(float)p->sx_loup;
    phi[1]=(float)p->sy_loup;
    phi[2]=1.f/(p->dloup+1.f);      
    phi[3]=(float)p->hx_herbe;
    phi[4]=(float)p->hy_herbe;
    phi[5]=(float)p->herbe;
    phi[6]=(float)p->rx_region;      
    phi[7]=(float)p->ry_region;      
    phi[8]=1.f;                     
}
