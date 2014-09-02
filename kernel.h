#ifndef __KERNEL_H__
#define __KERNEL_H__


// constant
#define H          0.04f     // grid side length
#define H2         0.0016f   // H^2
#define TIMESTEP   0.006f
#define NPARTICLES 800
#define PM         0.0118f     
#define INV_PM     84.57627f
#define PRAD       0.04f
#define PREST_D    1000.f
#define GAS_CONST  20.f
#define PG         10.f      //viscosity
#define GRAVITY    9.81f
#define PT         0.6f      //tension
#define PMIN_SUF_GRAD 10.f

// world cube size
#define  CUBE_X   0.2f
#define  CUBE_Y   0.2f
#define  CUBE_Z   0.6f

#define NUM_GRID_Z ((int)(CUBE_Z/H))
#define NUM_GRID_X ((int)(CUBE_X/H))
#define NUM_GRID_Y ((int)(CUBE_Y/H))

// prefiex "K" means Kernal
extern float KDensityConst;
extern float KPressureConst;
extern float KViscosityConst;
extern float KSurfaceTensionConst;
extern float CSelf;

// kernel functions
float KernelDensity(float distSq);
float KernelPressureGrad(float dist);
float KernelViscosityLaplacian(float dist);
void  KernelSurfaceTension(float distSq, /*out*/ float& grad, /*out*/ float &lap);
  
#endif
