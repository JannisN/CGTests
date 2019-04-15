#include <iostream>
#include <random>

#include "Utils.h"
#include "Matrix.h"

#include "GLFW/glfw3.h"

using namespace cg;

int main()
{
    std::uniform_real_distribution<double> random(0.0, 1.0);
    std::default_random_engine generator;
    
    Matrix<double, 8, 8> grid;
    
    for (int i = 0; i < std::get<1>(grid.getSize()); i++)
    {
        for (int j = 0; j < std::get<0>(grid.getSize()); j++)
        {
            grid(i, j) = random(generator);
        }
    }
    
    GLFWwindow* window;
    glfwInit();
    window = glfwCreateWindow(640, 480, "Simulation", 0, 0);
    glfwMakeContextCurrent(window);
    
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glLineWidth(3);
        
        glLoadIdentity();
        glFro
        
        glPushMatrix();
        glScaled(0.1, 0.1, 0.1);
        glBegin(GL_LINES);
        for (int i = 0; i < std::get<1>(grid.getSize()) - 1; i++)
        {
            for (int j = 0; j < std::get<0>(grid.getSize()) - 1; j++)
            {
                glVertex3d(i, j, grid(i, j));
                glVertex3d(i + 1, j, grid(i + 1, j));
                glVertex3d(i, j, grid(i, j));
                glVertex3d(i, j + 1, grid(i, j + 1));
            }
        }
        glEnd();
        glPopMatrix();
        
        std::cout << "asdf";
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;
}
