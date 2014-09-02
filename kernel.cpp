#include "kernel.h"
#include <cmath>

float KDensityConst        = 315.0f / (64.0f * M_PI * H*H*H*H*H*H*H*H*H);
float KPressureConst       = -45.f  / (M_PI*H*H*H*H*H*H);
float KViscosityConst      =  45.f  / (M_PI*H*H*H*H*H*H);
float KSurfaceTensionConst = - 945.f / (32.f * M_PI * pow(H,9));
float CSelf                = -945.f * 3.f / (32.f*M_PI*pow(H,5));

float KernelDensity(float distSq)
{
	if(distSq > H2)
        return 0;
    
    float f = H2 - distSq;
    
	return KDensityConst * f * f * f;
}

float KernelPressureGrad(float dist)
{
	if (dist > H)
		return 0;
    
	if(dist!=0)
	    return KPressureConst / dist * (H-dist)*(H-dist);
    else
		return KPressureConst / 100000.0f * (H-dist)*(H-dist);
}

float KernelViscosityLaplacian(float dist)
{
    if (dist > H)
        return 0;
    
	return KViscosityConst * (H - dist);
}

void KernelSurfaceTension(float distSq, /*out*/ float& grad, /*out*/ float &lap)
{
	if(distSq > H2)
    {
        grad = 0;
        lap  = 0;
        return ;
	}
    
	float r2mh2 = distSq - H2;
	float tmp = KSurfaceTensionConst * r2mh2;
	grad = tmp * r2mh2;
	
	// calculate laplacian
	lap = tmp * (7*distSq - 3*H2);
}
