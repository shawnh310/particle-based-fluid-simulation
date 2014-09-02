#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <time.h>
#include <stdlib.h>
#include <vector>

#include <GLFW/glfw3.h>

#include "camera.h"
#include "kernel.h"
#include "particle.h"
#include "world.h"

using namespace std;

void InitOpenGL()
{
	glClearColor(1,1,1,1);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,1, 0.0001, 100);
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    
    float ambient[3]={0.4,0.2,0.4};
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
    
    float diffuse[3]={0.3,0.3,0.7};
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
    
    float specular[3]={1,1,1};
    glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
    
    float light_pos[3]={0.1,0,-1};
    glLightfv(GL_LIGHT0,GL_POSITION,light_pos);
    
    float light1_pos[3]={-0.1,0,-1};
    glLightfv(GL_LIGHT1,GL_POSITION,light1_pos);
    
    float light2_pos[3]={0,0.1,-1};
    glLightfv(GL_LIGHT1,GL_POSITION,light2_pos);
    
    float shine=50;
    glMaterialfv(GL_FRONT,GL_SHININESS,&shine);
}

void ApplyExternalForce(World& world,
                        float xforce,
                        float yforce)
{
    vector<Particle>& particles = world.GetParticles();
    
    for(int i = 0; i < particles.size(); i++)
    {
        Particle& p = particles[i];
        
        if(xforce > 0)
        {
            if(p.r.x < -0.07)
                p.a.x += xforce;
        }
        else
        {
            if(p.r.x > 0.07)
                p.a.x += xforce;
        }
        
        if(yforce > 0)
        {
            if(p.r.z > 0.25)
                p.a.z += -yforce;
        }
        else
        {
            if(p.r.z < -0.25)
                p.a.z += -yforce;
        }
    }
}

int main(void)
{
    GLFWwindow* window;
    
    if (!glfwInit())
        return -1;
    
    window = glfwCreateWindow(640, 480, "particle based fluid simulation", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    InitOpenGL();
    
    Camera camera;
    camera.Init(3.14159f/2.f, 0, 0.6);    
    
    int mouse_lx = 0;
    int mouse_ly = 0;
    
    int mouse_rx = 0;
    int mouse_ry = 0;
    
    srand((unsigned int)time(NULL));
    
    World world;
    world.InitGrid();
    
    bool exforce = false;
    float xforce = 0;
    float yforce = 0;
    
    double last_update = glfwGetTime();
    
    while (!glfwWindowShouldClose(window))
    {
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        
        // holding mouse left button + mouse move to change camera angles
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            int dx = mouse_lx - (int)mx;
            int dy = mouse_ly - (int)my;
            
            mouse_lx = (int)mx;
            mouse_ly = (int)my;
            
            camera.UpdateAngles( dy/100.0f, dx/100.0f);
        }
        
        // holding mouse right button + mouse move to apply external force
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            int dx = (int)mx - mouse_rx;
            int dy = (int)my - mouse_ry;
            
            mouse_rx = (int)mx;
            mouse_ry = (int)my;
            
            exforce = true;
            xforce = dx * 3.0f;
            yforce = dy * 3.0f;
        }
        
        mouse_lx = (int)mx;
        mouse_ly = (int)my;
        
        mouse_rx = (int)mx;
        mouse_ry = (int)my;
        
        world.UpdateFluidDensity();
        world.UpdateFluidAcceleration();
        
        // apply external force to particles applied by user
        if(exforce == true)
        {
            exforce = false;

            ApplyExternalForce(world, xforce, yforce);
            
            xforce = 0;
            yforce = 0;
        }
        
        world.UpdateFluidPosition();
        world.UpdateGrid();
        
        // lock at 30 fps
        {
            double cur = glfwGetTime();
            while (cur - last_update < 0.033)
            {
                cur = glfwGetTime();
            }
            last_update = cur;
        }
        
        world.Render();
        
        glfwSwapBuffers(window);
        
        // Poll for and process events
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}