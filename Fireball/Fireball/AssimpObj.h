#pragma once

#include "assimp.h"
#include "aiPostProcess.h"
#include "aiScene.h"
#include <gl/GLUT.h>
#include <IL/il.h>
#include <string.h>
#include <map>

/*#include "assimp/Importer"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
*/

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)
#define TRUE                1
#define FALSE               0
// Id degli oggetti
const int BALL = 0, LAVA = 1, WATER = 2, COIN = 3, TREE = 4;


class AssimpObj
{
public:
	AssimpObj(void);
	virtual ~AssimpObj(void);
	void recursive_render(const struct aiScene *sc, const struct aiNode* nd, float scale);
	int InitGL(int myobj);
	const struct aiScene* getScene();


private:
	void get_bounding_box_for_node(const struct aiNode* nd,
		aiVector3D* min,
		aiVector3D* max,
		aiMatrix4x4* trafo
	);
	void get_bounding_box(aiVector3D* min, aiVector3D* max);
	void color4_to_float4(const aiColor4D *c, float f[4]);
	void set_float4(float f[4], float a, float b, float c, float d);
	void Color4f(const aiColor4D *color);
	int LoadGLTextures(const aiScene* scene);
	int loadasset(const char* path);

	// currently this is hardcoded
	//static const std::string basepath = "./models/textures/"; //obj..
	const std::string basepath = "/Users/Emanuele/Documents/Visual Studio 2017/Projects/Informatica grafica/Fireball/models/";
	const char* basepathChar = "/Users/Emanuele/Documents/Visual Studio 2017/Projects/Informatica grafica/Fireball/models/";
	const struct aiScene* scene = NULL;
	GLuint scene_list = 0;
	aiVector3D scene_min, scene_max, scene_center;

	// images / texture
	std::map<std::string, GLuint*> textureIdMap;    // map image filenames to textureIds
	GLuint*        textureIds;                            // pointer to texture Array

	
};