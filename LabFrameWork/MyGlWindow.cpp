#include "MyGlWindow.h"


#include <vector>

#include  <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static float DEFAULT_VIEW_POINT[3] = { 5, 5, 5 };
static float DEFAULT_VIEW_CENTER[3] = { 0, 0, 0 };
static float DEFAULT_UP_VECTOR[3] = { 0, 1, 0 };


MyGlWindow::MyGlWindow(int w, int h)
//==========================================================================
{
	m_width = w;
	m_height = h;

	// initialize object value
	m_floor = 0;
	m_sphere = 0;
	m_cube = 0;

	glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
	glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
	glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

	float aspect = (w / (float)h);
	m_viewer = new Viewer(viewPoint, viewCenter, upVector, 45.0f, aspect);
	m_rotate = 0;

	// initialize shader value
	m_myFloorShader = 0;
	m_mySphereShader = 0;
	m_myCubeShader = 0;

	initialize();

	initColor();

	setupShaders();
}

glm::mat4 lookAt(glm::vec3 pos, glm::vec3 look, glm::vec3 up)
{
	glm::vec3 zaxis = glm::normalize(pos - look);
	glm::vec3 xaxis = glm::normalize(glm::cross(up, zaxis));
	glm::vec3 yaxis = glm::normalize(glm::cross(zaxis, xaxis));

	glm::mat4 R;

	R[0] = glm::vec4(xaxis.x, yaxis.x, zaxis.x, 0.0f);
	R[1] = glm::vec4(xaxis.y, yaxis.y, zaxis.y, 0.0f);
	R[2] = glm::vec4(xaxis.z, yaxis.z, zaxis.z, 0.0f);
	R[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 T;

	T[0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	T[1] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	T[2] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	T[3] = glm::vec4(-pos.x, -pos.y, -pos.z, 1.0f);


	return R*T;
}


glm::mat4 perspective(float fov, float aspect, float n, float f)
{
	glm::mat4 P(0.0f);

	const float tanHalfFOV = 1.0f / (tan(glm::radians(fov) / 2.0f));

	float A = -(n + f) / (f - n);
	float B = -(2 * (n * f)) / (f - n);

	P[0] = glm::vec4(tanHalfFOV / aspect, 0, 0, 0);
	P[1] = glm::vec4(0, tanHalfFOV, 0, 0);
	P[2] = glm::vec4(0, 0, A, -1.0f);
	P[3] = glm::vec4(0, 0, B, 0.0f);


	return P;

}

void MyGlWindow::drawFloor(shaderLoading * shader, glm::mat4 & view, glm::mat4 & projection)
{
	m_model.glPushMatrix();

	m_model.glTranslate(0, 0, 0);
	glm::mat4 mview = view * m_model.getMatrix();
	glm::mat4 mvp = projection * view * m_model.getMatrix();

	glm::vec4 lightPos(10, 10, 10, 1);
	glm::vec3 La(0.1, 0.1, 0.1);
	glm::vec3 Ld(0.5, 0.5, 0.5);
	glm::vec3 Ls(1.0, 1.0, 1.0);

	glm::vec3 Ka(0.1, 0.1, 0.1);
	glm::vec3 Kd(0.4, 0.4, 0.4);
	glm::vec3 Ks(0.9, 0.9, 0.9);
	GLfloat shiness = 10.0f;

	glm::mat4 imvp = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imvp));
	glm::mat3 nMatrix = glm::mat3(glm::vec3(view[0]), glm::vec3(view[1]), glm::vec3(view[2]));
	shader->use();

	shader->setMat4("ModelViewMatrix", mview);
	shader->setMat3("NormalMatrix", nmat);
	shader->setMat4("MVP", mvp);

	shader->setVec3("Light.position", glm::vec3(view * lightPos));
	shader->setVec3("Light.direction", glm::vec3(nMatrix * glm::vec3(-lightPos)));
	shader->setFloat("Light.cutoff", glm::degrees(30.0f));
	shader->setVec3("Light.La", La);
	shader->setVec3("Light.Ld", Ld);
	shader->setVec3("Light.Ls", Ls);

	shader->setVec3("Light.intensity", glm::vec3(0.7f, 0.7f, 0.7f));
	shader->setFloat("Light.exponent", 30.0f);

	shader->setVec3("Material.Ka", Ka);
	shader->setVec3("Material.Kd", m_floor->floorColor->Kd_x, m_floor->floorColor->Kd_y, m_floor->floorColor->Kd_z);
	shader->setVec3("Material.Ks", Ks);
	shader->setFloat("Material.shiness", shiness);


	if (m_floor)
		m_floor->draw();

	shader->disable();
	m_model.glPopMatrix();
}

