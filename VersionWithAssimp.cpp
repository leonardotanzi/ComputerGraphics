//
//  main.cpp
//  ProgettoFinaleInfoGrafica
//
//  Created by leonardo tanzi on 19/11/17.
//  Copyright © 2017 leonardo tanzi. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <math.h>
#include <string.h>
#include <map>
#include <string>
#include <sstream>

#include "IL/il.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"

// ----------------------------------------------------------------------------

// currently this is hardcoded
//static const std::string basepath = "./models/textures/"; //obj..
static const std::string basepath = "/Users/leonardotanzi/Desktop/ProvaInfoGrafica1811/ProvaInfoGrafica1811/models/";
const struct aiScene* scene = NULL;
GLuint scene_list = 0;
aiVector3D scene_min, scene_max, scene_center;

// images / texture
std::map<std::string, GLuint*> textureIdMap;    // map image filenames to textureIds
GLuint*        textureIds;                            // pointer to texture Array

GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[]= { 0.0f, 0.0f, 15.0f, 1.0f };

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)
#define TRUE                1
#define FALSE               0

float angle = 0.0f;

// ----------------------------------------------------------------------------


// Id degli oggetti
const int BALL = 0, LAVA = 1, WATER = 2, COIN = 3;
// N degli oggetti generati
const int NLAVA = 2, NWATER = 2, NCOINS = 5;

// Raggi/lati degli oggetti
float ballRay = 0.3f, coinRay = 0.15f;
float lavaEdge = 0.8f, waterEdge = 0.8f;

// Valori minimi e massimi della x della palla e degli oggetti
float xMin = -2.01f, xMax = 2.01f;
// Valori minimi e massimi del raggio della palla
float ballRayMin = 0.1f, ballRayMax = 2*ballRay;
// Velocit‡ con cui si muovono gli oggetti
float speed = 0.1f, minSpeed = 0.01f, maxSpeed = 0.5f, speedIncr = 0.01f;
// Valore di partenza della palla
float xBall = 0.0f;
// Velocit‡ della palla
float xMovement = 0.1f;
// Incremento/decremento palla
float ballIncr = 0.05f;

