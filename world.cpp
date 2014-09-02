#include "world.h"
#include <assert.h>
#include <GLUT/glut.h>
#include <stdlib.h>

using namespace std;

World::World()
{    
    InitParticles();
}

// init particles randomly
void World::InitParticles()
{
    for(int i = 0; i < NPARTICLES; i++)
    {
        Vector3Df r;
        r.x = (rand()%500) / 100000.0f * powf(-1,rand()%2);
        r.y = (rand()%500) / 100000.0f * powf(-1,rand()%2);
        r.z = 0.15 + ((rand()%30000) / 10000.0f) * powf(-1,rand()%2);
                            
        particles_.push_back( Particle(r, Vector3Df(0,0,0), Vector3Df(0,0,0)) );
    }
}

void World::Render() const
{
	float red[3]  = {0.7, 0,0};
	float blue[3] = {0,0, 0.7};
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(int i = 0; i < particles_.size(); i++)
    {
        const Particle& p = particles_[i];
        
        // surface particle
        if(p.n.Length() > PMIN_SUF_GRAD)
        {
            glMaterialfv(GL_FRONT,GL_SPECULAR, red);
            glMaterialfv(GL_FRONT,GL_DIFFUSE,  red);
            glMaterialfv(GL_FRONT,GL_AMBIENT,  red);
        }
        
        // internal particle
        else
        {
            glMaterialfv(GL_FRONT,GL_SPECULAR, blue);
            glMaterialfv(GL_FRONT,GL_DIFFUSE,  blue);
            glMaterialfv(GL_FRONT,GL_AMBIENT,  blue);
        }
        
        glPushMatrix();
        glTranslatef(p.r.x, p.r.y, p.r.z);
        glutSolidSphere(0.004, 5, 5);
        glPopMatrix();
    }
}


void World::InitGrid()
{
	for(int i = 0; i < particles_.size(); i++)
	{
        Particle& p = particles_[i];
        
        Vector3Di index = GetParticleGrid(p);
        
		p.grid_index[0] = index.z;
		p.grid_index[1] = index.x;
		p.grid_index[2] = index.y;
        
        grid_[index.z][index.x][index.y].insert(i);
	}
}

Vector3Di World::GetParticleGrid(const Particle& p) const
{
    int max_x = (int)(CUBE_X/H) - 1;
    int max_y = (int)(CUBE_Y/H) - 1;
    int max_z = (int)(CUBE_Z/H) - 1;
    
    int x = (p.r.x - (-CUBE_X/2.f) ) / H;
    if( x > max_x )
        x = max_x;
    else if(x < 0)
        x = 0;
    
    int y = (p.r.y - (-CUBE_Y/2.f) ) / H;
    if( y > max_y )
        y = max_y;
    else if( y < 0)
        y = 0;
    
    int z = ((CUBE_Z/2.f) - p.r.z) / H;
    if( z > max_z )
        z = max_z;
    else if(z < 0)
        z = 0;
    
    return Vector3Di(x,y,z);
}


void World::UpdateGrid()
{
	for(int i = 0; i < particles_.size(); i++)
	{
        Particle& p = particles_[i];
        
        Vector3Di index = GetParticleGrid(p);
        int x = index.x;
        int y = index.y;
        int z = index.z;
        
        int gz = p.grid_index[0];
        int gx = p.grid_index[1];
        int gy = p.grid_index[2];
        
		if( gz == z &&
            gx == x &&
            gy == y)
			continue;
        
        grid_[gz][gx][gy].erase(i);
        
		p.grid_index[0] = z;
		p.grid_index[1] = x;
		p.grid_index[2] = y;
        
        grid_[z][x][y].insert(i);
	}
}



int World::GetNeighborParticles(int particle_index, /*out*/ std::vector<int>& neighbors)
{    
	int num_neighbors = 0;
   
    assert(particle_index >= 0 && particle_index < particles_.size());
	Particle& p = particles_[particle_index];
    
	int gz = p.grid_index[0];
	int gx = p.grid_index[1];
	int gy = p.grid_index[2];
    
	for(int z_offset = -1; z_offset <= 1; z_offset++)
	{
		if( gz + z_offset < 0 || gz + z_offset > (NUM_GRID_Z-1) )
			continue;
        
		for(int x_offset = -1; x_offset <= 1; x_offset++)
		{
			if( gx + x_offset < 0 || gx + x_offset > (NUM_GRID_X-1) )
				continue;
            
			for(int y_offset = -1; y_offset <= 1; y_offset++)
			{
				if( gy + y_offset < 0 || gy + y_offset > (NUM_GRID_Y-1) )
					continue;
                
				set<int>& particles_in_grid = grid_[gz + z_offset][gx + x_offset][gy + y_offset];
                
				for( set<int>::iterator it = particles_in_grid.begin();
					it != particles_in_grid.end(); ++it)
				{
					int neighbor_particle_index = *it;
                    
                    neighbors.push_back(neighbor_particle_index);
                    
                    num_neighbors++;
				}
			}
		}
	}
    
	return num_neighbors;
}


