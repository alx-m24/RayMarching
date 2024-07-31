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
#include "Headers/LightingSystem.hpp"
#include "Headers/IO/Input.hpp"
#include "Headers/Objects.hpp"
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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RayMarching", nullptr, nullptr);
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
	Objects objects;
	objects.addSphere({ 1.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, 0.0f });
	objects.addSphere({ 0.58f, { 1.0f, 0.5f, -3.0f }, { 1.0f, 0.0f, 0.0f }, 0.0f });

	objects.addCube({ { 0.0f, -1.5f, 0.0f }, { 1.0f, 1.0f, 1.0f, 0.0f }, {10.0f, 0.5f, 10.0f }, { 0.0f, 1.0f, 0.0f }, 0.0f, 0.0f });
	objects.addCube({ { -2.5f, 0.5f, 2.0f }, { 1.0f, 1.0f, 1.0f, 0.0f }, {0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f }, 0.0f, 0.5f });

	objects.addCapsule({ { 0.0f, 2.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 0.0f }, { 1.0f,1.0f,-2.5f }, { 1.0f,1.0f,2.5f }, { 1.0f,1.0f,1.0f }, 0.0f, 1.0f });

	LightingSystem lightSys;
	lightSys.addPointLight(PointLight({ 0.0f, 5.0f, 0.0f }));
	lightSys.addPointLight(PointLight({ 3.0f, 5.0f, 1.0f }));
	
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

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowMetricsWindow();

#pragma endregion

#pragma region Render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();

		objects.cubes[1].center.y = sin(time) + 1.5f;
		objects.capsules[0].rotation.w = time;

		objects.update(shader);
		lightSys.update(shader);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
#pragma endregion
	}
	glfwTerminate();

	return EXIT_SUCCESS;
}