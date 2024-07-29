/* RayMarching
* 1. Draw a quad with the desired resolution:
*	-Vertex Shader is empty
*	-Fragment Shader is where everything happens
* 2. Send the positions and size of each objects to the gpu
* 3. In the quad's fragment shader cast 'rays' and calculate point of intersections
*/
// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// Other
#include <iostream>
// GUI
#include "Headers/imgui/imgui.h"
#include "Headers/imgui/imgui_impl_opengl3.h"
#include "Headers/imgui/imgui_impl_glfw.h"
// My headers
#include "Headers/Shaders/Shader.hpp"
#include "Headers/IO/Input.hpp"
#include "Headers/Camera.hpp"

using namespace IO;

int main() {
#pragma region init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#pragma endregion

#pragma region Window and Context
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting", NULL, NULL);
	if (window == nullptr) {
		std::cerr << "Failed to create window" << std::endl;
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
#pragma endregion

#pragma region Shader
	const std::string VertexPath = "C:\\Users\\alexa\\OneDrive\\Coding\\C++\\RayMarching\\RayMarching\\res\\Shaders\\Shader.vert";
	const std::string FragPath = "C:\\Users\\alexa\\OneDrive\\Coding\\C++\\RayMarching\\RayMarching\\res\\Shaders\\Shader.frag";

	Shader shader(VertexPath, FragPath);
#pragma endregion

#pragma region Quad
	constexpr float size = 1.0f;
	float vertices[] = {
		-size, -size, 0.0f, // Bottom right
		size, -size, 0.0f, // Bottom left
		size,  size, 0.0f, // Top left
		-size,  size, 0.0f // Top Right
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
#pragma endregion

#pragma region Objects
	float radius = 1.0f;
	glm::vec3 spherePos = {0.0f, 0.0f, 0.0f};
	glm::vec3 sphereColor = {0.0f, 1.0f, 1.0f};

	float radius_2 = 1.0f;
	glm::vec3 spherePos_2 = { 1.0f, 0.5f, -3.0f };
	glm::vec3 sphereColor_2 = { 1.0f, 0.0f, 0.0f };

	glm::vec3 cubeSize = glm::vec3(20.0f, 1.0f, 20.0f);
	glm::vec3 cubePos = { 0.0f, -1.5f, 0.0f };
	glm::vec3 cubeColor = { 0.7f, 0.7f, 0.7f };

	glm::vec3 cubeSize2 = glm::vec3(1.0f);
	glm::vec3 cubePos2 = { -2.5f, 0.5f, 2.0f };
	glm::vec3 cubeColor2 = { 1.0f, 0.0f, 1.0f };

	glm::vec3 pointLightPositions[2] = { {0.0f, 5.0f, 0.0f}, {3.0f, 5.0f, 1.0f} };
	
	Camera camera(window, shader);
#pragma endregion

#pragma region GUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 420");
#pragma endregion

#pragma region Time Variables
	float time = 0.0f;
	float lastTime = 0.0f;
	float dt = 0.0f;
#pragma endregion

	while (!glfwWindowShouldClose(window)) {
#pragma region Time
		time = static_cast<float>(glfwGetTime());
		dt = time - lastTime;
		lastTime = time;
#pragma endregion

#pragma region Inputs
		glfwPollEvents();

		shader.use();
		camera.update(window, shader, dt);

		processInput(window);

		
		if (glfwGetKey(window, GLFW_KEY_W)) pointLightPositions[0].z += 0.1;
		else if (glfwGetKey(window, GLFW_KEY_S)) pointLightPositions[0].z -= 0.1;
		if (glfwGetKey(window, GLFW_KEY_D)) pointLightPositions[0].x -= 0.1;
		else if (glfwGetKey(window, GLFW_KEY_A)) pointLightPositions[0].x += 0.1;

		/*
		if (glfwGetKey(window, GLFW_KEY_W)) cubePos.z += 0.1;
		else if (glfwGetKey(window, GLFW_KEY_S)) cubePos.z -= 0.1;
		if (glfwGetKey(window, GLFW_KEY_D)) cubePos.x += 0.1;
		else if (glfwGetKey(window, GLFW_KEY_A)) cubePos.x -= 0.1;
		if (glfwGetKey(window, GLFW_KEY_Z)) cubePos.y += 0.1;
		else if (glfwGetKey(window, GLFW_KEY_X)) cubePos.y -= 0.1;*/

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowMetricsWindow();

#pragma endregion

#pragma region Render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();

		shader.setFloat("spheres[0].radius", radius);
		shader.setFloat("spheres[0].reflection", 0.0);
		shader.setVec3("spheres[0].center", spherePos);
		shader.setVec3("spheres[0].color", sphereColor);
		shader.setFloat("spheres[1].radius", radius_2);
		shader.setFloat("spheres[0].reflection", 0.0);
		shader.setVec3("spheres[1].center", spherePos_2);
		shader.setVec3("spheres[1].color", sphereColor_2);

		shader.setVec3("cubes[0].halfSize", cubeSize * 0.5f);
		shader.setFloat("cubes[0].reflection", 0.8);
		shader.setVec3("cubes[0].center", cubePos);
		shader.setVec3("cubes[0].color", cubeColor);

		shader.setVec3("cubes[1].halfSize", cubeSize2 * 0.5f);
		shader.setFloat("cubes[1].reflection", 0.0);
		shader.setVec3("cubes[1].center", cubePos2);
		shader.setVec3("cubes[1].color", cubeColor2);

		shader.setVec3("dirLight.direction", 0.0, -1.0, 0.0);
		shader.setVec3("dirLight.color", 1.0f, 1.0f, 1.0f);
		shader.setVec3("dirLight.ambient", 0.06f, 0.06f, 0.06f);
		shader.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);

		shader.setVec3("pointLights[0].position", pointLightPositions[0]);
		shader.setVec3("pointLights[0].color", 1.0f, 1.0f, 1.0f);
		shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLights[0].diffuse", 0.6f, 0.6f, 0.6f);
		shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[0].constant", 1.0f);
		shader.setFloat("pointLights[0].linear", 0.09f);
		shader.setFloat("pointLights[0].quadratic", 0.032f);

		shader.setVec3("pointLights[1].position", pointLightPositions[1]);
		shader.setVec3("pointLights[1].color", 1.0f, 1.0f, 1.0f);
		shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLights[1].diffuse", 0.6f, 0.6f, 0.6f);
		shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[1].constant", 1.0f);
		shader.setFloat("pointLights[1].linear", 0.09f);
		shader.setFloat("pointLights[1].quadratic", 0.032f);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
#pragma endregion
	}
}