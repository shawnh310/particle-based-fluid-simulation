#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "vector3d.h"

// simple camera in spherical coordinate
// always looking at origin
class Camera
{
public:
    Camera();
    
public:
	void Init(float theta, float phi, float r);
    
	void UpdateAngles(float dtheta, float dphi);
    
private:
    Vector3Df SphericalToXYZ();
    
private:
	float theta_; // angle from Z axis to Y axis in radian
	float phi_;   // angle around Z axis in radian
	float r_;     // distance to origin
};
	
#endif