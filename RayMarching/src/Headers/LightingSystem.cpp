#include "LightingSystem.hpp"

void LightingSystem::update(Shader& shader)
{
	
	shader.setVec3("dirLight.direction", dirLight.direction);
	shader.setVec3("dirLight.color", dirLight.color);
	shader.setVec3("dirLight.ambient", dirLight.ambient);
	shader.setVec3("dirLight.diffuse", dirLight.diffuse);
	shader.setVec3("dirLight.specular", dirLight.specular);

	for (int i = 0; i < pointLights.size(); ++i) {
		std::string name = "pointLights[" + std::to_string(i) + "].";
		PointLight& pointLight = pointLights[i];

		shader.setVec3(name + "position", pointLight.position);
		shader.setVec3(name + "ambient", pointLight.ambient);
		shader.setVec3(name + "diffuse", pointLight.diffuse);
		shader.setVec3(name + "specular", pointLight.specular);
		shader.setVec3(name + "color", pointLight.color);
		shader.setFloat(name + "constant", pointLight.constant);
		shader.setFloat(name + "linear", pointLight.linear);
		shader.setFloat(name + "quadratic", pointLight.quadratic);
	}
}

void LightingSystem::addPointLight(PointLight pointlight)
{
	pointLights.emplace_back(pointlight);;
}