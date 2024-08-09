#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Shaders/Shader.hpp"

struct Sphere {
	float radius = 1.0f;
	glm::vec3 center = glm::vec3(0.0f);
	glm::vec3 color = glm::vec3(1.0f);
	float reflection = 0.0f;
};

struct Cube {
	glm::vec3 center = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);

	glm::vec3 size = glm::vec3(1.0f);
	glm::vec3 color = glm::vec3(1.0f);
	float reflection = 0.0f;
	float rounding = 0.0f;
};

struct Capsule {
	glm::vec3 center = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);

	glm::vec3 pos1 = glm::vec3(0.0f);
	glm::vec3 pos2 = glm::vec3(0.0f);
	glm::vec3 color = glm::vec3(1.0f);
	float reflection = 0.0f;
	float radius = 1.0f;
};

glm::mat4 getMatrix(Cube& cube);
glm::mat4 getMatrix(Capsule& capsule);

class Objects {
public:
	std::vector<Sphere> spheres;
	std::vector<Cube> cubes;
	std::vector<Capsule> capsules;

public:
	void update(Shader& shader);

	void addSphere(Sphere sphere);
	void addCube(Cube cube);
	void addCapsule(Capsule capsule);
};