// Posizione iniziale oggetti
float startDistance = 100.0f;
// Valori iniziali delle x e delle z degli oggetti
float xLava[] = { 0.0f, 0.0f };
float zLava[] = { startDistance, startDistance };
float xWater[] = { 0.0f, 0.0f };
float zWater[] = { startDistance, startDistance };
float xCoin[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float zCoin[] = { startDistance, startDistance, startDistance, startDistance, startDistance };
bool lavaCollision[] = { false, false };
bool waterCollision[] = { false, false };

// Valori HUD
// Vite
float lifeRay = 0.1f, xLifeCenter = -1.7f, yLifeCenter = 2.7f, distanceBetweenLifes = 0.01f;
int maxLifes = 5, nLifes = 3;// maxLifes;
// Punteggio
int points = 0, lavaPoints = 50, waterPoints = 50, coinPoints = 100, multiplierCount = 10, multiplierIncr = 5;




// ----------------------------------------------------------------------------

void get_bounding_box_for_node (const struct aiNode* nd,
                                aiVector3D* min,
                                aiVector3D* max,
                                aiMatrix4x4* trafo
                                ){
    aiMatrix4x4 prev;
    unsigned int n = 0, t;
    
    prev = *trafo;
    aiMultiplyMatrix4(trafo,&nd->mTransformation);
    
    for (; n < nd->mNumMeshes; ++n) {
        const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
        for (t = 0; t < mesh->mNumVertices; ++t) {
            
            aiVector3D tmp = mesh->mVertices[t];
            aiTransformVecByMatrix4(&tmp,trafo);
            
            min->x = aisgl_min(min->x,tmp.x);
            min->y = aisgl_min(min->y,tmp.y);
            min->z = aisgl_min(min->z,tmp.z);
            
            max->x = aisgl_max(max->x,tmp.x);
            max->y = aisgl_max(max->y,tmp.y);
            max->z = aisgl_max(max->z,tmp.z);
        }
    }
    
    for (n = 0; n < nd->mNumChildren; ++n) {
        get_bounding_box_for_node(nd->mChildren[n],min,max,trafo);
    }
    *trafo = prev;
}


void get_bounding_box (aiVector3D* min, aiVector3D* max)
{
    aiMatrix4x4 trafo;
    aiIdentityMatrix4(&trafo);
    
    min->x = min->y = min->z =  1e10f;
    max->x = max->y = max->z = -1e10f;
    get_bounding_box_for_node(scene->mRootNode,min,max,&trafo);
}


void color4_to_float4(const aiColor4D *c, float f[4])
{
    f[0] = c->r;
    f[1] = c->g;
    f[2] = c->b;
    f[3] = c->a;
}


void set_float4(float f[4], float a, float b, float c, float d)
{
    f[0] = a;
    f[1] = b;
    f[2] = c;
    f[3] = d;
}


void Color4f(const aiColor4D *color)
{
    glColor4f(color->r, color->g, color->b, color->a);
}

void recursive_render (const struct aiScene *sc, const struct aiNode* nd, float scale)
{
    unsigned int i;
    unsigned int n=0, t;
    aiMatrix4x4 m = nd->mTransformation;
    
    printf("Node name: %s\n",nd->mName.data);
    
    //m.Scaling(aiVector3D(scale, scale, scale), m);
    
    // update transform
    m.Transpose();
    glPushMatrix();
    glMultMatrixf((float*)&m);
    
    // draw all meshes assigned to this node
    for (; n < nd->mNumMeshes; ++n)
    {
        const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
        
        ///
        printf("Drawing MESH with this name: %s\n",mesh->mName.data);
        
        //apply_material(sc->mMaterials[mesh->mMaterialIndex]);
        
        
        if(mesh->HasTextureCoords(0))
            glEnable(GL_TEXTURE_2D);
        else
            glDisable(GL_TEXTURE_2D);
        if(mesh->mNormals == NULL)
        {
            glDisable(GL_LIGHTING);
        }
        else
        {
            glEnable(GL_LIGHTING);
        }
        
        if(mesh->mColors[0] != NULL)
        {
            glEnable(GL_COLOR_MATERIAL);
        }
        else
        {
            glDisable(GL_COLOR_MATERIAL);
        }
        
        
        
        for (t = 0; t < mesh->mNumFaces; ++t) {
            const struct aiFace* face = &mesh->mFaces[t];
            GLenum face_mode;
            
            switch(face->mNumIndices)
            {
                case 1: face_mode = GL_POINTS; break;
                case 2: face_mode = GL_LINES; break;
                case 3: face_mode = GL_TRIANGLES; break;
                default: face_mode = GL_POLYGON; break;
            }
            
            glBegin(face_mode);
            
            for(i = 0; i < face->mNumIndices; i++)        // go through all vertices in face
            {
                int vertexIndex = face->mIndices[i];    // get group index for current index
                if(mesh->mColors[0] != NULL)
                    Color4f(&mesh->mColors[0][vertexIndex]);
                if(mesh->mNormals != NULL)
                    
                    if(mesh->HasTextureCoords(0))        //HasTextureCoords(texture_coordinates_set)
                    {
                        glTexCoord2f(mesh->mTextureCoords[0][vertexIndex].x, 1 - mesh->mTextureCoords[0][vertexIndex].y); //mTextureCoords[channel][vertex]
                    }
                
                glNormal3fv(&mesh->mNormals[vertexIndex].x);
                glVertex3fv(&mesh->mVertices[vertexIndex].x);
            }
            
            glEnd();
            
        }
        
    }
    
    
    // draw all children
    for (n = 0; n < nd->mNumChildren; ++n)
    {
        recursive_render(sc, nd->mChildren[n], scale);
    }
    
    glPopMatrix();
}

int LoadGLTextures(const aiScene* scene)
{
    ILboolean success;
    
    /* Before calling ilInit() version should be checked. */
    if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
    {
        ILint test = ilGetInteger(IL_VERSION_NUM);
        /// wrong DevIL version ///
        std::string err_msg = "Wrong DevIL version. Old devil.dll in system32/SysWow64?";
        char* cErr_msg = (char *) err_msg.c_str();
        
        return -1;
    }
    
    ilInit(); /* Initialization of DevIL */
    
    //if (scene->HasTextures()) abortGLInit("Support for meshes with embedded textures is not implemented");
    
    /* getTexture Filenames and Numb of Textures */
    for (unsigned int m=0; m<scene->mNumMaterials; m++)
    {
        int texIndex = 0;
        aiReturn texFound = AI_SUCCESS;
        
        aiString path;    // filename
        
        while (texFound == AI_SUCCESS)
        {
            texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
            textureIdMap[path.data] = NULL; //fill map with textures, pointers still NULL yet
            texIndex++;
        }
    }
    
    int numTextures = (int) textureIdMap.size();
    
    /* array with DevIL image IDs */
    ILuint* imageIds = NULL;
    imageIds = new ILuint[numTextures];
    
    /* generate DevIL Image IDs */
    ilGenImages(numTextures, imageIds); /* Generation of numTextures image names */
    
    /* create and fill array with GL texture ids */
    textureIds = new GLuint[numTextures];
    glGenTextures(numTextures, textureIds); /* Texture name generation */
    
    /* define texture path */
    //std::string texturepath = "/Users/leonardotanzi/Downloads/stone/Texture";
    
    /* get iterator */
    std::map<std::string, GLuint*>::iterator itr = textureIdMap.begin();
    
    for (int i=0; i<numTextures; i++)
    {
        
        //save IL image ID
        std::string filename = (*itr).first;  // get filename
        (*itr).second =  &textureIds[i];      // save texture id for filename in map
        itr++;                                  // next texture
        
        
        ilBindImage(imageIds[i]); /* Binding of DevIL image name */
        std::string fileloc = basepath + filename;    /* Loading of image */
        success = ilLoadImage((const char *)fileloc.c_str());
        
        fprintf(stdout,"Loading Image: %s\n", fileloc.data());
        
        if (success) /* If no error occured: */
        {
            success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); /* Convert every colour component into
                                                                 unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
            if (!success)
            {
                /* Error occured */
                fprintf(stderr,"Couldn't convert image");
                return -1;
            }
            //glGenTextures(numTextures, &textureIds[i]); /* Texture name generation */
            glBindTexture(GL_TEXTURE_2D, textureIds[i]); /* Binding of texture name */
            //redefine standard texture values
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear
                                                                               interpolation for magnification filter */
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear
                                                                               interpolation for minifying filter */
            glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
                         ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
                         ilGetData()); /* Texture specification */
        }
        else
        {
            /* Error occured */
            fprintf(stderr,"Couldn't load Image: %s\n", fileloc.data());
        }
    }
    ilDeleteImages(numTextures, imageIds); /* Because we have already copied image data into texture data
                                            we can release memory used by image. */
    
    //Cleanup
    delete [] imageIds;
    imageIds = NULL;
    
    //return success;
    return TRUE;
}
int loadasset (const char* path)
{
    // we are taking one of the postprocessing presets to avoid
    // writing 20 single postprocessing flags here.
    scene = aiImportFile(path,aiProcessPreset_TargetRealtime_Quality);
    if (scene) {
        get_bounding_box(&scene_min,&scene_max);
        scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
        scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
        scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
        return 0;
    }
    
    return 1;
}

