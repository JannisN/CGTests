#include <iostream>
#include <random>

#include "Utils.h"
#include "Matrix.h"

#include "GLFW/glfw3.h"

using namespace cg;

double zoom = 0;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	zoom += yoffset;
}

int main()
{
    std::uniform_real_distribution<double> random(0.0, 1.0);
    std::default_random_engine generator;
    
    Matrix<double, 16, 16> grid;
    
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
	glfwSetScrollCallback(window, scroll_callback);

	double mouseX = 0;
	double mouseY = 0;
	double mousePosX = 0;
	double mousePosY = 0;
	glfwGetCursorPos(window, &mousePosX, &mousePosY);

    while (!glfwWindowShouldClose(window))
    {
		glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glLineWidth(1);

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		glViewport(0, 0, width, height);
		double fract = static_cast<double>(width) / static_cast<double>(height);

		glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
		//glOrtho(-1.0, 1.0, -1.0, 1.0, -1.5, 1.5);
		glFrustum(-0.01 * fract, 0.01 * fract, -0.01, 0.01, 0.01, 100000.0);
        
		//std::cout << glGetError() << std::endl;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

        glPushMatrix();

		glTranslated(0, 0, -1000 + zoom * 100);
		
		double mousePosXOld = mousePosX;
		double mousePosYOld = mousePosY;
		glfwGetCursorPos(window, &mousePosX, &mousePosY);

		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if (state == GLFW_PRESS)
		{
			mouseX += mousePosX - mousePosXOld;
			mouseY += mousePosY - mousePosYOld;
		}

		glRotated(mouseY * 0.5, 1, 0, 0);
		glRotated(mouseX * 0.5, 0, 0, 1);

		double gridOffsetX = (std::get<1>(grid.getSize()) - 1) / 2.0 * 100;
		double gridOffsetY = (std::get<0>(grid.getSize()) - 1) / 2.0 * 100;

        glBegin(GL_LINES);
		glColor3d(100 / 255.0, 149 / 255.0, 237 / 255.0);
        for (int i = 0; i < std::get<1>(grid.getSize()) - 1; i++)
        {
            for (int j = 0; j < std::get<0>(grid.getSize()) - 1; j++)
            {
                glVertex3d(i * 100 - gridOffsetX, j * 100 - gridOffsetY, -grid(i, j) * 100);
                glVertex3d((i + 1) * 100 - gridOffsetX, j * 100 - gridOffsetY, -grid(i + 1, j) * 100);
                glVertex3d(i * 100 - gridOffsetX, j * 100 - gridOffsetY, -grid(i, j) * 100);
                glVertex3d(i * 100 - gridOffsetX, (j + 1) * 100 - gridOffsetY, -grid(i, j + 1) * 100);
            }
        }
		for (int i = 0; i < std::get<1>(grid.getSize()) - 1; i++)
		{
			glVertex3d(i * 100 - gridOffsetX, (std::get<0>(grid.getSize()) - 1) * 100 - gridOffsetY, -grid(i, (std::get<0>(grid.getSize()) - 1)) * 100);
			glVertex3d((i + 1) * 100 - gridOffsetX,( std::get<0>(grid.getSize()) - 1) * 100 - gridOffsetY, -grid(i + 1, (std::get<0>(grid.getSize()) - 1)) * 100);
		}
		for (int i = 0; i < std::get<0>(grid.getSize()) - 1; i++)
		{
			glVertex3d((std::get<1>(grid.getSize()) - 1) * 100 - gridOffsetX, i * 100 - gridOffsetY, -grid((std::get<1>(grid.getSize()) - 1), i) * 100);
			glVertex3d((std::get<1>(grid.getSize()) - 1) * 100 - gridOffsetX, (i + 1) * 100 - gridOffsetY, -grid((std::get<1>(grid.getSize()) - 1), i + 1) * 100);

		}
        glEnd();
		glPopMatrix();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;
}
