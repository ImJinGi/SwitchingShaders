


#include "cube.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

Cube::Cube()
{
	cubeColor = new objectColor();
	setup();
}

void Cube::setup()
{
	glm::vec4 position[8] = {
		glm::vec4(-1.0f, -1.0f,  1.0f, 1.0f),
		glm::vec4(-1.0f,  1.0f,  1.0f, 1.0f),
		glm::vec4(1.0f,  1.0f,  1.0f, 1.0f),
		glm::vec4(1.0f, -1.0f,  1.0f, 1.0f),

		glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
		glm::vec4(-1.0f,  1.0f, -1.0f, 1.0f),
		glm::vec4(1.0f,  1.0f, -1.0f, 1.0f),
		glm::vec4(1.0f, -1.0f, -1.0f, 1.0f)
	};

	computeNormalVec(position[0], position[1], position[2]);
	computeNormalVec(position[0], position[2], position[3]);

	computeNormalVec(position[2], position[3], position[7]);
	computeNormalVec(position[2], position[7], position[6]);

	computeNormalVec(position[3], position[0], position[4]);
	computeNormalVec(position[3], position[4], position[7]);

	computeNormalVec(position[6], position[5], position[1]);
	computeNormalVec(position[6], position[1], position[2]);

	computeNormalVec(position[4], position[6], position[5]);
	computeNormalVec(position[4], position[7], position[6]);

	computeNormalVec(position[5], position[4], position[0]);
	computeNormalVec(position[5], position[0], position[1]);

	//create vao
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	//create vbo for vertices
	glGenBuffers(1, &vbo_cube_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cube_vertices.size() * 4, cube_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		0, // attribute
		4,                 // number of elements per vertex, here (x,y,z,1)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
		);
	glEnableVertexAttribArray(0);


	//create vbo for colors
	glGenBuffers(1, &vbo_cube_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cube_normals.size() * 3, cube_normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		1, // attribute
		3,                 // number of elements per vertex, here (R,G,B)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
		);
	glEnableVertexAttribArray(1);


	//glGenBuffers(1, &ibo_cube_elements);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	//	
	//glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &elementSize);
		
	glBindVertexArray(0);
	
}

void Cube::draw()
{
	glBindVertexArray(vaoHandle);
	
	glDrawArrays(GL_TRIANGLES, 0, cube_vertices.size() * 3);
	glBindVertexArray(0);


}

void Cube::computeNormalVec(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3)
{
	glm::vec3 v1 = glm::vec3(p2 - p1);
	v1 = glm::normalize(v1);
	glm::vec3 v2 = glm::vec3(p3 - p1);
	v2 = glm::normalize(v2);

	glm::vec3 n1 = glm::cross(v1, v2);

	cube_vertices.push_back(p1);
	cube_vertices.push_back(p2);
	cube_vertices.push_back(p3);

	cube_normals.push_back(n1);
	cube_normals.push_back(n1);
	cube_normals.push_back(n1);
}