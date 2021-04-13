#ifndef __CUBE_H__
#define __CUBE_H__

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include <vector>

#include "ObjectColor.h"


class Cube
{
private:
	std::vector<glm::vec3> cube_normals;
	std::vector<glm::vec4> cube_vertices;

public:
	Cube();
	void setup();
	void draw();

	int elementSize;

	GLuint vaoHandle;
	GLuint vbo_cube_vertices, vbo_cube_normals, ibo_cube_elements;

	void computeNormalVec(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3);

	objectColor *cubeColor;

};

#endif