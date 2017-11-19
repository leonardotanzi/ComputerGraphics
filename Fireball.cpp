#include <stdio.h>+
#include <stdlib.h>
#include <gl/GLUT.h>
#include <math.h>
#include <string>
#include <string.h>
#include <sstream>

// Interruttori di gioco, menu e gameover
bool gameOn = false, gameOver = false;
char *gameOverString = "GAME OVER", *newGameString = "NUOVA PARTITA", *finalScoreString = "Punteggio: ";
const float xNewGame = 0.5f, yNewGame = 0.0f, xGameOver = -0.5f, yGameOver = 2.0f, xFinalScore = -0.55f, yFinalScore = 1.5f, changeColorInterval = 1000.0f;
float changeColorTime = 0.0f;

// Id degli oggetti
const int BALL = 0, LAVA = 1, WATER = 2, COIN = 3, TREE = 4;
// N degli oggetti generati
const int NLAVA = 2, NWATER = 2, NCOINS = 5, NTREES = 10;

// Raggi/lati degli oggetti
float ballRayInit = 0.3f, ballRay = ballRayInit, coinRay = 0.15f;
const float lavaEdge = 0.8f, waterEdge = 0.8f, treeEdge = 1.0f;

// Valori minimi e massimi della x della palla e degli oggetti
const float xMin = -2.01f, xMax = 2.01f;
// Valori minimi e massimi del raggio della palla
float ballRayMin = 0.1f, ballRayMax = 2*ballRayInit;
// Velocità con cui si muovono gli oggetti
float speedInit = 0.01f, speed = speedInit, minSpeed = 0.001f, maxSpeed = 0.05f, speedIncr = 0.001f;
// Valore di partenza della palla
float xBallInit = 0.0f, xBall = xBallInit;
// Velocità della palla
const float xMovement = 0.1f;
// Velocità di rotazione degli oggetti (palla e moneta)
float angleInit = 0.0f, ballAngle = angleInit, coinAngle = angleInit, speedRot = 0.2f;
// Incremento/decremento palla
const float ballIncr = 0.05f;

// Posizione iniziale oggetti
float startDistance = 100.0f;
// Valori iniziali delle x e delle z degli oggetti
float xLava[] = { 0.0f, 0.0f };
float zLava[] = { startDistance, startDistance };
float xWater[] = { 0.0f, 0.0f };
float zWater[] = { startDistance, startDistance };
float xCoin[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float zCoin[] = { startDistance, startDistance, startDistance, startDistance, startDistance };
float xTree[] = { startDistance, startDistance, startDistance, startDistance, startDistance };
float zTree[] = { startDistance, startDistance, startDistance, startDistance, startDistance };
bool lavaCollision[] = { false, false };
bool waterCollision[] = { false, false };

// Valori HUD
// Vite
const float lifeRay = 0.1f, xLifeCenter = -1.7f, yLifeCenter = 2.7f, distanceBetweenLifes = 0.01f;
int maxLifes = 5, nLifesInit = 3, nLifes = nLifesInit;
// Punteggio
const float xPoints = 0.0f, yPoints = 2.5f, xMult = 1.5f, yMult = 2.5f;
int pointsInit = 0, points = pointsInit, lavaPoints = 50, waterPoints = 50, coinPoints = 100,
multiplierCountInit = 10, multiplierCount = multiplierCountInit, multiplierIncr = 5;

// Calcola il modulo di un numero
float modulo(float n) {
	if (n < 0) n = n - 2 * n;
	return n;
}

void drawText(float x, float y, char *string, int value)
{
	char myStr[10];
	if (string == "") {
		sprintf(myStr, "%d", value);
		string = myStr;
	}
	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);
	//get the length of the string to display
	int len = (int)strlen(string);

	//loop to display character by character
	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
};

