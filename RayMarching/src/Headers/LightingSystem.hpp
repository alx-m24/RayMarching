#pragma once
#include <vector>
#include "Shaders/Shader.hpp"

struct DirectionalLight {
	glm::vec3 direction = { 0.2f, -1.0f, -0.15 };
	glm::vec3 ambient = { 0.06f, 0.06f, 0.06f };
	glm::vec3 diffuse = { 0.6f, 0.6f, 0.6f };
	glm::vec3 specular = { 0.0f, 0.0f, 0.0f };
	glm::vec3 color = { 1.0f, 1.0f, 1.0f };
};

struct PointLight {
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 ambient = { 0.05f, 0.05f, 0.05f };
	glm::vec3 diffuse = { 0.8f, 0.8f, 0.8f };
	glm::vec3 specular = { 1.0f, 1.0f, 1.0f };
	glm::vec3 color =  { 1.0f, 1.0f, 1.0f };
	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;
};

class LightingSystem {
public:
	DirectionalLight dirLight;
	std::vector<PointLight> pointLights;

	void update(Shader& shader);
	void addPointLight(PointLight pointlight);
};

