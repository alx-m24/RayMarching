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
// My headers
#include "Headers/Shaders/Shader.hpp"
#include "Headers/LightingSystem.hpp"
#include "Headers/IO/Input.hpp"
#include "Headers/Objects.hpp"
#include "Headers/Camera.hpp"
#include "Headers/GUI.hpp"

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

	objects.addCube({ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, {9.88f, 0.2f, 15.03f }, { 0.501f, 0.361f, 0.204f }, 0.0f, 0.0f });
	objects.addCube({ { -8.775f, 3.2f, 14.825f }, { 0.0f, 0.0f, 0.0f }, {1.100f, 3.0f, 0.2f }, { 0.854f, 0.961f, 0.322f }, 0.0f, 0.0f });
	objects.addCube({ { -6.170, 3.2f, 14.825f }, { 0.0f, 0.0f, 0.0f }, {1.500f, 1.47f, 0.2f }, { 1.0f, 1.0f, 1.0f }, 0.0f, 0.0f });
	objects.addCube({ { -2.650, 3.2f, 14.825f }, { 0.0f, 0.0f, 0.0f }, {2.00f, 3.0f, 0.2f }, { 0.854f, 0.961f, 0.322f }, 0.0f, 0.0f });
	objects.addCube({ { -6.170, 5.450, 14.825f }, { 0.0f, 0.0f, 0.0f }, {1.500, 0.750, 0.2f }, { 0.854f, 0.961f, 0.322f }, 0.0f, 0.0f });

	objects.addCapsule({ { 0.0f, 2.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f,1.0f,-2.5f }, { 1.0f,1.0f,2.5f }, { 1.0f,1.0f,1.0f }, 0.0f, 1.0f });

	LightingSystem lightSys;
	lightSys.addPointLight(PointLight({ 0.0f, 5.0f, 0.0f }));
	lightSys.addPointLight(PointLight({ 3.0f, 5.0f, 1.0f }));
	
	Camera camera(window, shader);
#pragma endregion

#pragma region GUI
	GUI gui(window, camera, objects, lightSys);
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

		gui.update();

		processInput(window);
#pragma endregion

#pragma region Render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();
		/*
		for (Cube& cube : objects.cubes) {
			cube.center.x *= rand() % (int)time;
			cube.rotation.z *= rand() % (int)sin(time);
		}*/

		objects.update(shader);
		lightSys.update(shader);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		gui.render();

		glfwSwapBuffers(window);
#pragma endregion
	}
	glfwTerminate();

	return EXIT_SUCCESS;
}