// Funzioni che disegnano gli elementi di gioco, richiamate nel drawRandomObject()
void drawGround() {
	glColor3f(0.5f, 0.2f, 0.2f);
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();
	glColor3f(0.3f, 0.1f, 0.1f);
	glBegin(GL_QUADS);
	glVertex3f(xMin - 0.5f, 0.001f, -100.0f);
	glVertex3f(xMin - 0.5f, 0.001f,  100.0f);
	glVertex3f(xMax + 0.5f, 0.001f,  100.0f);
	glVertex3f(xMax + 0.5f, 0.001f, -100.0f);
	glEnd();
}

void drawBall() {
	glColor3f(0.4f, 0.2f, 0.1f);
	glMaterialf(GL_FRONT, GL_AMBIENT, (0.2, 0.2, 0.2, 1));
	glMaterialf(GL_FRONT, GL_DIFFUSE, (0.4, 0.2, 0.1, 1));
	glMaterialf(GL_FRONT, GL_SPECULAR, (0, 0, 0, 1));
	glMaterialf(GL_FRONT, GL_SHININESS, 55);
	glMaterialf(GL_FRONT, GL_EMISSION, (1, 1, 0, 1));
	if (modulo(ballAngle) >= 360) ballAngle = 0;
	else ballAngle += speedRot;
	glPushMatrix();
	glTranslatef(xBall, ballRay, 0.0f);
	glRotatef(ballAngle + speedRot, 1, 0, 0);
	glutSolidSphere(ballRay, 20, 20);
	glPopMatrix();
}

void drawCoin() {
	glColor3f(1.0f, 1.0f, 0.0f);
	glMaterialf(GL_FRONT, GL_AMBIENT, (0, 0, 0, 1));
	glMaterialf(GL_FRONT, GL_DIFFUSE, (1, 1, 0.8, 1));
	glMaterialf(GL_FRONT, GL_SPECULAR, (0, 0, 0, 1));
	glMaterialf(GL_FRONT, GL_SHININESS, 55);
	glMaterialf(GL_FRONT, GL_EMISSION, (1, 1, 0, 1));
	if (modulo(coinAngle) >= 360) coinAngle = 0;
	else coinAngle += speedRot;
	glTranslatef(0.0f, coinRay, 0.0f);
	glRotatef(coinAngle + speedRot, 0, 1, 0);
	glutSolidTorus(coinRay/2, coinRay, 20, 20);
}

void drawLava() {
	glColor3f(1.0f, 0.4f, 0.0f);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glMaterialf(GL_FRONT, GL_AMBIENT, (0.5, 0.2, 0, 1));
	glMaterialf(GL_FRONT, GL_DIFFUSE, (0.8, 0.8, 0.8, 1));
	glMaterialf(GL_FRONT, GL_SPECULAR, (0, 0, 0, 1));
	glMaterialf(GL_FRONT, GL_SHININESS, 5.0);
	glMaterialf(GL_FRONT, GL_EMISSION, (1, 0.6, 0, 1));
	glVertex3f(-lavaEdge / 2, 0.02f, -lavaEdge / 2);
	glVertex3f(-lavaEdge / 2, 0.02f, lavaEdge / 2);
	glVertex3f(lavaEdge / 2, 0.02f, lavaEdge / 2);
	glVertex3f(lavaEdge / 2, 0.02f, -lavaEdge / 2);
	glEnd();
}

void drawWater() {
	glColor3f(0.0f, 0.4f, 1.0f);
	glBegin(GL_QUADS);
	glMaterialf(GL_FRONT, GL_AMBIENT, (0, 0, 1, 1));
	glMaterialf(GL_FRONT, GL_DIFFUSE, (0.1, 0.5, 0.8, 1.0));
	glMaterialf(GL_FRONT, GL_SPECULAR, (1.0, 1.0, 1.0, 1.0));
	glMaterialf(GL_FRONT, GL_SHININESS, 5.0);
	glMaterialf(GL_FRONT, GL_EMISSION, (0, 0.4, 1, 1));
	glVertex3f(-waterEdge / 2, 0.01f, -waterEdge / 2);
	glVertex3f(-waterEdge / 2, 0.01f, waterEdge / 2);
	glVertex3f(waterEdge / 2, 0.01f, waterEdge / 2);
	glVertex3f(waterEdge / 2, 0.01f, -waterEdge / 2);
	glEnd();
}

