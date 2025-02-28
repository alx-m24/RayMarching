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
	objects.addSphere({ 1.0f, {-2.5f, 2.150f, -2.100f}, {0.0f, 1.0f, 1.0f}, 0.340f });
	objects.addSphere({ 0.58f, { 0.7f, 1.9f, -7.2f }, { 1.0f, 0.0f, 0.0f }, 0.210f });

	objects.addCube({ { -0.750f, 3.38f, 4.0f }, { 0.0f, 0.0f, 0.0f }, {1.0f, 1.0f, 1.0f }, { 0.501f, 0.361f, 0.204f }, 0.190f, 0.690f });
	objects.addCube({ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, {9.88f, 0.2f, 15.03f }, { 0.568f, 0.568f, 0.568f }, 0.17f, 0.0f });

	objects.addCapsule({ { 4.8f, 2.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f,1.0f,-2.5f }, { 1.0f,1.0f,2.5f }, { 0.352f,1.0f,0.0f }, 0.4f, 1.0f });

	LightingSystem lightSys;
	lightSys.addPointLight(PointLight({ -8.85f, 1.8f, -9.6f }));
	lightSys.addPointLight(PointLight({ 7.25f, 5.0f, 10.0f }));
	
	Camera camera(window, shader);
	camera.Position = glm::vec3(9.0, 12.0f, -15.0f);
	camera.front = glm::vec3(-0.5f, -0.4f, 0.7f);
#pragma endregion
#pragma region GUI
	//GUI gui(window, camera, objects, lightSys);
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

		//gui.update();

		processInput(window);
#pragma endregion

#pragma region Render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();
		
		int i = 0;
		for (Sphere& sphere : objects.spheres) {
			sphere.center.y = sin(time + i * 2) + 3.0f;
			++i;
		}

		Cube& cube = objects.cubes[0];
		cube.rotation.z += dt * 45.0f;
		cube.rotation.y += dt * -25.0f;

		objects.capsules[0].radius = 0.8f * sin(time) + 1.0f;

		i = 0;
		for (PointLight& light : lightSys.pointLights) {
			light.position.x = sin(time + i * glm::radians(180.0f)) * 5.0f;
			if (i == 0) {
				light.position.y = sin(2.0f * time) * 2.0f + 3.8f;
			}
			++i;
		}

		glm::vec3 center = glm::vec3(0.0f);
		float radius = 15.0f;
		float angle = time * 10.0f;

		camera.Position.x = radius * cos(glm::radians(angle));
		camera.Position.y = sin(glm::radians(angle)) * 8.0f + 10.0f;
		camera.Position.z = radius * sin(glm::radians(angle));

		camera.front = glm::normalize(center - camera.Position);

		objects.update(shader);
		lightSys.update(shader);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//gui.render();

		glfwSwapBuffers(window);
#pragma endregion
	}
	glfwTerminate();

	return EXIT_SUCCESS;
}