//// Guida openGL ////

//// FINESTRA ////

// Inizializzazione finestra
glutInit(&argc, argv);
glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
/* Valori:
	GLUT_RGB/RGBA/INDEX: modalità di rappresentazione dei colori
	GLUT_SINGLE/DOUBLE: buffer singolo/doppio
	GLUT_ACCUM: con buffer di accumulazione, per effetti particolari
	GLUT_ALPHA: componente alpha per il buffer del colore
	GLUT_DEPTH: con buffer di profondità
	GLUT_STENCIL: con buffer di stencil
	GLUT_MULTISAMPLE: con supporto per multi-sample
	GLUT_STEREO: finestra stereo
*/

// Funzione per la gestione dell'area di disegno della finestra
glutDisplayFunction(function());

// Titolo, dimensione e posizione iniziale finestra
glutCreateWindow("Titolo finestra");
glutInitWindowSize(x, y);
glutInitWindowPosition(x, y);

// Innesca l'attivazione del ciclo d'ascolto degli eventi
// Una volta chiamata, la funzione non ritorna. Chiamata una sola volta.
glutMainLoop();

// Animazioni (con GLUT_DOUBLE):
// Richiamata quando la finestra deve essere ridisegnata
glutPostRedisplay();
// Scambia i due buffer
glutSwapBuffers();

// Funzione di background
glutIdleFunc(function());

//// COLORI E OGGETTI ////

// Definisce il colore di sfondo della finestra (RGBA)
glClearColor(0.0, 0.0, 0.0, 0.0);

// Imposta lo sfondo utilizzando il colore indicato
glClear(GL_COLOR_BUFFER_BIT);

// Definisce il colore dell'oggetto
glColor3f(1.0, 1.0, 1.0);
glIndex3f(...);
// Oppure
float color_array[] = { 1.0, 1.0, 1.0 };
glColor3fv(color_array);

// Ottenere variabili
float *color = new float[4];
glGetFloatv(GL_CURRENT_COLOR, color);
//glGet-Boolean/Double/Float/Integer-v

// Inizio e fine di un oggetto
glBegin(x);
glEnd();
/* Valori di x:
	GL_POINTS
	GL_LINES
	GL_LINE_STRIP/LOOP: disegna una linea aperta/chiusa
	GL_TRIANGLES
	GL_TRIANGLE_STRIP: disegna una serie di triangoli con lato3 di T1 = lato1 di T2
	GL_TRIANGLE_FAN: tutti i triangoli hanno il primo vertice in comune (es. cerchio)
	GL_QUADS
	GL_QUAD_STRIP
	GL_POLYGON
*/

// Forza la visualizzazione su schermo senza attendere comandi
glFlush();

// Vertici
glVertex2f(1.0, 0.5);
glVertex3i(1, 0, -1);
glPointSize(dimensione);

// Spessore linee
glLineWidth(spessore);
// Pattern: forma - Factor: lunghezza
glLineStipple(int factor, uns short pattern);

// Rettangolo
glRect(x1, y1, x2, y2);

// Modalità di rendering oggetti 3D
glPolygonMode(face, mode);
/* Face: GL_FRONT_AND_BACK/GL_FRONT/GL_BACK
   Mode: GL_POINT/GL_LINE/GL_FILL
*/

// Riempimento poligono
glPolygonStipple(mask: 32x32bit)

// Oggetti 3D
glutSolid / Wire...
glutSolidSphere(raggio, slices(verticale), stacks(orizzontale));
glutSolidCube(lato);
glutSolidCone(raggio di base, altezza, slices, stacks);
glutSolidTeapot(dimensione);

// Modello di sfumatura
glShadeModel(GL_SMOOTH / GL_FLAT);

// Caratteri
glutBitmapCharacter(void *font, character);
/* Font: GLUT_BITMAP_8_BY_13/9_BY_15 - GLUT_BITMAP_TIMES_ROMAN_10/24
		 GLUT_BITMAP_HELVETICA_10/12/18
   Character: un char, non una stringa!
*/

//// VISTE ////

// Viewport: area della finestra in cui si va a disegnare
glViewport(x, y, larghezza, altezza);

// Vista ortogonale
glOrtho(left, right, bottom, top, near, far);

// Vista prospettica
gluPerspective(fovy, aspect, zNear, zFar);
/* Fovy: angolo del piano xz
   Aspect: larghezza/altezza
*/

// Posizione telecamera
gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
/* Eye: posizione osservatore (o camera)
   Center: dove la camera punta, il centro della scena osservata (View Reference Point)
   Up: inclinazione (rotazione laterale) della camera (View Up Vector)
*/

//// MATRICI ////

// Matrice di trasformazione corrente
glMatrixMode(GL_MODELVIEW / GL_PROJECTION / GL_TEXTURE);

// Svuotamento stack
glLoadIdentity();

// Trasformazioni (ordine INVERSO!!!)
glTranslatef(x, y, z);
glRotatef(angolo, x, y, z); //x, y, z: asse di rotazione

// Push/pop di matrici dallo stack
glPushMatrix();
glPopMatrix();

//// COMANDI ////

// Tastiera
glutKeyboardFunc(void function(unsigned char key, int x, int y));
glutSpecialFunc(void function(int k, int x, int y))

// Mouse
glutMouseFunc(void function(int button, int state, int x, int y));
/* Valori di button: GLUT_LEFT/MIDDLE/RIGHT_BUTTON
   Valori di state: GLUT_UP/DOWN, se la funzione viene invocata al rilascio/alla pressione del pulsante
   x,y: posizione mouse
*/
glutMotionFunction(void function(x,y));
glutPassiveMotionFunc(void function(x, y));

