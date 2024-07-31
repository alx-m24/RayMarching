#include "Objects.hpp"

glm::mat4 getMatrix(Cube& cube)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, cube.center);
	model = glm::rotate(model, cube.rotation.w, { cube.rotation.x, cube.rotation.y, cube.rotation.z });

	return model;
}

glm::mat4 getMatrix(Capsule& capsule)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, capsule.center);
	model = glm::rotate(model, capsule.rotation.w, { capsule.rotation.x, capsule.rotation.y, capsule.rotation.z });

	return model;
}

void Objects::update(Shader& shader)
{
	for (int i = 0; i < spheres.size(); ++i) {
		std::string name = "spheres[" + std::to_string(i) + "].";
		Sphere& sphere = spheres[i];

		shader.setFloat(name + "radius", sphere.radius);
		shader.setFloat(name + "reflection", sphere.reflection);
		shader.setVec3(name + "center", sphere.center);
		shader.setVec3(name + "color", sphere.color);
	}
	
	for (int i = 0; i < cubes.size(); ++i) {
		std::string name = "cubes[" + std::to_string(i) + "].";
		Cube& cube = cubes[i];

		shader.setVec3(name + "halfSize", cube.halfSize);
		shader.setFloat(name + "reflection", cube.reflection);
		shader.setFloat(name + "rounding", cube.rounding);
		shader.setVec3(name + "color", cube.color);
		shader.setMat4(name + "inverseTransormation", glm::inverse(getMatrix(cube)));
	}
	for (int i = 0; i < capsules.size(); ++i) {
		std::string name = "capsules[" + std::to_string(i) + "].";
		Capsule& capsule = capsules[i];

		shader.setVec3(name + "pos1", capsule.pos1);
		shader.setVec3(name + "pos2", capsule.pos2);
		shader.setFloat(name + "reflection", capsule.reflection);
		shader.setFloat(name + "radius", capsule.radius);
		shader.setVec3(name + "color", capsule.color);
		shader.setMat4(name + "inverseTransormation", glm::inverse(getMatrix(capsule)));
	}
}

void Objects::addSphere(Sphere sphere)
{
	spheres.emplace_back(sphere);
}

void Objects::addCube(Cube cube)
{
	cubes.emplace_back(cube);
}

void Objects::addCapsule(Capsule capsule)
{
	capsules.emplace_back(capsule);
}
