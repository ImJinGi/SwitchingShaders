
//#define GLFW_INCLUDE_GLU

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>

#include "MyGlWindow.h"

#define VERTEX 1
#define FRAGMENT 2

MyGlWindow * win;

bool lbutton_down;
bool rbutton_down;
bool mbutton_down;
double m_lastMouseX;
double m_lastMouseY;
double cx, cy;

/*----------------------------------------------------------------------------------------------------*/

ImVec4 sphere_color = ImVec4(0.4, 0.4, 0.4, 1.0f);
ImVec4 floor_color = ImVec4(0.4, 0.4, 0.4, 1.0f);
ImVec4 cube_color = ImVec4(0.4, 0.4, 0.4, 1.0f);
ImVec4 mesh_color = ImVec4(0.4, 0.4, 0.4, 1.0f);

// new application window 
static bool isFloor = true;
static bool isSphere = false;
static bool isCube = false;
static bool isMesh = true;

static char floor_buf[128];
static char sphere_buf[128];
static char cube_buf[128];
static char mesh_buf[128];

static ImVec4 _lightPosition = ImVec4(0, 10, 0, 1);

char* convertStringToChar(const std::string &str)
{
	char *retPtr(new char[str.length() + 1]);

	copy(str.begin(), str.end(), retPtr);

	retPtr[str.length()] = '\0';

	return retPtr;
}

std::string convertCharToString(char* cha)
{
	std::string strTemp;
	strTemp = std::string(cha);

	return strTemp;
}

char *selectShader(char* shader, int shaderType)
{
	std::string path = "shaders/";
	if (shaderType == VERTEX)
	{
		std::string fileExtension = ".vert";
		std::string changeChar = convertStringToChar(shader);
		changeChar = path + changeChar + fileExtension;

		shader = convertStringToChar(changeChar);
	}
	else if (shaderType == FRAGMENT)
	{
		std::string fileExtension = ".frag";
		std::string changeChar = convertStringToChar(shader);
		changeChar = path + changeChar + fileExtension;

		shader = convertStringToChar(changeChar);
	}

	return shader;
}

/*----------------------------------------------------------------------------------------------------*/

void window_size_callback(GLFWwindow* window, int width, int height)
{
	win->setSize(width, height);
	win->setAspect(width / (float)height);

}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}


static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	cx = xpos;
	cy = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		m_lastMouseX = xpos;
		m_lastMouseY = ypos;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			lbutton_down = true;
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (GLFW_PRESS == action)
			rbutton_down = true;
		else if (GLFW_RELEASE == action)
			rbutton_down = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (GLFW_PRESS == action)
			mbutton_down = true;
		else if (GLFW_RELEASE == action)
			mbutton_down = false;
	}
}