void drawTree() {
	glColor3f(0.4f, 0.2f, 0.1f);
	glBegin(GL_QUADS);
	glVertex3f(-treeEdge / 8, -treeEdge, 0.0f);
	glVertex3f(-treeEdge / 8, treeEdge, 0.0f);
	glVertex3f(treeEdge / 8, treeEdge, 0.0f);
	glVertex3f(treeEdge / 8, -treeEdge, 0.0f);
	glEnd();
}


void drawLife(float green) {
	glColor3f(0.0f, green, 0.0f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(xLifeCenter, yLifeCenter);
	for(int ang = 0; ang <= 360; ang = ang + 15)
		glVertex2f(xLifeCenter + cos(ang)*lifeRay, yLifeCenter + sin(ang)*lifeRay);
	glEnd();
}

void drawHud() {
	// Disegno le vite
	glPushMatrix();
	for (int i = 0; i < maxLifes; i++) {
		if (i < nLifes) drawLife(1.0f);
		else drawLife(0.0f);
		glTranslatef(distanceBetweenLifes + 2*lifeRay, 0, 0);
	}
	glPopMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	// Scrivo i punti
	drawText(xPoints, yPoints, "", points);
	// Scrivo il moltiplicatore
	char tmp[10], *multStr;
	sprintf(tmp, "x%d", multiplierCount / 10);
	multStr = tmp;
	drawText(xMult, yMult, multStr, -1);
}

// Verifica se l'oggetto in questione collide con uno qualunque tra gli oggetti di tipo LAVA
bool collision_with_lava(int OBJECT, int arrayIndex, float myedge, float x, float z) {
	for (int i = 0; i < NLAVA; i++) {
		if ( (OBJECT != LAVA) || (i != arrayIndex) ) {
			if ( (modulo(xLava[i]-x) < (lavaEdge+myedge)) && (modulo(zLava[i]-z) < (lavaEdge+myedge)) ) {
				// Collisione
				if ( (OBJECT == BALL) && (!lavaCollision[i]) ) {
					lavaCollision[i] = true;
					if (nLifes < maxLifes) {
						nLifes++;
						ballRay += ballIncr;
						points += lavaPoints*(multiplierCount / 10);
					}
				}
				return true;
			}
			else {
				if ( (OBJECT == BALL) && (lavaCollision[i]) ) {
					lavaCollision[i] = false;
				}
			}
		}
	}
	return false;
}

// Verifica se l'oggetto in questione collide con uno qualunque tra gli oggetti di tipo WATER
bool collision_with_water(int OBJECT, int arrayIndex, float myedge, float x, float z) {
	for (int i = 0; i < NWATER; i++) {
		if ((OBJECT != WATER) || (i != arrayIndex)) {
			if ((modulo(xWater[i] - x) < (waterEdge + myedge)) && (modulo(zWater[i] - z) < (waterEdge + myedge))) {
				// Collisione
				if ((OBJECT == BALL) && (!waterCollision[i])) {
					waterCollision[i] = true;
					if (nLifes > 0) {
						nLifes--;
						ballRay -= ballIncr;
						points -= waterPoints;
						multiplierCount = 10;
					}
				}
				return true;
			}
			else {
				if ((OBJECT == BALL) && (waterCollision[i])) {
					waterCollision[i] = false;
				}
			}
		}
	}
	return false;
}

// Verifica se l'oggetto in questione collide con uno qualunque tra gli oggetti di tipo COIN
bool collision_with_coins(int OBJECT, int arrayIndex, float myedge, float x, float z) {
	bool collision = false;
	for (int i = 0; i < NCOINS; i++) {
		if ( (OBJECT != COIN) || (i != arrayIndex) ) {
			if ((modulo(xCoin[i] - x) < (coinRay + myedge)) && (modulo(zCoin[i] - z) < (coinRay + myedge))) {
				// Collisione
				return true;
			}
		}
	}
	return false;
}

// Verifica se l'oggetto in questione collide con uno qualunque tra gli oggetti di tipo COIN
int collision_with_a_coin(float myedge, float x, float z) {
	for (int i = 0; i < NCOINS; i++) {
		if ((modulo(xCoin[i] - x) < (coinRay + myedge)) && (modulo(zCoin[i] - z) < (coinRay + myedge))) {
			// Collisione
			fprintf(stdout, "\a");
			points += coinPoints*(multiplierCount / 10);
			multiplierCount += multiplierIncr;
			zCoin[i] = coinRay + 2 * ballRay + 0.1f;
			return i;
		}
	}
	return -1;
}

// Funzione che genera un qualunque oggetto in modo casuale
void drawRandomObject(int OBJECT) {
	float *x, *z;
	int size = 0, edge = 0;
	switch (OBJECT) {
	case LAVA:
		size = NLAVA;
		edge = lavaEdge;
		x = xLava;
		z = zLava;
		glEnable(GL_NORMALIZE);
		break;
	case WATER:
		size = NWATER;
		edge = waterEdge;
		x = xWater;
		z = zWater;
		glEnable(GL_NORMALIZE);
		break;
	case COIN:
		size = NCOINS;
		edge = coinRay;
		x = xCoin;
		z = zCoin;
		glDisable(GL_NORMALIZE);
		break;
	case TREE:
		size = NTREES;
		edge = treeEdge;
		x = xTree;
		z = zTree;
		glEnable(GL_NORMALIZE);
		break;
	}

	int distanceFromBall = 20;
	// Ripete il ciclo "size" volte, dove size è il numero di oggetti di quel tipo generati
	for (int i = 0; i < size; i++) {
		if (z[i] > edge * distanceFromBall) {
			// Una volta superata la palla, l'oggetto viene rigenerato con una probabilità di 1/500
			// Serve a non generare gli oggetti tutti allo stesso tempo
			int okGenerate = rand() % 500;
			int attempt = 0;
			bool generate = false;
			if (okGenerate == 1) {
				do {
					attempt++;
					x[i] = xMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (xMax - xMin)));
					// Incrementa maggiormente la x se l'oggetto è un albero
					if (OBJECT == TREE) x[i] = modulo(x[i]) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f)));

					z[i] = -30.0f;
					
					if ( (!collision_with_lava(OBJECT, i, edge, x[i], z[i])) && (!collision_with_water(OBJECT, i, edge, x[i], z[i])) ) {
						generate = true;
					}

				} while ((attempt < 10) && (!generate));
				if (!generate) z[i] = edge * distanceFromBall;
			}
		}
		else {
			// Altrimenti l'oggetto si muove incrementando la z
			z[i] += speed;
			glPushMatrix();
			switch (OBJECT) {
			case LAVA:
				glTranslatef(x[i], 0, z[i]);
				drawLava();
				break;
			case WATER:
				glTranslatef(x[i], 0, z[i]);
				drawWater();
				break;
			case COIN:
				glTranslatef(x[i], coinRay, z[i]);
				drawCoin();
				collision_with_a_coin(ballRay, xBall, 0.0);
				break;
			case TREE:
				glTranslatef(xMin - x[i], 0, z[i]);
				drawTree();
				glPopMatrix();
				glPushMatrix();
				glTranslatef(xMax + x[i], 0, z[i]);
				drawTree();
			}
			glPopMatrix();
		}
	}
	collision_with_lava(BALL, -1, 0, xBall, 0);
	collision_with_water(BALL, -1, 0, xBall, 0);
}