void MyGlWindow::drawSphere(shaderLoading * shader, glm::mat4 & view, glm::mat4 & projection)
{
	m_model.glPushMatrix();

	m_model.glTranslate(0, 1, 4);
	glm::mat4 mview = view * m_model.getMatrix();
	glm::mat4 mvp = projection * view * m_model.getMatrix();

	glm::vec3 La(0.1, 0.1, 0.1);
	glm::vec3 Ld(0.5, 0.5, 0.5);
	glm::vec3 Ls(1.0, 1.0, 1.0);

	glm::vec3 Ka(0.1, 0.1, 0.1);
	glm::vec3 Kd(0.4, 0.4, 0.4);
	glm::vec3 Ks(0.9, 0.9, 0.9);
	GLfloat shiness = 100;

	glm::mat4 imvp = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imvp));

	shader->use();

	shader->setMat4("ModelViewMatrix", mview);
	shader->setMat3("NormalMatrix", nmat);
	shader->setMat4("MVP", mvp);

	shader->setVec4("Light.Position", glm::vec4(*m_lightPos, 1));
	shader->setVec3("Light.La", La);
	shader->setVec3("Light.Ld", Ld);
	shader->setVec3("Light.Ls", Ls);
	
	shader->setVec3("Material.Ka", Ka);
	shader->setVec3("Material.Kd", m_sphere->sphereColor->Kd_x, m_sphere->sphereColor->Kd_y, m_sphere->sphereColor->Kd_z);
	shader->setVec3("Material.Ks", Ks);
	shader->setFloat("Material.shiness", shiness);

	glm::vec4 LightLookingAt(0, 0, 0, 1);
	glm::vec4 spotPosition = glm::vec4(0.0f, 10.0f, 0.0f, 1.0);
	glm::vec3 spotdirection = glm::vec3(glm::normalize(view * LightLookingAt - view * glm::vec4(*m_lightPos, 1)));
	float intensity = 30.0f;
	float exponent = 20.0f;
	float cutoff = glm::degrees(30.0f);

	shader->setVec4("Spot.position", view * glm::vec4(*m_lightPos, 1));
	shader->setVec3("Spot.direction", spotdirection);
	shader->setFloat("Spot.intensity", intensity);
	shader->setFloat("Spot.exponent", exponent);
	shader->setFloat("Spot.cutoff", cutoff);


	if (m_sphere)
		m_sphere->draw();

	shader->disable();
	m_model.glPopMatrix();
}

void MyGlWindow::drawCube(shaderLoading * shader, glm::mat4 & view, glm::mat4 & projection)
{
	m_model.glPushMatrix();

	m_model.glTranslate(4, 1, 0);
	glm::mat4 mview = view * m_model.getMatrix();
	glm::mat4 mvp = projection * view * m_model.getMatrix();

	glm::vec3 La(0.1, 0.1, 0.1);
	glm::vec3 Ld(0.5, 0.5, 0.5);
	glm::vec3 Ls(1.0, 1.0, 1.0);

	glm::vec3 Ka(0.1, 0.1, 0.1);
	glm::vec3 Kd(0.4, 0.4, 0.4);
	glm::vec3 Ks(0.9, 0.9, 0.9);
	GLfloat shiness = 100;

	glm::mat4 imvp = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imvp));

	shader->use();

	shader->setMat4("ModelViewMatrix", mview);
	shader->setMat3("NormalMatrix", nmat);
	shader->setMat4("MVP", mvp);

	shader->setVec4("Light.Position", glm::vec4(*m_lightPos, 1));
	shader->setVec3("Light.La", La);
	shader->setVec3("Light.Ld", Ld);
	shader->setVec3("Light.Ls", Ls);

	shader->setVec3("Material.Ka", Ka);
	shader->setVec3("Material.Kd", m_cube->cubeColor->Kd_x, m_cube->cubeColor->Kd_y, m_cube->cubeColor->Kd_z);
	shader->setVec3("Material.Ks", Ks);
	shader->setFloat("Material.shiness", shiness);

	glm::vec4 LightLookingAt(0, 0, 0, 1);
	glm::vec4 spotPosition = glm::vec4(0.0f, 10.0f, 0.0f, 1.0);
	glm::vec3 spotdirection = glm::vec3(glm::normalize(view * LightLookingAt - view * glm::vec4(*m_lightPos, 1)));
	float intensity = 30.0f;
	float exponent = 20.0f;
	float cutoff = glm::degrees(30.0f);

	shader->setVec4("Spot.position", view * glm::vec4(*m_lightPos, 1));
	shader->setVec3("Spot.direction", spotdirection);
	shader->setFloat("Spot.intensity", intensity);
	shader->setFloat("Spot.exponent", exponent);
	shader->setFloat("Spot.cutoff", cutoff);

	if (m_cube)
		m_cube->draw();

	shader->disable();
	m_model.glPopMatrix();
}

