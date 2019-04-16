#include <iostream>
#include <random>

#include "Utils.h"
#include "Matrix.h"

#include "GLFW/glfw3.h"

using namespace cg;

template <unsigned int X, unsigned int Y>
Matrix<double, X, Y> laplaceFinitDifference()
{
	Matrix<double, X, Y> grid;

	// randwerte
	for (int i = 0; i < X; i++)
	{
		grid(i, Y - 1) = (std::pow(((X - 1) / 2.0 - i) / (X - 1) * 4, 2) - 4) * 0.1;
		std::cout << grid(i, Y - 1) << std::endl;
	}

	//double dx = 1.0;
	//double dy = 1.0;
	double dx = 1.0 / (X - 1);
	double dy = 1.0 / (Y - 1);

	unsigned int iterations = 100;

	// gauss-seidel
	for (int i = 0; i < iterations; i++)
	{
		for (int x = 1; x < X - 1; x++)
		{
			for (int y = 1; y < Y - 1; y++)
			{
				grid(x, y) = 1.0 / (2.0 / (dx * dx) + 2.0 / (dy * dy)) * ((grid(x + 1, y) + grid(x - 1, y)) / (dx * dx) + (grid(x, y + 1) + grid(x, y - 1)) / (dy * dy));
			}
		}
	}

	return grid;
}

template <unsigned int T, unsigned int U>
Matrix<double, T, U> randomGrid()
{
	std::uniform_real_distribution<double> random(0.0, 1.0);
	std::default_random_engine generator;

	Matrix<double, T, U> grid;

	for (int i = 0; i < std::get<1>(grid.getSize()); i++)
	{
		for (int j = 0; j < std::get<0>(grid.getSize()); j++)
		{
			grid(i, j) = random(generator);
		}
	}
	return grid;
}

double zoom = 0;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	zoom += yoffset;
}

int main()
{
	auto grid = laplaceFinitDifference<16, 16>();
    
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
		glFrustum(-0.001 * fract, 0.001 * fract, -0.001, 0.001, 0.01, 100000.0);
        
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
                glVertex3d((i * 100 - gridOffsetX) / (double)std::get<1>(grid.getSize()), (j * 100 - gridOffsetY) / (double)std::get<1>(grid.getSize()), grid(i, j) * 100);
                glVertex3d(((i + 1) * 100 - gridOffsetX) / (double)std::get<1>(grid.getSize()), (j * 100 - gridOffsetY) / (double)std::get<1>(grid.getSize()), grid(i + 1, j) * 100);
                glVertex3d((i * 100 - gridOffsetX) / (double)std::get<1>(grid.getSize()), (j * 100 - gridOffsetY) / (double)std::get<1>(grid.getSize()), grid(i, j) * 100);
                glVertex3d((i * 100 - gridOffsetX) / (double)std::get<1>(grid.getSize()), ((j + 1) * 100 - gridOffsetY) / (double)std::get<1>(grid.getSize()), grid(i, j + 1) * 100);
            }
        }
		for (int i = 0; i < std::get<1>(grid.getSize()) - 1; i++)
		{
			glVertex3d((i * 100 - gridOffsetX) / (double)std::get<1>(grid.getSize()), ((std::get<0>(grid.getSize()) - 1) * 100 - gridOffsetY) / (double)std::get<1>(grid.getSize()), grid(i, (std::get<0>(grid.getSize()) - 1)) * 100);
			glVertex3d(((i + 1) * 100 - gridOffsetX) / (double)std::get<1>(grid.getSize()), (( std::get<0>(grid.getSize()) - 1) * 100 - gridOffsetY) / (double)std::get<1>(grid.getSize()), grid(i + 1, (std::get<0>(grid.getSize()) - 1)) * 100);
		}
		for (int i = 0; i < std::get<0>(grid.getSize()) - 1; i++)
		{
			glVertex3d(((std::get<1>(grid.getSize()) - 1) * 100 - gridOffsetX) / (double)std::get<1>(grid.getSize()), (i * 100 - gridOffsetY) / (double)std::get<1>(grid.getSize()), grid((std::get<1>(grid.getSize()) - 1), i) * 100);
			glVertex3d(((std::get<1>(grid.getSize()) - 1) * 100 - gridOffsetX) / (double)std::get<1>(grid.getSize()), ((i + 1) * 100 - gridOffsetY) / (double)std::get<1>(grid.getSize()), grid((std::get<1>(grid.getSize()) - 1), i + 1) * 100);

		}
        glEnd();
		glPopMatrix();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;
}
