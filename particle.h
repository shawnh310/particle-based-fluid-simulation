#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "kernel.h"
#include "vector3d.h"

class Particle
{
public:
    Particle()
    {
        m = PM;
        g = PG;
    }
    
    Particle(Vector3Df r_, Vector3Df v_, Vector3Df a_) :
        r(r_),
        v(v_),
        a(a_)
    {
        m = PM;
        g = PG;
        p = 0;
        d = 0;
        V = 0;
        c_lap = 0;
    }
    
// public member data for easy access
public:
    float m;     // mass
    float g;     // viscosity
    float p;     // presure
    float d;     // density
    float V;     // 1/d
    float c_lap; // laplace color field
    Vector3Df r;  // position
    Vector3Df v;  // velocity
    Vector3Df a;  // acceleration
    Vector3Df n;  // normal
    int grid_index[3];
};

#endif