// Ridimensionamento finestra corrente
glutReshapeFunc(function(larghezza, altezza));

// Creazione menu
glutCreateMenu(void function(valore menu));
glutSetMenu(int menu); // Imposta il menu corrente
glutGetMenu(); // Ottiene il menu corrente
glutDestroyMenu(int menu);
glutAddMenuEntry(testo menu, valore menu);
glutAddSubMenu(testo menu, id submenu);
//

//// ILLUMINAZIONE ////

// Definizione luci
glLight(v)f / i(GL_LIGHT0 / 1 / ... / 7, tipo luce, valori luce);
/* Tipi e valori:
	GL_AMBIENT: luce ambientale, valori: RGBA (default: (0,0,0,1))
	GL_DIFFUSE: luce diffusa, valori: RGBA (default: (1,1,1,1) GL_LIGHT0, (0,0,0,1) altre luci)
	GL_SPECULAR: luce speculare, valori: RGBA (default: (1,1,1,1) GL_LIGHT0, (0,0,0,1) altre luci)
	GL_EMISSIVE: luce emissiva, valori: RGBA
	GL_POSITION: valori: vettore di coordinate x,y,z,w
		w = 0: luce direzionale infinita, w = 1: luce posizionale
*/

// Modello di illuminazione
glLightModel(v)f / i(tipo, valore);
/* Tipi e valori:
	GL_LIGHT_MODEL_AMBIENT: luce ambiente, valori: RGBA (default: (0.2,0.2,0.2,1))
	GL_LIGHT_MODEL_TWO_SIDE: solo le facce anteriori vengono illuminate (GL_FALSE, default) o entrambe (GL_TRUE)
	GL_LIGHT_MODEL_LOCAL_VIEWER: punto di vista considerato per la luce speculare (GL_TRUE) o no (GL_FALSE, default)
*/

// Materiali
glMaterial(v)f / i(faccia, tipo, valore);
/* Face: GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
   Tipi e valori:
    GL_AMBIENT: RGBA (default: (0.2,0.2,0.2,1))
	GL_DIFFUSE: RGBA (default: (0.8,0.8,0.8,1))
	GL_SPECULAR: RGBA (default: (0,0,0,1))
	GL_AMBIENT_AND_DIFFUSE: per assegnare a GL_AMBIENT e GL_DIFFUSE lo stesso vettore RGBA
	GL_EMISSION: RGBA (default: (0,0,0,1))
	GL_SHININESS: intensità della luce speculare riflessa, valore tra 0 e 128
*/

// Normali
glNormal3f(x, y, z);
glEnable(GL_NORMALIZE);

// Procedura di illuminazione:
glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0); glEnable(GL_LIGHT1); ...
glEnable(GL_NORMALIZE);
glLightModel(...);
glLightfv(GL_LIGHTi, GL_POSITION, ...); glLightfv(GL_LIGHTi, GL_AMBIENT, ...); ...
glMaterial(...); glNormal(...);


//// PROFONDITA' ////

// Abilito profondità
glEnable(GL_DEPTH_TEST);

// Tutti i valori vengono impostati alla distanza più grande possibile
glClear(GL_DEPTH_BUFFER_BIT);

// Trasparenza: Colore = Csorgente*sfactor + Cdestinazione*dfactor
glEnable(GL_BLEND);
glBlendFunc(sfactor, dfactor);
/* sfactor: come i fattori RGBA di sorgente devono essere calcolati
	GL_ZERO - GL_ONE - GL_DST_COLOR - GL_ONE_MINUS_DST_COLOR
	GL_SRC/DST_ALPHA - GL_ONE_MINUS_SRC/DST_ALPHA - GL_SRC_ALPHA_SATURATE
   dfactor: come i fattori RGBA di destinazione devono essere calcolati
	GL_ZERO - GL_ONE - GL_SRC_COLOR - GL_ONE_MINUS_SRC_COLOR
	GL_SRC/DST_ALPHA - GL_ONE_MINUS_SRC/DST_ALPHA
*/
/* Gestione oggetti trasparenti:
	- Disegno oggetti opachi
	glDepthMask(GL_FALSE);
	- Disegno oggetti trasparenti
	glDepthMask(GL_TRUE);
*/

//// TEXTURE ////

// Seleziona l'id della texture da rendere attiva
glBindTexture(GL_TEXTURE_2D, idtexture);

// Indica qual è l'allineamento dei dati che stanno per essere caricati
glPixelStorei(GL_UNPACK_ALIGNMENT, nbyte x colore)

// Setta parametri della texture attiva
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S / T | GL_TEXTURE_MAG / MIN_FILTER, GL_REPEAT | GL_LINEAR);

// Imposta le variabili d'ambiente per la texture attiva
glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE/GL_DECAL);

// Carica la texture nella memora video
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, larghezza img, altezza img, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
/*	Target: GL_TEXTURE_2D
	Level: livello di dettaglio (di solito 0)
	Formato interno: n componenti di colore memorizzate per la texture (es. GL_RGB)
	Border: bordo dell'immagine, 0 o 1
	Formato: formato dei dati caricati (es. GL_RGB)
	Tipo: tipo di dati caricati (es. GL_UNSIGNED_BYTE)
	Pixels: puntatore ai dati (immagine)
*/

/* Procedimento:
	- glEnable(GL_TEXTURE_2D);
	- glBindTexture(...); prima di ogni glBegin() o glEnd()
	- Verificare di aver fornito le coordinate di texture prima di specificare ogni vertice che compone la faccia (TexCoord2f(0.0, 0.0))
	- Verificare di aver memorizzato l'identificatore nella texture in una variabile (es. glGenTextures(...))
*/
