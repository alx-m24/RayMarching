#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include <vector>
#include "LightingSystem.hpp"
#include "Objects.hpp"
#include "Camera.hpp"

class GUI {
private:
	Objects& objects;
	LightingSystem& lightSys;
	Camera& camera;

public:
	GUI(GLFWwindow* window, Camera& camera, Objects& objects, LightingSystem& lightSys);

	void update();
	void render();

private:
	bool lightingWindow = true;

	void lightAndCamWindow();
	void lighting();
	void cameraWindow();
	void myObjects();
};

