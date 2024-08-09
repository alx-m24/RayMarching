#include "GUI.hpp"

GUI::GUI(GLFWwindow* window, Camera& camera, Objects& objects, LightingSystem& lightSys) : objects(objects), lightSys(lightSys), camera(camera)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 420");
}

void GUI::update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowMetricsWindow();
    lightAndCamWindow();
    myObjects();
}

void GUI::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::lightAndCamWindow()
{
	ImGui::Begin("Lighting and Cameras");
	ImGui::BeginTabBar("LightingAndCameras");
	if (ImGui::TabItemButton("Lighting")) lightingWindow = true;
	if (ImGui::TabItemButton("Cameras")) lightingWindow = false;
	ImGui::EndTabBar();

	lightingWindow ? lighting() : cameraWindow();

	ImGui::End();
}

void GUI::lighting()
{
    ImGui::SeparatorText("Directional Light");
    ImGui::BeginChild("Directional Lights", { 0.0f, 115.0f });

    DirectionalLight& dirLight = lightSys.dirLight;
    glm::vec3& dir = dirLight.direction;
    glm::vec3& ambient = dirLight.ambient;
    glm::vec3& diffuse = dirLight.diffuse;
    glm::vec3& specular = dirLight.specular;
    glm::vec3& color = dirLight.color;

    ImGui::DragFloat3("Direction", &dir[0], 0.001f, -1.0f, 1.0f);
    ImGui::ColorEdit3("Ambient", &ambient[0]);
    ImGui::ColorEdit3("Diffuse", &diffuse[0]);
    ImGui::ColorEdit3("Specular", &specular[0]);
    ImGui::ColorEdit3("Color", &color[0]);

    ImGui::EndChild();

    ImGui::SeparatorText("Point Lights");
    ImGui::BeginChild("Point Lights");
    for (int i = 0; i < lightSys.pointLights.size(); ++i) {
        if (i > 0) ImGui::Separator();
        ImGui::BeginChild("PointLight" + i, { 0, 180 });

        PointLight& pointLight = lightSys.pointLights[i];
        glm::vec3& position = pointLight.position;
        glm::vec3& ambient = pointLight.ambient;
        glm::vec3& diffuse = pointLight.diffuse;
        glm::vec3& specular = pointLight.specular;
        glm::vec3& color = pointLight.color;

        ImGui::DragFloat3("Position", &position[0], 0.05f);
        ImGui::ColorEdit3("Ambient", &ambient[0]);
        ImGui::ColorEdit3("Diffuse", &diffuse[0]);
        ImGui::ColorEdit3("Specular", &specular[0]);
        ImGui::ColorEdit3("Color", &color[0]);
        ImGui::DragFloat("Constant", &pointLight.constant, 0.05f);
        ImGui::DragFloat("Linear", &pointLight.linear, 0.05f);
        ImGui::DragFloat("Quadratic", &pointLight.quadratic, 0.05f);

        ImGui::EndChild();
    }
    ImGui::EndChild();
}

void GUI::cameraWindow()
{
    ImGui::BeginChild("Camera");

    glm::vec3& position = camera.Position;
    glm::vec3& rotation = camera.front;

    ImGui::DragFloat3("Position", &position[0], 0.05f);
    ImGui::DragFloat3("Rotation", &rotation[0], 0.05f);

    ImGui::EndChild();
}

void GUI::myObjects()
{
    ImGui::Begin("Objects");

    ImGui::SeparatorText("Spheres");
    ImGui::BeginChild("Spheres", { 0, 95 * (float)objects.spheres.size()});
    for (int i = 0; i < objects.spheres.size(); ++i) {
        if (i > 0) ImGui::Separator();
        ImGui::BeginChild("Sphere" + i, { 0, 90 });

        Sphere& sphere = objects.spheres[i];
        glm::vec3& position = sphere.center;
        glm::vec3& color = sphere.color;

        ImGui::DragFloat3("Position", &position[0], 0.05f);
        ImGui::ColorEdit3("Color", &color[0]);
        ImGui::DragFloat("Radius", &sphere.radius, 0.05f, 0.1f);
        ImGui::DragFloat("Reflection", &sphere.reflection, 0.01f, 0.0f, 1.0f);

        ImGui::EndChild();
    }
    ImGui::EndChild();

    ImGui::SeparatorText("Cubes");
    ImGui::BeginChild("Cubes", { 0, static_cast<float>(140 * objects.cubes.size()) });
    for (int i = 0; i < objects.cubes.size(); ++i) {
        if (i > 0) ImGui::Separator();
        ImGui::BeginChild("Cube" + i, { 0, 140 });

        Cube& cubes = objects.cubes[i];
        glm::vec3& position = cubes.center;
        glm::vec3& rotation = cubes.rotation;
        glm::vec3& halfSize = cubes.size;
        glm::vec3& color = cubes.color;

        ImGui::DragFloat3("Position", &position[0], 0.05f);
        ImGui::DragFloat3("Rotation", &rotation[0], 0.05f);
        ImGui::DragFloat3("Size", &halfSize[0], 0.05f);
        ImGui::ColorEdit3("Color", &color[0]);
        ImGui::DragFloat("Rounding", &cubes.rounding, 0.01f, 0.0f);
        ImGui::DragFloat("Reflection", &cubes.reflection, 0.01f, 0.0f, 1.0f);

        ImGui::EndChild();
    }
    ImGui::EndChild();

    ImGui::SeparatorText("Capsules");
    ImGui::BeginChild("Capsules", { 0, static_cast<float>(160 * objects.capsules.size()) });
    for (int i = 0; i < objects.capsules.size(); ++i) {
        if (i > 0) ImGui::Separator();
        ImGui::BeginChild("Capsule" + i, { 0, 160 });

        Capsule& capsule = objects.capsules[i];
        glm::vec3& position = capsule.center;
        glm::vec3& rotation = capsule.rotation;
        glm::vec3& pos1 = capsule.pos1;
        glm::vec3& pos2 = capsule.pos2;
        glm::vec3& color = capsule.color;

        ImGui::DragFloat3("Position", &position[0], 0.05f);
        ImGui::DragFloat3("Rotation", &rotation[0], 0.05f);
        ImGui::DragFloat3("Pos1", &pos1[0], 0.05f);
        ImGui::DragFloat3("Pos2", &pos2[0], 0.05f);
        ImGui::ColorEdit3("Color", &color[0]);
        ImGui::DragFloat("Radius", &capsule.radius, 0.01f, 0.0f);
        ImGui::DragFloat("Reflection", &capsule.reflection, 0.01f, 0.0f, 1.0f);

        ImGui::EndChild();
    }
    ImGui::EndChild();

    ImGui::End();
}
