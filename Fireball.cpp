#include <GL/glut.h>
#include <math.h>

GLfloat xpos = 0.0f;
GLfloat raggio = 0.2f;

void display(void)
{
	glLoadIdentity();
	gluLookAt(xpos, 1.0, 1.0, 0.0, 0.5, 0.0, 0.0, 1.0, 0.0);
	glClearColor(0.8, 0.5, 0.1, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(0.3, 0.2, 0.1);
	glBegin(GL_QUADS);
	glVertex3f(1.0, 0.0, -1.0);
	glVertex3f(1.0, 0.0, 1.0);
	glVertex3f(-1.0, 0.0, 1.0);
	glVertex3f(-1.0, 0.0, -1.0);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(1.0, 0.0);
	glVertex2f(-1.0, 0.0);
	glEnd();
	glColor3f(1.0, 1.0, 1.0);

	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_diffuse[] = { 0.4, 0.2, 0.1, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat low_shininess[] = { 5.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

	glPushMatrix();
	glTranslatef(0.0, raggio, 0.0);
	glutSolidSphere(raggio, 20, 20);
	glPopMatrix();

	glFlush();
}

void init()
{
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	/*
	GLfloat ambient[] = { 0.8, 0.5, 0.1, 1.0 };
	GLfloat diffuse[] = { 1.0,1.0,1.0,1.0 };
	GLfloat position[] = { 0.0,3.0,2.0,0.0 };
	GLfloat lmodel_amb[] = { 0.4,0.4,0.4,1.0 };
	GLfloat local_view[] = { 0.0 };
	glEnable(GL_DEPTH_TEST);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_amb);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	*/
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= (h * 2))
		glOrtho(-6.0, 6.0, 3.0*((GLfloat)h * 2) / (GLfloat)w,
			3.0*((GLfloat)h * 2) / (GLfloat)w, -10.0, 10.0);
	else
		glOrtho(-6.0*(GLfloat)w / ((GLfloat)h * 2), 6.0*
		(GLfloat)w / ((GLfloat)h * 2), -3.0, 3.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
}

void tastiSpeciali(int k, int x, int y)
{
	switch (k) {
	case GLUT_KEY_LEFT:
		xpos -= 0.1f;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		xpos += 0.1f;
		glutPostRedisplay();
		break;
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Fireball");
	glutSpecialFunc(tastiSpeciali);

	init();
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}