#ifndef ALEA_H
#define ALEA_H

#include <math.h>


typedef unsigned int Rng; 
static inline unsigned int rng_u32(Rng *x){
    *x = 1664525u * (*x) + 1013904223u;   //(a*x + b) mod 2^32 
    return *x;
}


// reel uniforme dans [0, 1[ 
static inline float rng_float(Rng *x){
    return rng_u32(x) / 4294967296.0f;   
}
// entier uniforme dans [0, n[ 
static inline int rng_int(Rng *x, int n){
    return (int)(rng_float(x) * n);
}


static inline float rng_gauss(Rng *x){
    float u1 = rng_float(x);
    float u2 = rng_float(x);
    if (u1 < 1e-12f) u1 = 1e-12f;          //evite ln(0) 
    return sqrtf(-2.f*logf(u1)) * cosf(6.2831853f*u2);

}

// normale de moyenne mu et d'ecart-type sigma 
static inline float rng_normal(Rng *x, float mu, float sigma)
{
    return mu + sigma*rng_gauss(x);
}















#endif