#ifndef __FLOOR_H__
#define __FLOOR_H__

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include "Loader.h"
#include "ObjectColor.h"
#include <vector>

class Floor
{
public:
	Floor();
	void setup(float size, int nSquares);
	void draw();

	GLuint vaoHandle;
	GLuint vbo_position, vbo_normal;

	void computeNormal(glm::vec4 p1, glm::vec4 p2, glm::vec4 v3);
	void computeNormal(std::vector<glm::vec4> &vertices,
		std::vector<glm::vec3> &normals,
		glm::vec4 pos[],
		int a, int b, int c, int d);
	objectColor *floorColor;
};

#endif // ! __FLOOR_H__
