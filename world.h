#ifndef __WORLD_H__
#define __WORLD_H__

#include "kernel.h"
#include "particle.h"
#include "vector3d.h"

#include <vector>
#include <set>


class World
{
public:
	World();
    
public:
    void InitGrid();
    void UpdateGrid();
    
    void UpdateFluidDensity();
    void UpdateFluidAcceleration();
	void UpdateFluidPosition();
    
	void Render() const;
    
    std::vector<Particle>& GetParticles() { return particles_; };
 
private:
    void InitParticles();
    
    Vector3Di GetParticleGrid(const Particle& p) const;
    
    void HandleParticleCollision();
    
    int GetNeighborParticles(int particle_index, /*out*/ std::vector<int>& neighbors);
    
private:
    std::vector<Particle> particles_;
    
    // index order: z, x, y
	std::set<int> grid_[ (int)(CUBE_Z/H) ][ (int)(CUBE_X/H) ][ (int)(CUBE_Y/H) ];
};










#endif