// Funzione di diplay
void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// Set the camera
	gluLookAt(0.0f, 1.0f, 5.0f,
		0.0f, 1.0f, 4.0f,
		0.0f, 1.0f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT);
	if (gameOn) {
		// Draw elements
		drawHud();
		drawGround();
		drawBall();
		drawRandomObject(LAVA);
		drawRandomObject(WATER);
		drawRandomObject(COIN);
		drawRandomObject(TREE);

		// Verifico se è gameover
		if (nLifes < 1) {
			gameOn = false;
			gameOver = true;
		}
	}
	else {
		glDisable(GL_LIGHTING);
		glClearColor(0.8f, 0.5f, 0.1f, 1.0f);
		if (gameOver) {
			glColor3f(1.0f, 0.8f, 0.8f);
			drawText(xGameOver, yGameOver, gameOverString, -1);
			char tmp[50], *pointsStr;
			sprintf(tmp, "%s%d", finalScoreString, points);
			pointsStr = tmp;
			glColor3f(0.0f, 0.0f, 0.0f);
			drawText(xFinalScore, yFinalScore, pointsStr, -1);
		}
		if (changeColorTime < changeColorInterval) {
			glColor3f(1.0f, 1.0f, 1.0f);
		}
		else if (changeColorTime < 2 * changeColorInterval) {
			glColor3f(0.0f, 0.0f, 0.0f);
		}
		else {
			changeColorTime = 0.0f;
		}
		changeColorTime += 1.0f;
		drawText(xNewGame, yNewGame, newGameString, -1);
	}

	glutSwapBuffers();
}