void MyGlWindow::drawModel(shaderLoading * shader, glm::mat4& view, glm::mat4 &projection)
{
	m_model.glPushMatrix();

	m_model.glTranslate(-4, 0, 0);
	//m_model.glScale(0.01, 0.01, 0.01);
	glm::mat4 mview = view * m_model.getMatrix();
	glm::mat4 mvp = projection * view * m_model.getMatrix();

	glm::vec3 La(0.1, 0.1, 0.1);
	glm::vec3 Ld(0.5, 0.5, 0.5);
	glm::vec3 Ls(1.0, 1.0, 1.0);

	glm::vec3 Ka(0.1, 0.1, 0.1);
	glm::vec3 Kd(0.4, 0.4, 0.4);
	glm::vec3 Ks(0.9, 0.9, 0.9);
	GLfloat shiness = 100;

	glm::mat4 imvp = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imvp));

	shader->use();

	shader->setMat4("ModelViewMatrix", mview);
	shader->setMat3("NormalMatrix", nmat);
	shader->setMat4("MVP", mvp);

	shader->setVec4("Light.Position", glm::vec4(*m_lightPos, 1));
	shader->setVec3("Light.La", La);
	shader->setVec3("Light.Ld", Ld);
	shader->setVec3("Light.Ls", Ls);

	shader->setVec3("Material.Ka", Ka);
	shader->setVec3("Material.Kd", m_mesh->meshColor->Kd_x, m_mesh->meshColor->Kd_y, m_mesh->meshColor->Kd_z);
	shader->setVec3("Material.Ks", Ks);
	shader->setFloat("Material.shiness", shiness);

	glm::vec4 LightLookingAt(0, 0, 0, 1);
	glm::vec4 spotPosition = glm::vec4(0.0f, 10.0f, 0.0f, 1.0);
	glm::vec3 spotdirection = glm::vec3(glm::normalize(view * LightLookingAt - view * glm::vec4(*m_lightPos, 1)));
	float intensity = 30.0f;
	float exponent = 20.0f;
	float cutoff = glm::degrees(30.0f);

	shader->setVec4("Spot.position", view * glm::vec4(*m_lightPos, 1));
	shader->setVec3("Spot.direction", spotdirection);
	shader->setFloat("Spot.intensity", intensity);
	shader->setFloat("Spot.exponent", exponent);
	shader->setFloat("Spot.cutoff", cutoff);

	if (m_mesh)
		m_mesh->render();

	shader->disable();
	m_model.glPopMatrix();
}

void MyGlWindow::printFloor()
{
	m_myFloorShader->showShaderName("myFloorShader");
	m_myFloorShader->showAttributeLocation("VertexPosition");
	m_myFloorShader->showAttributeLocation("VertexNormal");

	m_myFloorShader->showUniformLocation("ModelViewMatrix");
	m_myFloorShader->showUniformLocation("NormalMatrix");
	m_myFloorShader->showUniformLocation("MVP");

	m_myFloorShader->showUniformLocation("Light.position");
	m_myFloorShader->showUniformLocation("Light.La");
	m_myFloorShader->showUniformLocation("Light.Ld");
	m_myFloorShader->showUniformLocation("Light.Ls");

	m_myFloorShader->showUniformLocation("Material.Ka");
	m_myFloorShader->showUniformLocation("Material.Kd");
	m_myFloorShader->showUniformLocation("Material.Ks");
	m_myFloorShader->showUniformLocation("Material.shiness");

	//m_myFloorShader->showUniformLocation("Spot.position");
	//m_myFloorShader->showUniformLocation("Spot.direction");
	//m_myFloorShader->showUniformLocation("Spot.intensity");
	//m_myFloorShader->showUniformLocation("Spot.cutoff");
	//m_myFloorShader->showUniformLocation("Spot.exponent");

	std::cout << std::endl;
}

void MyGlWindow::printSphere()
{
	m_mySphereShader->showShaderName("mySphereShader");
	m_mySphereShader->showAttributeLocation("VertexPosition");
	m_mySphereShader->showAttributeLocation("VertexNormal");

	m_mySphereShader->showUniformLocation("ModelViewMatrix");
	m_mySphereShader->showUniformLocation("NormalMatrix");
	m_mySphereShader->showUniformLocation("MVP");

	m_mySphereShader->showUniformLocation("Light.Position");
	m_mySphereShader->showUniformLocation("Light.La");
	m_mySphereShader->showUniformLocation("Light.Ld");
	m_mySphereShader->showUniformLocation("Light.Ls");

	m_mySphereShader->showUniformLocation("Material.Ka");
	m_mySphereShader->showUniformLocation("Material.Kd");
	m_mySphereShader->showUniformLocation("Material.Ks");
	m_mySphereShader->showUniformLocation("Material.shiness");

	m_mySphereShader->showUniformLocation("Spot.position");
	m_mySphereShader->showUniformLocation("Spot.direction");
	m_mySphereShader->showUniformLocation("Spot.intensity");
	m_mySphereShader->showUniformLocation("Spot.cutoff");
	m_mySphereShader->showUniformLocation("Spot.exponent");

	std::cout << std::endl;
}