// update the density and pressure of each particle
void World::UpdateFluidDensity()
{
    vector<int> neighbors;
	for(int i = 0; i < particles_.size(); i++)
	{
		Particle& p = particles_[i];
        
		p.d = 0;
        
        neighbors.resize(0);
		int num_neighbors = GetNeighborParticles(i, neighbors);
		for( int j = 0; j < num_neighbors; j++ )
		{
			int neighbor_index = neighbors[j];
            
			Particle& neighbor_particle = particles_[neighbor_index];
            
			p.d += KernelDensity( p.r.Distance2(neighbor_particle.r) );
		}
        
		p.d *= PM;
		p.V = 1.f / p.d;
		p.p = GAS_CONST * (p.d - PREST_D);
        
		if( p.p < 0 )
			p.p = 0;
        
		p.a.Clear();
		p.n.Clear();
		p.c_lap = 0;
	}
}


void World::UpdateFluidAcceleration()
{
	Vector3Df fpressure;
	Vector3Df fviscosity;
	Vector3Df dr;
	Vector3Df dv;
    vector<int> neighbors;
	for(int i=0; i < particles_.size(); i++ )
	{
		fpressure.Clear();
		fviscosity.Clear();
        
		Particle& p = particles_[i];
        
		p.c_lap += CSelf * p.V;
        
        neighbors.resize(0);
		int num_neighbors = GetNeighborParticles(i, neighbors);
        
		for(int j = 0; j < num_neighbors; j++)
		{
			int neighbor_index = neighbors[j];
            
			Particle& neighbor_particle = particles_[neighbor_index];
            
			float pjV = neighbor_particle.V;
            
			// pressure
			dr = p.r - neighbor_particle.r; // from to neighbor to self
			float grad = (p.p + neighbor_particle.p) / 2 * KernelPressureGrad(dr.Length());
			fpressure -= dr * grad * pjV;
            
			// viscosity
			dv = neighbor_particle.v - p.v; // from self to neighbor
			fviscosity += dv * pjV * KernelViscosityLaplacian(dr.Length());
            
			// tension
			float c_lap, c_grad_fact;
			KernelSurfaceTension(dr.Length2(), c_grad_fact, c_lap);
			p.n     += dr * (c_grad_fact * pjV);
			p.c_lap += c_lap * pjV;
		}
        
		fviscosity *= PG * PM;		
		fpressure *= PM;
        
		p.a = (fpressure + fviscosity) * p.V;
		p.n *= PM;
		p.c_lap *= PM;
        
	}
    
	for(int i = 0; i < particles_.size(); i++ )
	{
		Particle& p = particles_[i];
        
		float nl = p.n.Length();
        
		if ( nl > PMIN_SUF_GRAD ) // only if gradient is sufficiently big
		{
			Vector3Df fsur = p.n * (PT * p.c_lap / nl);
            
			p.a -= fsur * p.V;
		}
        
		p.a.z -= GRAVITY;
	}
}


void World::HandleParticleCollision()
{
	for(int i = 0; i < particles_.size(); i++)
	{
        Particle& p = particles_[i];
        
        if (p.r.x > (CUBE_X/2.f)*0.9)
        {
            p.r.x = (CUBE_X/2.f)*0.9;
            p.v.x *= -1;
            p.v *= 0.94f;
        }
        else if (p.r.x < (-CUBE_X/2.f)*0.9)
        {
            p.r.x = (-CUBE_X/2.f)*0.9;
            p.v.x *= -1;
            p.v *= 0.94f;
        }
        
        if (p.r.y > (CUBE_Y/2.f)*0.9)
        {
            p.r.y = (CUBE_Y/2.f)*0.9;
            p.v.y *= -1;
            p.v *= 0.94f;
        }
        else if (p.r.y < (-CUBE_Y/2.f)*0.9)
        {
            p.r.y = (-CUBE_Y/2.f)*0.9;
            p.v.y *= -1;
            p.v *= 0.94f;
        }
        
        if (p.r.z < (-CUBE_Z/2.f)*0.9)
        {
            p.r.z = (-CUBE_Z/2.f)*0.9;
            p.v.z *= -1;
            p.v *= 0.94f;
        }
        else if (p.r.z > (CUBE_Z/2.f)*0.9)
        {
            p.r.z = (CUBE_Z/2.f)*0.9;
            p.v.z *= -1;
            p.v *= 0.94f;
        }
    }	
}


void World::UpdateFluidPosition()
{
    for(int i = 0; i < particles_.size(); i++)
	{
        particles_[i].v += particles_[i].a * TIMESTEP;
        particles_[i].r += particles_[i].v * TIMESTEP;
    }
    
    HandleParticleCollision();
}