int InitGL()                     // All Setup For OpenGL goes here
{
    if (!LoadGLTextures(scene))
    {
        return FALSE;
    }
    
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);         // Enables Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);                // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);        // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);            // The Type Of Depth Test To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);    // Really Nice Perspective Calculation
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);    // Uses default lighting parameters
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
    glEnable(GL_LIGHT1);
    
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    
    return TRUE;                    // Initialization Went OK
}

// ----------------------------------------------------------------------------

char *IntToChar(int number)
{
    std::string s = std::to_string(number);
    char const *pchar = s.c_str();
    //char *s = pchar;
    //return pchar;
    return "";
}

void drawText(int x, int y, int z, char *string, int value)
{
    char const *myStr;
    if (string != "x") {
        glColor3f(1.0f, 1.0f, 1.0f);
        std::string s = std::to_string(value);
        char const *pchar = s.c_str();
        myStr = pchar;
        fprintf(stdout, "%s\n", myStr);
        //sprintf(string, "%d", value);
    }
    else {
        myStr = string;
    }
    //set the position of the text in the window using the x and y coordinates
    glRasterPos2f(x, y);
    //get the length of the string to display
    int len = (int)strlen(myStr);
    fprintf(stdout, "%d\n", len);
    
    //loop to display character by character
    for (int i = 0; i < len; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, myStr[i]);
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
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(xBall, 0.4f, 0.0f);
    glRotatef(angle, -1.0, 0.0, 0.0);
   // glScalef(0.4,0.4,0.4);
    glScalef(0.8 * ballRay, 0.8 * ballRay, 0.8 * ballRay);
    recursive_render(scene, scene->mRootNode, 1.0);
    
    //glutSolidSphere(ballRay, 20, 20);
    glPopMatrix();
}

