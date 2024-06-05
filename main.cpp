#include <GL/glut.h>
#include <iostream>
#include "physics.h"

constexpr int N = 30;
std::unique_ptr<ClothSimulation> Sim{new ClothSimulation(
	N * N,
	0.05f,
	20000.0f,
	0.1f,
	2.f,
	glm::vec3(0, -9.81f, 0)
)};

bool InitGL()
{
	// for(int i = 0; i < Sim->n; ++i) {
	// 	Sim->getMass(Sim->getIdx(i, Sim->n - 1)).vel.z = 10.f;
	// }

	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	return true;
}

void changeSize(int w, int h)
{
    if(h == 0) h = 1;
    float ratio = w * 1.0 / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void Update()
{
	Sim->operate(0.001f);
}

void renderScene(void)
{
	glMatrixMode(GL_MODELVIEW);
	// Reset The Modelview Matrix
	glLoadIdentity();
	
	// Position Camera 40 Meters Up In Z-Direction.
	// Set The Up Vector In Y-Direction So That +X Directs To Right And +Y Directs To Up On The Window.
	gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
	// Clear Screen And Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Update();

	// Start Drawing The Rope.
	glColor3ub(255, 255, 0);
	for(const auto& spring : Sim->springs) {
		const glm::vec3& pos1 = spring->mass1->pos;
		const glm::vec3& pos2 = spring->mass2->pos;
		//std::cout << std::format("{} {} {}\n", pos1.x, pos1.y, pos1.z);
		glLineWidth(4);
		glBegin(GL_LINES);
			glVertex3f(pos1.x, pos1.y, pos1.z);
			glVertex3f(pos2.x, pos2.y, pos2.z);
		glEnd();
	}
	// Drawing The Rope Ends Here.

	// Flush The GL Rendering Pipeline
	glFlush();
    glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'r':
		for(int i = 0; i < 2; ++i) {
			Sim->fixVel[i] = glm::vec3();
		}
		break;
	case 27:
		exit(0);
	case 'd':
		Sim->fixPos[0].x += 0.05f;
		break;
	case 'a':
		Sim->fixPos[0].x -= 0.05f;
		break;
	case 'w':
		Sim->fixPos[0].y += 0.05f;
		break;
	case 's':
		Sim->fixPos[0].y -= 0.05f;
		break;
	case 'l':
		Sim->fixPos[1].x += 0.05f;
		break;
	case 'j':
		Sim->fixPos[1].x -= 0.05f;
		break;
	case 'i':
		Sim->fixPos[1].y += 0.05f;
		break;
	case 'k':
		Sim->fixPos[1].y -= 0.05f;
		break;
    }
}

void processSpecialKeys(int key, int x, int y)
{
    switch(key)
	{
	case 'd':
		Sim->fixVel[0].x += 1.0f;
		break;
	case 'a':
		Sim->fixVel[0].x -= 1.0f;
		break;
	case 'w':
		Sim->fixVel[0].y += 1.0f;
		break;
	case 's':
		Sim->fixVel[0].y -= 1.0f;
		break;
	case 'l':
		Sim->fixVel[1].x += 1.0f;
		break;
	case 'j':
		Sim->fixVel[1].x -= 1.0f;
		break;
	case 'i':
		Sim->fixVel[1].y += 1.0f;
		break;
	case 'k':
		Sim->fixVel[1].y -= 1.0f;
		break;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(640,640);
    glutCreateWindow("Cloth Simulation");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);

	if(!InitGL()) {
		std::cout << "Initialization Failed.\n";
		return 1;
	}

    glutKeyboardFunc(processNormalKeys);
    //glutSpecialFunc(processSpecialKeys);

    glutMainLoop();

    return 0;
}