// Funzione di inizializzazione
void init() {
	ballRay = ballRayInit;
	speed = speedInit;
	xBall = xBallInit;
	ballAngle = angleInit;
	coinAngle = angleInit;
	nLifes = nLifesInit;
	points = pointsInit;

	for (int i = 0; i < NLAVA; i++) {
		xLava[i] = 0.0f;
		zLava[i] = startDistance;
		lavaCollision[i] = false;
	}
	for (int i = 0; i < NWATER; i++) {
		xWater[i] = 0.0f;
		zWater[i] = startDistance;
		waterCollision[i] = false;
	}
	for (int i = 0; i < NCOINS; i++) {
		xCoin[i] = 0.0f;
		zCoin[i] = startDistance;
	}
	for (int i = 0; i < NTREES; i++) {
		xTree[i] = 0.0f;
		zTree[i] = startDistance;
	}
	
	// Illuminazione
	GLfloat ambient[] = { 0.0,0.0,0.0,1.0 };
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
	glEnable(GL_COLOR_MATERIAL);
	glClearColor(0.8f, 0.5f, 0.1f, 1.0f);
}

// Funzione dei tasti normali
void processNormalKeys(unsigned char key, int x, int y) {
	// 13: codice ascii del tasto ENTER (INVIO)
	const int ENTER = 13;

	switch (key) {
	case ENTER:
		if (!gameOn) {
			gameOn = true;
			gameOver = false;
			changeColorTime = 0.0f;
			init();
		}
	}
}


// Funzione dei tasti speciali
void processSpecialKeys(int key, int xx, int yy) {

	float fraction = 0.1f;

	switch (key) {
	// SINISTRA - DESTRA: sposta palla
	case GLUT_KEY_LEFT:
		if (xBall > xMin)	xBall -= xMovement;
		break;
	case GLUT_KEY_RIGHT:
		if (xBall < xMax)	xBall += xMovement;
		break;
	// SU - GIU': modifica velocità di gioco
	case GLUT_KEY_UP:
		if (speed < maxSpeed)	speed += speedIncr;
		break;
	case GLUT_KEY_DOWN:
		if (speed > minSpeed)	speed -= speedIncr;
		break;
	}
}

// Funzione per il ridimensionamento della finestra
void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you can't make a window of zero width)
	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}


int main(int argc, char **argv) {

	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Fireball");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}
