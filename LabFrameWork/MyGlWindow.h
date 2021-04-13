//#define  FREEGLUT_LIB_PRAGMAS  0

#pragma warning(push)
#pragma warning(disable:4311)		// convert void* to long
#pragma warning(disable:4312)		// convert long to void*

#include <iostream>
#include "GL/glew.h"
#include <string>
#include <stack>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"



#include "Viewer.h"
#include "ModelView.h"

#include "ShaderLoad.h"
#include "ObjectColor.h"
#include "GlobalData.h"

#include "floor.h"
#include "Sphere.h"
#include "cube.h"

#include "Mesh.h"

#pragma warning(pop)

static unsigned int FloorShader = 0;

class MyGlWindow {
public:
	MyGlWindow(int w, int h);
	~MyGlWindow();
	void draw();
	void setSize(int w, int h) { m_width = w; m_height = h; }
	void setAspect(float r) { m_viewer->setAspectRatio(r); }
	Viewer *m_viewer;
	float m_rotate;
private:
	int m_width;
	int m_height;
	void initialize();
	void initColor();
	void setupShaders();
	
	Model m_model;

	// draw objects using ShaderLoad.h
	void drawFloor(shaderLoading * shader, glm::mat4 & view, glm::mat4 & projection);
	void drawSphere(shaderLoading * shader, glm::mat4 & view, glm::mat4 & projection);
	void drawCube(shaderLoading * shader, glm::mat4& view, glm::mat4 &projection);
	void drawModel(shaderLoading * shader, glm::mat4& view, glm::mat4 &proejction);
	
	// print object's uniform values
	void printFloor();
	void printSphere();
	void printCube();
	void printModel();

public:
	glm::vec3 * m_lightPos;

	// objects
	Floor * m_floor;
	Sphere * m_sphere;
	Cube * m_cube;
	Mesh * m_mesh;

	// shaders
	shaderLoading * m_myFloorShader;
	shaderLoading * m_mySphereShader;
	shaderLoading * m_myCubeShader;
	shaderLoading * m_myMeshShader;

};