void MyGlWindow::printCube()
{
	m_myCubeShader->showShaderName("mySphereShader");
	m_myCubeShader->showAttributeLocation("VertexPosition");
	m_myCubeShader->showAttributeLocation("VertexNormal");

	m_myCubeShader->showUniformLocation("ModelViewMatrix");
	m_myCubeShader->showUniformLocation("NormalMatrix");
	m_myCubeShader->showUniformLocation("MVP");

	m_myCubeShader->showUniformLocation("Light.Position");
	m_myCubeShader->showUniformLocation("Light.La");
	m_myCubeShader->showUniformLocation("Light.Ld");
	m_myCubeShader->showUniformLocation("Light.Ls");

	m_myCubeShader->showUniformLocation("Material.Ka");
	m_myCubeShader->showUniformLocation("Material.Kd");
	m_myCubeShader->showUniformLocation("Material.Ks");
	m_myCubeShader->showUniformLocation("Material.shiness");

	m_myCubeShader->showUniformLocation("Spot.position");
	m_myCubeShader->showUniformLocation("Spot.direction");
	m_myCubeShader->showUniformLocation("Spot.intensity");
	m_myCubeShader->showUniformLocation("Spot.cutoff");
	m_myCubeShader->showUniformLocation("Spot.exponent");

	std::cout << std::endl;
}

void MyGlWindow::setupShaders()
{
	printFloor();
	//printSphere();
	//printCube();
}

void MyGlWindow::draw(void)
{
	glViewport(0, 0, m_width, m_height);

	glm::vec3 eye = m_viewer->getViewPoint(); // m_viewer->getViewPoint().x(), m_viewer->getViewPoint().y(), m_viewer->getViewPoint().z());
	glm::vec3 look = m_viewer->getViewCenter();   //(m_viewer->getViewCenter().x(), m_viewer->getViewCenter().y(), m_viewer->getViewCenter().z());
	glm::vec3 up = m_viewer->getUpVector(); // m_viewer->getUpVector().x(), m_viewer->getUpVector().y(), m_viewer->getUpVector().z());

	
	glm::mat4 view = lookAt(eye, look, up);
	glm::mat4 projection = perspective(45.0f, 1.0f*m_width / m_height, 0.1f, 500.0f);

	drawFloor(m_myFloorShader, view, projection);
	drawSphere(m_mySphereShader, view, projection);
	drawCube(m_myCubeShader, view, projection);
	drawModel(m_myMeshShader, view, projection);


}

MyGlWindow::~MyGlWindow()
{
	delete m_myFloorShader;
	delete m_mySphereShader;
	delete m_myCubeShader;
	delete m_myMeshShader;
}


void MyGlWindow::initialize()
{
	m_floor = new Floor();
	m_myFloorShader = new shaderLoading("shaders/phongFrag.vert", "shaders/phongFrag.frag");

	m_sphere = new Sphere(1, 100, 100);
	m_mySphereShader = new shaderLoading("shaders/phongFrag.vert", "shaders/phongFrag.frag");

	m_cube = new Cube();
	m_myCubeShader = new shaderLoading("shaders/phongFrag.vert", "shaders/phongFrag.frag");

	m_mesh = new Mesh("models/StreetLamp.obj");
	m_myMeshShader = new shaderLoading("shaders/phongFrag.vert", "shaders/phongFrag.frag");
}

void MyGlWindow::initColor()
{
	m_floor->floorColor->Kd_x = 0.4f;
	m_floor->floorColor->Kd_y = 0.4f;
	m_floor->floorColor->Kd_z = 0.4f;

	m_sphere->sphereColor->Kd_x = 0.4f;
	m_sphere->sphereColor->Kd_y = 0.4f;
	m_sphere->sphereColor->Kd_z = 0.4f;

	m_cube->cubeColor->Kd_x = 0.4f;
	m_cube->cubeColor->Kd_y = 0.4f;
	m_cube->cubeColor->Kd_z = 0.4f;

	m_mesh->meshColor->Kd_x = 0.4f;
	m_mesh->meshColor->Kd_y = 0.4f;
	m_mesh->meshColor->Kd_z = 0.4f;
}