void mouseDragging(double width, double height)
{

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (lbutton_down) {
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			win->m_viewer->rotate(fractionChangeX, fractionChangeY);
		}
		else if (mbutton_down) {
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			win->m_viewer->zoom(fractionChangeY);
		}
		else if (rbutton_down) {
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			win->m_viewer->translate(-fractionChangeX, -fractionChangeY, 1);
		}
	}


	m_lastMouseX = cx;
	m_lastMouseY = cy;

}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
	{

	}


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	int width = 1600;
	int height = 900;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "OpenGL FrameWork", NULL, NULL);


	if (!window)
	{
		glfwTerminate();
		return -1;
	}


	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init(window, true);

	glfwMakeContextCurrent(window);


	/* Make the window's context current */

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		//Problem: glewInit failed, something is seriously wrong.
		std::cout << "glewInit failed, aborting." << std::endl;
		return 0;
	}


	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));


	glfwSwapInterval(1);  //enable vsync

	win = new MyGlWindow(width, height);

	bool show_test_window = true;
	bool show_another_window = false;

	double previousTime = glfwGetTime();
	int frameCount = 0;
	double lastTime;

	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);

	glfwSetWindowTitle(window, "myGlWindow");
	
	while (!glfwWindowShouldClose(window))
	{
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		
		glClearColor(0.2, 0.2, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		ImGui_ImplGlfwGL3_NewFrame();
		
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin("Control Window"), window_flags) 
		{
			static float light_angle = 0.0f;
			static float light_x = 0.0f;
			static float light_y = 1.0f;
			static float light_z = 0.0f;

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			
			ImGui::Checkbox("Floor Shaders Window", &isFloor);
			ImGui::Checkbox("Sphere Shaders Window", &isSphere);
			ImGui::Checkbox("Cube Shaders Window", &isCube);
			ImGui::Checkbox("Mesh Shaders Window", &isMesh);

			
			ImGui::SliderFloat("Light Rotation", &light_angle, -180.0f, 180.0f);
			ImGui::SliderFloat("Light X", &light_x, -10, 10);
			ImGui::SliderFloat("Light Y", &light_y, 1, 10);
			ImGui::SliderFloat("Light Z", &light_z, -10, 10);

			glm::vec3 lpos = glm::vec3(light_x, light_y, light_z);

			win->m_lightPos = &lpos;
			ImGui::End();
		}

		if (isFloor)
		{
			ImGui::Begin("Floor", &isSphere);
			ImGui::Text("This is Floor Shader Window");

			ImGui::ColorEdit3("floor Color", (float*)&floor_color);

			ImGui::Text("Input Floor's Shader name");
			ImGui::Text("Shader"); ImGui::SameLine();
			ImGui::InputText("", floor_buf, IM_ARRAYSIZE(floor_buf));

			if (ImGui::Button("Input", ImVec2(70, 20)))
			{
				std::cout << floor_buf << std::endl;
				win->m_myFloorShader = new shaderLoading(selectShader(floor_buf, VERTEX), selectShader(floor_buf, FRAGMENT));
			}

			win->m_floor->floorColor->Kd_x = floor_color.x;
			win->m_floor->floorColor->Kd_y = floor_color.y;
			win->m_floor->floorColor->Kd_z = floor_color.z;

			static int item_current_2 = 1;
			struct FuncHolder { static bool ItemGetter(void* data, int idx, const char** out_str) { *out_str = ((const char**)data)[idx]; return true; } };
			const char* items[] = {"phongFrag", "spotlight", "toon"};
			ImGui::Combo("Combo", &item_current_2, &FuncHolder::ItemGetter, items, IM_ARRAYSIZE(items));

			switch (item_current_2)
			{
			case 0:
				win->m_myFloorShader = new shaderLoading(selectShader("phongFrag", VERTEX), selectShader("phongFrag", FRAGMENT));
				break;
			case 1:
				win->m_myFloorShader = new shaderLoading(selectShader("phongFrag", VERTEX), selectShader("phongFrag", FRAGMENT));
				break;
			case 2:
				win->m_myFloorShader = new shaderLoading(selectShader("toon", VERTEX), selectShader("toon", FRAGMENT));
				break;
			}

			ImGui::End();
		}

		if (isSphere)
		{
			ImGui::Begin("Sphere", &isSphere);
			ImGui::Text("This is Sphere Shader Window");

			ImGui::ColorEdit3("sphere Color", (float*)&sphere_color);
			
			ImGui::Text("Input Sphere's Shader name");
			ImGui::Text("Shader"); ImGui::SameLine();
			ImGui::InputText("", sphere_buf, IM_ARRAYSIZE(sphere_buf));
			
			if (ImGui::Button("Input", ImVec2(70, 20)))
			{
				std::cout << sphere_buf << std::endl;
				win->m_mySphereShader = new shaderLoading(selectShader(sphere_buf, VERTEX), selectShader(sphere_buf, FRAGMENT));
			}

			win->m_sphere->sphereColor->Kd_x = sphere_color.x;
			win->m_sphere->sphereColor->Kd_y = sphere_color.y;
			win->m_sphere->sphereColor->Kd_z = sphere_color.z;

			static int item_current_2 = 1;
			struct FuncHolder { static bool ItemGetter(void* data, int idx, const char** out_str) { *out_str = ((const char**)data)[idx]; return true; } };
			const char* items[] = { "phongFrag", "spotlight", "toon" };
			ImGui::Combo("Combo", &item_current_2, &FuncHolder::ItemGetter, items, IM_ARRAYSIZE(items));

			switch (item_current_2)
			{
			case 0:
				win->m_mySphereShader = new shaderLoading(selectShader("phongFrag", VERTEX), selectShader("phongFrag", FRAGMENT));
				break;
			case 1:
				win->m_mySphereShader = new shaderLoading(selectShader("spotlight", VERTEX), selectShader("spotlight", FRAGMENT));
				break;
			case 2:
				win->m_mySphereShader = new shaderLoading(selectShader("toon", VERTEX), selectShader("toon", FRAGMENT));
				break;
			}

			ImGui::End();
		}

		if (isCube)
		{
			ImGui::Begin("Cube", &isCube);
			ImGui::Text("This is Cube Shader Window");

			ImGui::ColorEdit3("cube Color", (float*)&cube_color);

			ImGui::Text("Input Cube's Shader name");
			ImGui::Text("Shader"); ImGui::SameLine();
			ImGui::InputText("", cube_buf, IM_ARRAYSIZE(cube_buf));

			if (ImGui::Button("Input", ImVec2(70, 20)))
			{
				std::cout << cube_buf << std::endl;
				win->m_myCubeShader = new shaderLoading(selectShader(cube_buf, VERTEX), selectShader(cube_buf, FRAGMENT));
			}

			win->m_cube->cubeColor->Kd_x = cube_color.x;
			win->m_cube->cubeColor->Kd_y = cube_color.y;
			win->m_cube->cubeColor->Kd_z = cube_color.z;

			static int item_current_2 = 1;
			struct FuncHolder { static bool ItemGetter(void* data, int idx, const char** out_str) { *out_str = ((const char**)data)[idx]; return true; } };
			const char* items[] = { "phongFrag", "spotlight", "toon" };
			ImGui::Combo("Combo", &item_current_2, &FuncHolder::ItemGetter, items, IM_ARRAYSIZE(items));

			switch (item_current_2)
			{
			case 0:
				win->m_myCubeShader = new shaderLoading(selectShader("phongFrag", VERTEX), selectShader("phongFrag", FRAGMENT));
				break;
			case 1:
				win->m_myCubeShader = new shaderLoading(selectShader("spotlight", VERTEX), selectShader("spotlight", FRAGMENT));
				break;
			case 2:
				win->m_myCubeShader = new shaderLoading(selectShader("toon", VERTEX), selectShader("toon", FRAGMENT));
				break;
			}

			ImGui::End();
		}

		if (isMesh)
		{
			ImGui::Begin("Mesh", &isMesh);
			ImGui::Text("This is Mesh Shader Window");

			ImGui::ColorEdit3("mesh Color", (float*)&mesh_color);

			ImGui::Text("Input Mesh's Shader name");
			ImGui::Text("Shader"); ImGui::SameLine();
			ImGui::InputText("", mesh_buf, IM_ARRAYSIZE(mesh_buf));

			if (ImGui::Button("Input", ImVec2(70, 20)))
			{
				std::cout << mesh_buf << std::endl;
				win->m_myMeshShader = new shaderLoading(selectShader(mesh_buf, VERTEX), selectShader(mesh_buf, FRAGMENT));
			}

			win->m_mesh->meshColor->Kd_x = mesh_color.x;
			win->m_mesh->meshColor->Kd_y = mesh_color.y;
			win->m_mesh->meshColor->Kd_z = mesh_color.z;

			static int item_current_2 = 1;
			struct FuncHolder { static bool ItemGetter(void* data, int idx, const char** out_str) { *out_str = ((const char**)data)[idx]; return true; } };
			const char* items[] = { "phongFrag", "spotlight", "toon" };
			ImGui::Combo("Combo", &item_current_2, &FuncHolder::ItemGetter, items, IM_ARRAYSIZE(items));

			switch (item_current_2)
			{
			case 0:
				win->m_myMeshShader = new shaderLoading(selectShader("phongFrag", VERTEX), selectShader("phongFrag", FRAGMENT));
				break;
			case 1:
				win->m_myMeshShader = new shaderLoading(selectShader("spotlight", VERTEX), selectShader("spotlight", FRAGMENT));
				break;
			case 2:
				win->m_myMeshShader = new shaderLoading(selectShader("toon", VERTEX), selectShader("toon", FRAGMENT));
				break;
			}

			ImGui::End();
		}

		win->draw();

		ImGui::Render();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		//glfwWaitEvents();

		mouseDragging(display_w, display_h);

	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}