void drawCoin() {
    glColor3f(1.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, coinRay, 0.0f);
    glutSolidSphere(coinRay, 20, 20);
}

void drawLava() {
    glColor3f(1.0f, 0.4f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(-lavaEdge/2, 0.02f, -lavaEdge/2);
    glVertex3f(-lavaEdge/2, 0.02f, lavaEdge/2);
    glVertex3f(lavaEdge/2, 0.02f, lavaEdge/2);
    glVertex3f(lavaEdge/2, 0.02f, -lavaEdge/2);
    glEnd();
}

void drawWater() {
    glColor3f(0.0f, 0.4f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-waterEdge / 2, 0.01f, -waterEdge / 2);
    glVertex3f(-waterEdge / 2, 0.01f, waterEdge / 2);
    glVertex3f(waterEdge / 2, 0.01f, waterEdge / 2);
    glVertex3f(waterEdge / 2, 0.01f, -waterEdge / 2);
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
    // Scrivo i punti
    drawText(0, yLifeCenter, 0, "", points);
    // Scrivo il moltiplicatore
    drawText(0.9f, yLifeCenter, 0, "x", -1);
    drawText(0.92f, yLifeCenter, 0, "", multiplierCount%10);
}

// Calcola il modulo di un numero
float modulo(float n) {
    if (n < 0) n = n - 2*n;
    return n;
}

// Verifica se l'oggetto in questione collide con uno qualunque tra gli oggetti di tipo LAVA
bool collision_with_lava(int OBJECT, int arrayIndex, float myedge, float x, float z) {
    for (int i = 0; i < NLAVA; i++) {
        if ( (OBJECT != LAVA) || (i != arrayIndex) ) {
            if ( (modulo(xLava[i]-x) < (lavaEdge+myedge)) && (modulo(zLava[i]-z) < (lavaEdge+myedge)) ) {
                //                if (OBJECT == BALL) fprintf(stdout, "++: %f - Distance: x: %f<%f, z: %f<%f\n",
                //                    ballRay, modulo(xLava[i] - x), (lavaEdge + myedge), zLava[i], (lavaEdge + myedge));
                // Collisione
                if ( (OBJECT == BALL) && (!lavaCollision[i]) ) {
                    lavaCollision[i] = true;
                    fprintf(stdout, "lavacoll = true\n");
                    if (nLifes < maxLifes) {
                        nLifes++;
                        ballRay += ballIncr;
                        points += lavaPoints*(multiplierCount % 10);
                    }
                }
                return true;
            }
            else {
                if ( (OBJECT == BALL) && (lavaCollision[i]) ) {
                    lavaCollision[i] = false;
                    fprintf(stdout, "lavacoll = false\n");
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
                points += coinPoints*(multiplierCount % 10);
                multiplierCount += multiplierIncr;
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
            //fprintf(stdout, "%d - z: %f\n", i, zCoin[i]);
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
            break;
        case WATER:
            size = NWATER;
            edge = waterEdge;
            x = xWater;
            z = zWater;
            break;
        case COIN:
            size = NCOINS;
            edge = coinRay;
            x = xCoin;
            z = zCoin;
            break;
    }
    
    int distanceFromBall = 20;
    // Ripete il ciclo "size" volte, dove size Ë il numero di oggetti di quel tipo generati
    for (int i = 0; i < size; i++) {
        if (z[i] > edge * distanceFromBall) {
            // Una volta superata la palla, l'oggetto viene rigenerato con una probabilit‡ di 1/500
            // Serve a non generare gli oggetti tutti allo stesso tempo
            int okGenerate = rand() % 500;
            int attempt = 0;
            bool generate = false;
            if (okGenerate == 1) {
                do {
                    attempt++;
                    x[i] = xMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (xMax - xMin)));
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
            }
            glPopMatrix();
        }
        switch (OBJECT) {
            case LAVA:
                xLava[i] = x[i];
                zLava[i] = z[i];
                break;
            case WATER:
                xWater[i] = x[i];
                zWater[i] = z[i];
                break;
            case COIN:
                xCoin[i] = x[i];
                zCoin[i] = z[i];
                break;
        }
    }
    collision_with_lava(BALL, -1, 0, xBall, 0);
    collision_with_water(BALL, -1, 0, xBall, 0);
}

// Funzione di diplay
void renderScene(void) {
    
    glClearColor(0.8f, 0.5f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    // Set the camera
    gluLookAt(0.0f, 1.0f, 5.0f,
              0.0f, 1.0f, 4.0f,
              0.0f, 1.0f, 0.0f);
    
    // Draw elements
    drawGround();
    drawBall();
    drawRandomObject(LAVA);
    drawRandomObject(WATER);
    drawRandomObject(COIN);
    drawHud();
    
    angle += speed * 12;
    
    glutSwapBuffers();
}

// Funzione dei tasti speciali
void processSpecialKeys(int key, int xx, int yy) {
    
    float fraction = 0.1f;
    
    switch (key) {
        case GLUT_KEY_LEFT:
            if (xBall > xMin)    xBall -= xMovement;
            break;
        case GLUT_KEY_RIGHT:
            if (xBall < xMax)    xBall += xMovement;
            break;
        case GLUT_KEY_UP:
            if (speed < maxSpeed)    speed += speedIncr;
            break;
        case GLUT_KEY_DOWN:
            if (speed > minSpeed)    speed -= speedIncr;
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
    
    struct aiLogStream stream;
    
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
    //glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);
    stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
    aiAttachLogStream(&stream);
    
    // ... exactly the same, but this stream will now write the
    // log file to assimp_log.txt
    stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE,"assimp_log.txt");
    aiAttachLogStream(&stream);
    
    loadasset( "/Users/leonardotanzi/Downloads/stone/Models/stone.obj" );
    //loadasset( "/Users/leonardotanzi/Desktop/ProvaInfoGrafica1811/ProvaInfoGrafica1811/models/suzanne.obj" );
    
    // OpenGL init
    glEnable(GL_DEPTH_TEST);
    InitGL();
    // enter GLUT event processing cycle
    glutMainLoop();
    
    return 1;
}
